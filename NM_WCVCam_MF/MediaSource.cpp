
#include "pch.h"
#include "Undocumented.h"
#include "Tools.h"
#include "MediaSource.h"

HRESULT MediaSource::Initialize(IMFAttributes* attributes)
{
    if (attributes)
    {
        RETURN_IF_FAILED(attributes->CopyAllItems(this));
    }

    wil::com_ptr_nothrow<IMFSensorProfileCollection> collection;
    RETURN_IF_FAILED(MFCreateSensorProfileCollection(&collection));

    DWORD streamId = 0;
    wil::com_ptr_nothrow<IMFSensorProfile> profile;
    RETURN_IF_FAILED(MFCreateSensorProfile(KSCAMERAPROFILE_Legacy, 0, nullptr, &profile));
    RETURN_IF_FAILED(profile->AddProfileFilter(streamId, L"((RES==;FRT<=30,1;SUT==))"));
    RETURN_IF_FAILED(collection->AddProfile(profile.get()));

    RETURN_IF_FAILED(MFCreateSensorProfile(KSCAMERAPROFILE_HighFrameRate, 0, nullptr, &profile));
    RETURN_IF_FAILED(profile->AddProfileFilter(streamId, L"((RES==;FRT>=60,1;SUT==))"));
    RETURN_IF_FAILED(collection->AddProfile(profile.get()));
    RETURN_IF_FAILED(SetUnknown(MF_DEVICEMFT_SENSORPROFILE_COLLECTION, collection.get()));

    try
    {
        auto appInfo = winrt::Windows::ApplicationModel::AppInfo::Current();
        if (appInfo)
        {
            RETURN_IF_FAILED(SetString(MF_VIRTUALCAMERA_CONFIGURATION_APP_PACKAGE_FAMILY_NAME, appInfo.PackageFamilyName().data()));
        }
    }
    catch (...)
    {
    }

    auto streams = wil::make_unique_cotaskmem_array<wil::com_ptr_nothrow<IMFStreamDescriptor>>(_streams.size());
    for (uint32_t i = 0; i < streams.size(); i++)
    {
        wil::com_ptr_nothrow<IMFStreamDescriptor> desc;
        RETURN_IF_FAILED(_streams[i]->GetStreamDescriptor(&desc));
        streams[i] = desc.detach();
    }
    RETURN_IF_FAILED(MFCreatePresentationDescriptor((DWORD)streams.size(), streams.get(), &_descriptor));
    RETURN_IF_FAILED(MFCreateEventQueue(&_queue));
    return S_OK;
}

int MediaSource::GetStreamIndexById(DWORD id)
{
    for (uint32_t i = 0; i < _streams.size(); i++)
    {
        wil::com_ptr_nothrow<IMFStreamDescriptor> desc;
        if (FAILED(_streams[i]->GetStreamDescriptor(&desc)))
            return -1;

        DWORD sid = 0;
        if (FAILED(desc->GetStreamIdentifier(&sid)))
            return -1;

        if (sid == id)
            return i;
    }
    return -1;
}

// IMFMediaEventGenerator
STDMETHODIMP MediaSource::BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState)
{
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue);

    RETURN_IF_FAILED(_queue->BeginGetEvent(pCallback, punkState));
    return S_OK;
}

STDMETHODIMP MediaSource::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    RETURN_HR_IF_NULL(E_POINTER, ppEvent);
    *ppEvent = nullptr;
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue);

    RETURN_IF_FAILED(_queue->EndGetEvent(pResult, ppEvent));
    return S_OK;
}

STDMETHODIMP MediaSource::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    RETURN_HR_IF_NULL(E_POINTER, ppEvent);
    *ppEvent = nullptr;
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue);

    RETURN_IF_FAILED(_queue->GetEvent(dwFlags, ppEvent));
    return S_OK;
}

STDMETHODIMP MediaSource::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue)
{
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue);

    RETURN_IF_FAILED(_queue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue));
    return S_OK;
}

// IMFMediaSource
STDMETHODIMP MediaSource::CreatePresentationDescriptor(IMFPresentationDescriptor** ppPresentationDescriptor)
{
    RETURN_HR_IF_NULL(E_POINTER, ppPresentationDescriptor);
    *ppPresentationDescriptor = nullptr;
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_descriptor);
    
    RETURN_IF_FAILED(_descriptor->Clone(ppPresentationDescriptor));
    return S_OK;
}

STDMETHODIMP MediaSource::GetCharacteristics(DWORD* pdwCharacteristics)
{
    RETURN_HR_IF_NULL(E_POINTER, pdwCharacteristics);

    *pdwCharacteristics = MFMEDIASOURCE_IS_LIVE;
    return S_OK;
}

STDMETHODIMP MediaSource::Pause()
{
    RETURN_HR(MF_E_INVALID_STATE_TRANSITION);
}

