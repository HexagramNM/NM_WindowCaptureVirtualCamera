﻿
#include "pch.h"
#include "Tools.h"
#include "MediaStream.h"

#include "../global_config.h"

HRESULT MediaStream::Initialize(IMFMediaSource* source, int index)
{
    RETURN_HR_IF_NULL(E_POINTER, source);
    _source = source;
    _index = index;

    RETURN_IF_FAILED(SetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, PINNAME_VIDEO_CAPTURE));
    RETURN_IF_FAILED(SetUINT32(MF_DEVICESTREAM_STREAM_ID, index));
    RETURN_IF_FAILED(SetUINT32(MF_DEVICESTREAM_FRAMESERVER_SHARED, 1));
    RETURN_IF_FAILED(SetUINT32(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES, MFFrameSourceTypes::MFFrameSourceTypes_Color));

    RETURN_IF_FAILED(MFCreateEventQueue(&_queue));

    // set 1 here to force RGB32 only
    auto types = wil::make_unique_cotaskmem_array<wil::com_ptr_nothrow<IMFMediaType>>(2);

    wil::com_ptr_nothrow<IMFMediaType> rgbType;
    RETURN_IF_FAILED(MFCreateMediaType(&rgbType));
    rgbType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    rgbType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    MFSetAttributeSize(rgbType.get(), MF_MT_FRAME_SIZE, VCAM_VIDEO_WIDTH, VCAM_VIDEO_HEIGHT);
    rgbType->SetUINT32(MF_MT_DEFAULT_STRIDE, VCAM_VIDEO_WIDTH * 4);
    rgbType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    rgbType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
    MFSetAttributeRatio(rgbType.get(), MF_MT_FRAME_RATE, 30, 1);
    auto bitrate = (uint32_t)(VCAM_VIDEO_WIDTH * VCAM_VIDEO_HEIGHT * 4 * 8 * 30);
    rgbType->SetUINT32(MF_MT_AVG_BITRATE, bitrate);
    MFSetAttributeRatio(rgbType.get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    types[0] = rgbType.detach();

    if (types.size() > 1)
    {
        wil::com_ptr_nothrow<IMFMediaType> nv12Type;
        RETURN_IF_FAILED(MFCreateMediaType(&nv12Type));
        nv12Type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        nv12Type->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
        nv12Type->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
        nv12Type->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
        MFSetAttributeSize(nv12Type.get(), MF_MT_FRAME_SIZE, VCAM_VIDEO_WIDTH, VCAM_VIDEO_HEIGHT);
        nv12Type->SetUINT32(MF_MT_DEFAULT_STRIDE, (UINT)(VCAM_VIDEO_WIDTH * 1.5));
        MFSetAttributeRatio(nv12Type.get(), MF_MT_FRAME_RATE, 30, 1);
        // frame size * pixel bit size * framerate
        bitrate = (uint32_t)(VCAM_VIDEO_WIDTH * 1.5 * VCAM_VIDEO_HEIGHT * 8 * 30);
        nv12Type->SetUINT32(MF_MT_AVG_BITRATE, bitrate);
        MFSetAttributeRatio(nv12Type.get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
        types[1] = nv12Type.detach();
    }

    RETURN_IF_FAILED_MSG(MFCreateStreamDescriptor(_index, (DWORD)types.size(), types.get(), &_descriptor), "MFCreateStreamDescriptor failed");

    wil::com_ptr_nothrow<IMFMediaTypeHandler> handler;
    RETURN_IF_FAILED(_descriptor->GetMediaTypeHandler(&handler));
    RETURN_IF_FAILED(handler->SetCurrentMediaType(types[0]));

    return S_OK;
}

HRESULT MediaStream::Start(IMFMediaType* type)
{
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue || !_allocator);

    if (type)
    {
        RETURN_IF_FAILED(type->GetGUID(MF_MT_SUBTYPE, &_format));
    }

    // at this point, set D3D manager may have not been called
    // so we want to create a D2D1 renter target anyway
    RETURN_IF_FAILED(_generator.EnsureRenderTarget(VCAM_VIDEO_WIDTH, VCAM_VIDEO_HEIGHT));

    RETURN_IF_FAILED(_allocator->InitializeSampleAllocator(10, type));
    RETURN_IF_FAILED(_queue->QueueEventParamVar(MEStreamStarted, GUID_NULL, S_OK, nullptr));
    _state = MF_STREAM_STATE_RUNNING;
    return S_OK;
}

HRESULT MediaStream::Stop()
{
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue || !_allocator);

    RETURN_IF_FAILED(_allocator->UninitializeSampleAllocator());
    RETURN_IF_FAILED(_queue->QueueEventParamVar(MEStreamStopped, GUID_NULL, S_OK, nullptr));
    _state = MF_STREAM_STATE_STOPPED;
    return S_OK;
}

MFSampleAllocatorUsage MediaStream::GetAllocatorUsage()
{
    return MFSampleAllocatorUsage_UsesProvidedAllocator;
}

HRESULT MediaStream::SetAllocator(IUnknown* allocator)
{
    RETURN_HR_IF_NULL(E_POINTER, allocator);
    _allocator.reset();
    RETURN_HR(allocator->QueryInterface(&_allocator));
}

HRESULT MediaStream::SetD3DManager(IUnknown* manager)
{
    RETURN_HR_IF_NULL(E_POINTER, manager);

    return S_OK;
}

