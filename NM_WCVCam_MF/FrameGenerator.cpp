
#include "pch.h"
#include "MFTools.h"
#include "FrameGenerator.h"
#include "../global_config.h"

HRESULT FrameGenerator::SetupD3D11Device() 
{
    UINT resetToken;
    RETURN_IF_FAILED(MFCreateDXGIDeviceManager(&resetToken, _dxgiManager.put()));

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    createDeviceFlags |= D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    wil::com_ptr_nothrow<ID3D11Device> device;
    D3D_FEATURE_LEVEL d3dFeatures[7] = {
        D3D_FEATURE_LEVEL_11_1
    };

    RETURN_IF_FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, createDeviceFlags, d3dFeatures, 1, D3D11_SDK_VERSION,
        device.put(), nullptr, _dxDeviceContext.put()));
    
    // マルチスレッドの設定は無くても大丈夫そうだが念のため
    wil::com_ptr_nothrow<ID3D11Multithread> dxMultiThread;
    _dxDeviceContext->QueryInterface(IID_PPV_ARGS(dxMultiThread.put()));
    dxMultiThread->SetMultithreadProtected(true);

    _dxgiManager->ResetDevice(device.get(), resetToken);

    device->QueryInterface(IID_PPV_ARGS(_dxDevice.put()));

    // 共有テクスチャをそのままMediaFoundationのサンプルに渡すことはできなかったので、
    // バッファとなる別テクスチャに一旦コピー（MiscFlags設定が原因だと思われる。）
    D3D11_TEXTURE2D_DESC bufferTextureDesc;
    bufferTextureDesc.Width = VCAM_VIDEO_WIDTH;
    bufferTextureDesc.Height = VCAM_VIDEO_HEIGHT;
    bufferTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bufferTextureDesc.ArraySize = 1;
    bufferTextureDesc.BindFlags = 0;
    bufferTextureDesc.CPUAccessFlags = 0;
    bufferTextureDesc.MipLevels = 1;
    bufferTextureDesc.MiscFlags = 0;
    bufferTextureDesc.SampleDesc.Count = 1;
    bufferTextureDesc.SampleDesc.Quality = 0;
    bufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    device->CreateTexture2D(&bufferTextureDesc, 0, _bufferTexture.put());

    return S_OK;
}

// キャプチャウィンドウの共有テクスチャをハンドルから取得。
HRESULT FrameGenerator::CreateSharedCaptureWindowTexture()
{
    if (_sharedCaptureWindowTexture != nullptr)
    {
        return S_OK;
    }

    RETURN_IF_FAILED(_dxDevice->OpenSharedResourceByName(
        SHARED_CAPTURE_WINDOW_TEXTURE_PATH, DXGI_SHARED_RESOURCE_READ,
        IID_PPV_ARGS(_sharedCaptureWindowTexture.put())));

    return S_OK;
}

// NV12フォーマットのコンバータ (IMFTransform) 作成
HRESULT FrameGenerator::SetupNV12Converter() 
{
    // create GPU RGB => NV12 converter
    RETURN_IF_FAILED(CoCreateInstance(CLSID_VideoProcessorMFT, nullptr, CLSCTX_ALL, IID_PPV_ARGS(_converter.put())));

    wil::com_ptr_nothrow<IMFAttributes> atts;
    RETURN_IF_FAILED(_converter->GetAttributes(&atts));
    TraceMFAttributes(atts.get(), L"VideoProcessorMFT");

    MFT_OUTPUT_STREAM_INFO info{};
    RETURN_IF_FAILED(_converter->GetOutputStreamInfo(0, &info));
    WINTRACE(L"FrameGenerator::EnsureRenderTarget CLSID_VideoProcessorMFT flags:0x%08X size:%u alignment:%u", info.dwFlags, info.cbSize, info.cbAlignment);

    wil::com_ptr_nothrow<IMFMediaType> inputType;
    RETURN_IF_FAILED(MFCreateMediaType(&inputType));
    inputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    inputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    MFSetAttributeSize(inputType.get(), MF_MT_FRAME_SIZE, _width, _height);
    RETURN_IF_FAILED(_converter->SetInputType(0, inputType.get(), 0));

    wil::com_ptr_nothrow<IMFMediaType> outputType;
    RETURN_IF_FAILED(MFCreateMediaType(&outputType));
    outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    outputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_NV12);
    MFSetAttributeSize(outputType.get(), MF_MT_FRAME_SIZE, _width, _height);
    RETURN_IF_FAILED(_converter->SetOutputType(0, outputType.get(), 0));

    // make sure the video processor works on GPU
    RETURN_IF_FAILED(_converter->ProcessMessage(MFT_MESSAGE_SET_D3D_MANAGER, (ULONG_PTR)_dxgiManager.get()));

    return S_OK;
}

HRESULT FrameGenerator::EnsureRenderTarget(UINT width, UINT height)
{
    if (!_dxgiManager) 
    {
        _width = width;
        _height = height;

        RETURN_IF_FAILED(SetupD3D11Device());

        RETURN_IF_FAILED(CreateSharedCaptureWindowTexture());

        RETURN_IF_FAILED(SetupNV12Converter());
    }

    return S_OK;
}

HRESULT FrameGenerator::Generate(IMFSample* sample, REFGUID format, IMFSample** outSample)
{
    RETURN_HR_IF_NULL(E_POINTER, sample);
    RETURN_HR_IF_NULL(E_POINTER, outSample);
    *outSample = nullptr;

    wil::com_ptr_nothrow<IMFMediaBuffer> mediaBuffer;

    // sampleから全てのバッファを削除（リセット）
    RETURN_IF_FAILED(sample->RemoveAllBuffers());

    // 共有テクスチャをバッファテクスチャにコピー
    wil::com_ptr_nothrow<IDXGIKeyedMutex> mutex;
    _sharedCaptureWindowTexture->QueryInterface(IID_PPV_ARGS(mutex.put()));
    mutex->AcquireSync(MUTEX_KEY, INFINITE);
    _dxDeviceContext->CopyResource(_bufferTexture.get(), _sharedCaptureWindowTexture.get());
    mutex->ReleaseSync(MUTEX_KEY);

    RETURN_IF_FAILED(MFCreateDXGISurfaceBuffer(__uuidof(ID3D11Texture2D), _bufferTexture.get(), 0, 0, &mediaBuffer));
    RETURN_IF_FAILED(sample->AddBuffer(mediaBuffer.get()));

    if (format == MFVideoFormat_NV12)
    {
        // MediaFoundationTransformでB8G8R8A8からNV12に変換
        assert(_converter);
        RETURN_IF_FAILED(_converter->ProcessInput(0, sample, 0));

        MFT_OUTPUT_DATA_BUFFER buffer = {};
        DWORD status = 0;
        RETURN_IF_FAILED(_converter->ProcessOutput(0, 1, &buffer, &status));
        *outSample = buffer.pSample;
    }
    else
    {
        sample->AddRef();
        *outSample = sample;
    }

    return S_OK;
}