STDMETHODIMP MediaSource::Shutdown()
{
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue);

    LOG_IF_FAILED_MSG(_queue->Shutdown(), "Queue shutdown failed");
    _queue.reset();

    for (uint32_t i = 0; i < _streams.size(); i++)
    {
        _streams[i]->Shutdown();
    }

    _descriptor.reset();
    _attributes.reset();
    return S_OK;
}

STDMETHODIMP MediaSource::Start(IMFPresentationDescriptor* pPresentationDescriptor, const GUID* pguidTimeFormat, const PROPVARIANT* pvarStartPosition)
{
    RETURN_HR_IF_NULL(E_POINTER, pPresentationDescriptor);
    RETURN_HR_IF_NULL(E_POINTER, pvarStartPosition);
    RETURN_HR_IF_MSG(E_INVALIDARG, pguidTimeFormat && *pguidTimeFormat != GUID_NULL, "Unsupported guid time format");
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue || !_descriptor);

    DWORD count;
    RETURN_IF_FAILED(pPresentationDescriptor->GetStreamDescriptorCount(&count));
    RETURN_HR_IF_MSG(E_INVALIDARG, count != (DWORD)_streams.size(), "Invalid number of descriptor streams");

    wil::unique_prop_variant time;
    RETURN_IF_FAILED(InitPropVariantFromInt64(MFGetSystemTime(), &time));

    for (DWORD i = 0; i < count; i++)
    {
        wil::com_ptr_nothrow<IMFStreamDescriptor> desc;
        BOOL selected = FALSE;
        RETURN_IF_FAILED(pPresentationDescriptor->GetStreamDescriptorByIndex(i, &selected, &desc));

        DWORD id = 0;
        RETURN_IF_FAILED(desc->GetStreamIdentifier(&id));

        auto index = GetStreamIndexById(id);
        RETURN_HR_IF(E_FAIL, index < 0);

        BOOL thisSelected = FALSE;
        wil::com_ptr_nothrow<IMFStreamDescriptor> thisDesc;
        RETURN_IF_FAILED(_descriptor->GetStreamDescriptorByIndex(index, &thisSelected, &thisDesc));

        MF_STREAM_STATE state;
        RETURN_IF_FAILED(_streams[i]->GetStreamState(&state));
        if (thisSelected && state == MF_STREAM_STATE_STOPPED )
        {
            thisSelected = FALSE;
        }
        else if (!thisSelected && state != MF_STREAM_STATE_STOPPED)
        {
            thisSelected = TRUE;
        }

        if (selected != thisSelected)
        {
            if (selected)
            {
                RETURN_IF_FAILED(_descriptor->SelectStream(index));

                wil::com_ptr_nothrow<IUnknown> unk;
                RETURN_IF_FAILED(_streams[index].copy_to(&unk));
                RETURN_IF_FAILED(_queue->QueueEventParamUnk(MENewStream, GUID_NULL, S_OK, unk.get()));

                wil::com_ptr_nothrow<IMFMediaTypeHandler> handler;
                wil::com_ptr_nothrow<IMFMediaType> type;
                RETURN_IF_FAILED(desc->GetMediaTypeHandler(&handler));
                RETURN_IF_FAILED(handler->GetCurrentMediaType(&type));

                RETURN_IF_FAILED(_streams[index]->Start(type.get()));
            }
            else
            {
                RETURN_IF_FAILED(_descriptor->DeselectStream(index));
                RETURN_IF_FAILED(_streams[index]->Stop());
            }
        }
    }

    RETURN_IF_FAILED(_queue->QueueEventParamVar(MESourceStarted, GUID_NULL, S_OK, &time));
    return S_OK;
}

STDMETHODIMP MediaSource::Stop()
{
    winrt::slim_lock_guard lock(_lock);
    RETURN_HR_IF(MF_E_SHUTDOWN, !_queue || !_descriptor);

    wil::unique_prop_variant time;
    RETURN_IF_FAILED(InitPropVariantFromInt64(MFGetSystemTime(), &time));

    for (DWORD i = 0; i < _streams.size(); i++)
    {
        RETURN_IF_FAILED(_streams[i]->Stop());
        RETURN_IF_FAILED(_descriptor->DeselectStream(i));
    }

    RETURN_IF_FAILED(_queue->QueueEventParamVar(MESourceStopped, GUID_NULL, S_OK, &time));
    return S_OK;
}

// IMFMediaSourceEx
STDMETHODIMP MediaSource::GetSourceAttributes(IMFAttributes** ppAttributes)
{
    RETURN_HR_IF_NULL(E_POINTER, ppAttributes);
    winrt::slim_lock_guard lock(_lock);

    RETURN_IF_FAILED(QueryInterface(IID_PPV_ARGS(ppAttributes)));
    return S_OK;
}