void MediaStream::Shutdown()
{
    if (_queue)
    {
        LOG_IF_FAILED_MSG(_queue->Shutdown(), "Queue shutdown failed");
        _queue.reset();
    }

    _descriptor.reset();
    _source.reset();
    _attributes.reset();
}

// IMFMediaEventGenerator
STDMETHODIMP MediaStream::BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState)
{
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue);

    RETURN_IF_FAILED(_queue->BeginGetEvent(pCallback, punkState));
    return S_OK;
}

STDMETHODIMP MediaStream::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    RETURN_HR_IF_NULL(E_POINTER, ppEvent);
    *ppEvent = nullptr;
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue);

    RETURN_IF_FAILED(_queue->EndGetEvent(pResult, ppEvent));
    return S_OK;
}

STDMETHODIMP MediaStream::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    RETURN_HR_IF_NULL(E_POINTER, ppEvent);
    *ppEvent = nullptr;
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue);

    RETURN_IF_FAILED(_queue->GetEvent(dwFlags, ppEvent));
    return S_OK;
}

STDMETHODIMP MediaStream::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue)
{
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue);

    RETURN_IF_FAILED(_queue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue));
    return S_OK;
}

// IMFMediaStream
STDMETHODIMP MediaStream::GetMediaSource(IMFMediaSource** ppMediaSource)
{
    RETURN_HR_IF_NULL(E_POINTER, ppMediaSource);
    *ppMediaSource = nullptr;
    RETURN_HR_IF(MF_E_SHUTDOWN, !_source);

    RETURN_IF_FAILED(_source.copy_to(ppMediaSource));
    return S_OK;
}

STDMETHODIMP MediaStream::GetStreamDescriptor(IMFStreamDescriptor** ppStreamDescriptor)
{
    RETURN_HR_IF_NULL(E_POINTER, ppStreamDescriptor);
    *ppStreamDescriptor = nullptr;
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_descriptor);

    RETURN_IF_FAILED(_descriptor.copy_to(ppStreamDescriptor));
    return S_OK;
}

STDMETHODIMP MediaStream::RequestSample(IUnknown* pToken)
{
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_allocator || !_queue);

    wil::com_ptr_nothrow<IMFSample> sample;
    RETURN_IF_FAILED(_allocator->AllocateSample(&sample));
    RETURN_IF_FAILED(sample->SetSampleTime(MFGetSystemTime()));
    RETURN_IF_FAILED(sample->SetSampleDuration(333333));

    // generate frame
    wil::com_ptr_nothrow<IMFSample> outSample;
    RETURN_IF_FAILED(_generator.Generate(sample.get(), _format, &outSample));

    if (pToken)
    {
        RETURN_IF_FAILED(outSample->SetUnknown(MFSampleExtension_Token, pToken));
    }
    RETURN_IF_FAILED(_queue->QueueEventParamUnk(MEMediaSample, GUID_NULL, S_OK, outSample.get()));
    return S_OK;
}

// IMFMediaStream2
STDMETHODIMP MediaStream::SetStreamState(MF_STREAM_STATE value)
{
    if (_state = value)
        return S_OK;
    switch (value)
    {
    case MF_STREAM_STATE_PAUSED:
        if (_state != MF_STREAM_STATE_RUNNING)
            RETURN_HR(MF_E_INVALID_STATE_TRANSITION);

        _state = value;
        break;

    case MF_STREAM_STATE_RUNNING:
        RETURN_IF_FAILED(Start(nullptr));
        break;

    case MF_STREAM_STATE_STOPPED:
        RETURN_IF_FAILED(Stop());
        break;

    default:
        RETURN_HR(MF_E_INVALID_STATE_TRANSITION);
        break;
    }
    return S_OK;
}

STDMETHODIMP MediaStream::GetStreamState(MF_STREAM_STATE* value)
{
    RETURN_HR_IF_NULL(E_POINTER, value);
    *value = _state;
    return S_OK;
}

// IKsControl
STDMETHODIMP_(NTSTATUS) MediaStream::KsProperty(PKSPROPERTY property, ULONG length, LPVOID data, ULONG dataLength, ULONG* bytesReturned)
{
    RETURN_HR_IF_NULL(E_POINTER, property);
    RETURN_HR_IF_NULL(E_POINTER, bytesReturned);
    winrt::slim_lock_guard lock(_lock);

    return HRESULT_FROM_WIN32(ERROR_SET_NOT_FOUND);
}

STDMETHODIMP_(NTSTATUS) MediaStream::KsMethod(PKSMETHOD method, ULONG length, LPVOID data, ULONG dataLength, ULONG* bytesReturned)
{
    RETURN_HR_IF_NULL(E_POINTER, method);
    RETURN_HR_IF_NULL(E_POINTER, bytesReturned);
    winrt::slim_lock_guard lock(_lock);

    return HRESULT_FROM_WIN32(ERROR_SET_NOT_FOUND);
}

STDMETHODIMP_(NTSTATUS) MediaStream::KsEvent(PKSEVENT evt, ULONG length, LPVOID data, ULONG dataLength, ULONG* bytesReturned)
{
    RETURN_HR_IF_NULL(E_POINTER, bytesReturned);
    winrt::slim_lock_guard lock(_lock);

    return HRESULT_FROM_WIN32(ERROR_SET_NOT_FOUND);
}