// IMFMediaSource2
STDMETHODIMP MediaSource::SetMediaType(DWORD dwStreamID, IMFMediaType* pMediaType)
{
    RETURN_HR_IF_NULL(E_POINTER, pMediaType);
    winrt::slim_lock_guard lock(_lock);

    return S_OK;
}

STDMETHODIMP MediaSource::GetStreamAttributes(DWORD dwStreamIdentifier, IMFAttributes** ppAttributes)
{
    RETURN_HR_IF_NULL(E_POINTER, ppAttributes);
    *ppAttributes = nullptr;
    winrt::slim_lock_guard lock(_lock);

    RETURN_HR_IF_MSG(E_FAIL, dwStreamIdentifier >= _streams.size(), "dwStreamIdentifier %u is invalid", dwStreamIdentifier);
    RETURN_IF_FAILED(_streams[dwStreamIdentifier].copy_to(ppAttributes));
    return S_OK;
}

STDMETHODIMP MediaSource::SetD3DManager(IUnknown* pManager)
{
    RETURN_HR_IF_NULL(E_POINTER, pManager);
    winrt::slim_lock_guard lock(_lock);

    for (DWORD i = 0; i < _streams.size(); i++)
    {
        RETURN_IF_FAILED(_streams[i]->SetD3DManager(pManager));
    }
    return S_OK;
}

// IMFGetService
STDMETHODIMP MediaSource::GetService(REFGUID siid, REFIID iid, LPVOID* ppvObject)
{
    if (iid == __uuidof(IMFDeviceController) || iid == __uuidof(IMFDeviceController2))
        return MF_E_UNSUPPORTED_SERVICE;

    RETURN_HR(MF_E_UNSUPPORTED_SERVICE);
}

// IMFSampleAllocatorControl
STDMETHODIMP MediaSource::SetDefaultAllocator(DWORD dwOutputStreamID, IUnknown* pAllocator)
{
    RETURN_HR_IF_NULL(E_POINTER, pAllocator);
    winrt::slim_lock_guard lock(_lock);

    auto index = GetStreamIndexById(dwOutputStreamID);
    RETURN_HR_IF(E_FAIL, index < 0);

    RETURN_HR_IF_MSG(E_FAIL, index < 0 || (DWORD)index >= _streams.size(), "dwOutputStreamID %u is invalid, index:%i", dwOutputStreamID, index);
    RETURN_HR(_streams[index]->SetAllocator(pAllocator));
}

STDMETHODIMP MediaSource::GetAllocatorUsage(DWORD dwOutputStreamID, DWORD* pdwInputStreamID, MFSampleAllocatorUsage* peUsage)
{
    RETURN_HR_IF_NULL(E_POINTER, peUsage);
    RETURN_HR_IF_NULL(E_POINTER, pdwInputStreamID);
    winrt::slim_lock_guard lock(_lock);

    auto index = GetStreamIndexById(dwOutputStreamID);
    RETURN_HR_IF(E_FAIL, index < 0);

    RETURN_HR_IF_MSG(E_FAIL, index < 0 || (DWORD)index >= _streams.size(), "dwOutputStreamID %u is invalid, index:%i", dwOutputStreamID, index);
    *pdwInputStreamID = dwOutputStreamID;
    *peUsage = _streams[index]->GetAllocatorUsage();
    return S_OK;
}

// IKsControl
STDMETHODIMP_(NTSTATUS) MediaSource::KsProperty(PKSPROPERTY property, ULONG length, LPVOID data, ULONG dataLength, ULONG* bytesReturned)
{
    RETURN_HR_IF_NULL(E_POINTER, property);
    RETURN_HR_IF_NULL(E_POINTER, bytesReturned);
    winrt::slim_lock_guard lock(_lock);

    return HRESULT_FROM_WIN32(ERROR_SET_NOT_FOUND);
}

STDMETHODIMP_(NTSTATUS) MediaSource::KsMethod(PKSMETHOD method, ULONG length, LPVOID data, ULONG dataLength, ULONG* bytesReturned)
{
    RETURN_HR_IF_NULL(E_POINTER, method);
    RETURN_HR_IF_NULL(E_POINTER, bytesReturned);
    winrt::slim_lock_guard lock(_lock);

    return HRESULT_FROM_WIN32(ERROR_SET_NOT_FOUND);
}

STDMETHODIMP_(NTSTATUS) MediaSource::KsEvent(PKSEVENT evt, ULONG length, LPVOID data, ULONG dataLength, ULONG* bytesReturned)
{
    RETURN_HR_IF_NULL(E_POINTER, bytesReturned);
    winrt::slim_lock_guard lock(_lock);

    return HRESULT_FROM_WIN32(ERROR_SET_NOT_FOUND);
}
