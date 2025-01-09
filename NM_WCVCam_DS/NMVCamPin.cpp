
#include "stdafx.h"
#include "NMVCamFilter.h"
#include <d3dcompiler.h>
#include <thread>

#define HLSL_EXTERNAL_INCLUDE(...) #__VA_ARGS__

// Embeded hlsl shader source code.
const char* hlslFormatterCode =
#include "SampleFormatter.hlsl"
;

NMVCamPin::NMVCamPin(HRESULT *phr, NMVCamSource *pFilter) : CSourceStream(NAME("NMVCamPin"), phr, pFilter, OUTPUT_PIN_NAME)
    , _pFilter(pFilter)
    , _rtFrameLength(666666)
{
    GetMediaType(&m_mt);

    CreateDirectXDeviceForTexture();
    SetupSampleFormatter();
    SetupPlaceholder();
    DrawPlaceholder();
}

NMVCamPin::~NMVCamPin() 
{
}

void NMVCamPin::CreateDirectXDeviceForTexture()
{
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    com_ptr<ID3D11Device> device(nullptr);
    D3D_FEATURE_LEVEL d3dFeatures[7] = {
        D3D_FEATURE_LEVEL_11_1
    };

    check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, createDeviceFlags, d3dFeatures, 1, D3D11_SDK_VERSION,
        device.put(), nullptr, _dxDeviceContext.put()));
    device->QueryInterface(IID_PPV_ARGS(_dxDevice.put()));
}

// ハンドルから共有されたテクスチャを取得
void NMVCamPin::GetSharedTextureFromHandle()
{
    winrt::com_ptr<ID3D11Texture2D> tempCaptureWindowTexture = nullptr;
    _dxDevice->OpenSharedResourceByName(SHARED_CAPTURE_WINDOW_TEXTURE_PATH,
        DXGI_SHARED_RESOURCE_READ, IID_PPV_ARGS(tempCaptureWindowTexture.put()));

    if (_sharedCaptureWindowTexture == nullptr) 
    {
        if (tempCaptureWindowTexture != nullptr) 
        {
            _sharedCaptureWindowTexture = tempCaptureWindowTexture;
            CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_B8G8R8A8_UNORM);
            _dxDevice->CreateShaderResourceView(_sharedCaptureWindowTexture.get(),
                &shaderResourceViewDesc, _formatterSRV.put());
        }
        else if (_formatterSRV == nullptr)
        {
            CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_B8G8R8A8_UNORM);
            _dxDevice->CreateShaderResourceView(_placeholderTexture.get(),
                &shaderResourceViewDesc, _formatterSRV.put());
        }
    }
    else 
    {
        if (tempCaptureWindowTexture == nullptr) 
        {
            _sharedCaptureWindowTexture = nullptr;
            CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_B8G8R8A8_UNORM);
            _dxDevice->CreateShaderResourceView(_placeholderTexture.get(),
                &shaderResourceViewDesc, _formatterSRV.put());
        }
    }
}

// SampleFormatter実行の準備
void NMVCamPin::SetupSampleFormatter()
{
    UINT bufferByteSize = VCAM_VIDEO_WIDTH * VCAM_VIDEO_HEIGHT * PIXEL_BYTE;

    size_t hlslSize = std::strlen(hlslFormatterCode);
    std::string csThreadsStr = std::to_string(CS_THREADS_NUM);
    std::string windowWidthStr = std::to_string(VCAM_VIDEO_WIDTH);
    std::string windowHeightStr = std::to_string(VCAM_VIDEO_HEIGHT);
    com_ptr<ID3DBlob> compiledCS;
    D3D_SHADER_MACRO csMacro[] = {
        "CS_THREADS_NUM_IN_CS", csThreadsStr.c_str(),
        "VCAM_VIDEO_WIDTH_IN_CS", windowWidthStr.c_str(),
        "VCAM_VIDEO_HEIGHT_IN_CS", windowHeightStr.c_str(),
        NULL, NULL
    };
    D3DCompile(hlslFormatterCode, hlslSize, nullptr, csMacro, nullptr,
        "formatterMain", "cs_5_0", 0, 0, compiledCS.put(), nullptr);

    _dxDevice->CreateComputeShader(compiledCS->GetBufferPointer(),
        compiledCS->GetBufferSize(), nullptr, _formatterCS.put());
    _dxDeviceContext->CSSetShader(_formatterCS.get(), 0, 0);

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = bufferByteSize;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    _dxDevice->CreateBuffer(&bufferDesc, nullptr, _gpuFormatterBuffer.put());

    bufferDesc.Usage = D3D11_USAGE_STAGING;
    bufferDesc.BindFlags = 0;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    bufferDesc.MiscFlags = 0;
    _dxDevice->CreateBuffer(&bufferDesc, nullptr, _cpuSampleBuffer.put());

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements = bufferByteSize / 4;
    uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
    _dxDevice->CreateUnorderedAccessView(_gpuFormatterBuffer.get(), &uavDesc, _formatterUAV.put());

    ID3D11UnorderedAccessView* uavs[] = { _formatterUAV.get() };
    UINT initialCounts[] = { 0 };
    _dxDeviceContext->CSSetUnorderedAccessViews(0, 1, uavs, initialCounts);
}

// "No Signal"表示の準備
void NMVCamPin::SetupPlaceholder() 
{
    D3D11_TEXTURE2D_DESC bufferTextureDesc;
    bufferTextureDesc.Width = VCAM_VIDEO_WIDTH;
    bufferTextureDesc.Height = VCAM_VIDEO_HEIGHT;
    bufferTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bufferTextureDesc.ArraySize = 1;
    bufferTextureDesc.MipLevels = 1;
    bufferTextureDesc.SampleDesc.Count = 1;
    bufferTextureDesc.SampleDesc.Quality = 0;
    bufferTextureDesc.MiscFlags = 0;
    bufferTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    bufferTextureDesc.CPUAccessFlags = 0;
    bufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    check_hresult(_dxDevice->CreateTexture2D(&bufferTextureDesc, 0, _placeholderTexture.put()));

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, _placeholderD2DFactory.put());
    D2D1_RENDER_TARGET_PROPERTIES d2d1props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));

    com_ptr<IDXGISurface>renderTargetSurface;
    _placeholderTexture.as(IID_PPV_ARGS(renderTargetSurface.put()));

    HRESULT hr = _placeholderD2DFactory->CreateDxgiSurfaceRenderTarget(
        renderTargetSurface.get(), &d2d1props, _placeholderRenderTarget.put());

    _placeholderRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);

    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), 
        reinterpret_cast<IUnknown**>(_placeholderDWFactory.put()));

    _placeholderDWFactory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, 
        DWRITE_FONT_STRETCH_NORMAL, 100, L"", _placeholderTextFormat.put());

    _placeholderTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    _placeholderTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    _placeholderRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), _placeholderBrush.put());
}

// DirectWriteで"No Signal"の表示
void NMVCamPin::DrawPlaceholder() 
{
    _placeholderRenderTarget->BeginDraw();

    _placeholderRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

    std::wstring placeholderText(L"No Signal");
    D2D1_RECT_F rect;
    rect.left = 0;
    rect.right = VCAM_VIDEO_WIDTH - 1;
    rect.top = 0;
    rect.bottom = VCAM_VIDEO_HEIGHT - 1;
    _placeholderRenderTarget->DrawText(placeholderText.c_str(), placeholderText.size(),
        _placeholderTextFormat.get(), rect, _placeholderBrush.get());

    _placeholderRenderTarget->EndDraw();
}

// コンピュートシェーダでサンプルのフォーマットにあったバッファを作成
void NMVCamPin::GetSampleOnCaptureWindow(LPBYTE sampleData)
{
    winrt::com_ptr<IDXGIKeyedMutex> mutex;
    if (_sharedCaptureWindowTexture != nullptr) 
    {
        _sharedCaptureWindowTexture->QueryInterface(IID_PPV_ARGS(mutex.put()));
    }

    if (mutex) 
    {
        mutex->AcquireSync(MUTEX_KEY, INFINITE);
    }

    ID3D11ShaderResourceView* tempShaderResourceViewPtr[] = { _formatterSRV.get() };
    _dxDeviceContext->CSSetShaderResources(0, 1, tempShaderResourceViewPtr);
    _dxDeviceContext->Dispatch(VCAM_VIDEO_WIDTH / (CS_THREADS_NUM * 4), VCAM_VIDEO_HEIGHT / CS_THREADS_NUM, 1);

    ID3D11ShaderResourceView* tempShaderResourceViewNullPtr[] = { nullptr };
    _dxDeviceContext->CSSetShaderResources(0, 1, tempShaderResourceViewNullPtr);
    if (mutex) 
    {
        mutex->ReleaseSync(MUTEX_KEY);
    }

    _dxDeviceContext->CopyResource(_cpuSampleBuffer.get(), _gpuFormatterBuffer.get());

    com_ptr<IDXGISurface> dxgiSurface;
    _cpuSampleBuffer->QueryInterface(IID_PPV_ARGS(dxgiSurface.put()));

    DXGI_MAPPED_RECT mapFromCpuSampleBuffer;
    dxgiSurface->Map(&mapFromCpuSampleBuffer, DXGI_MAP_READ);

    CopyMemory((PVOID)sampleData, (PVOID)mapFromCpuSampleBuffer.pBits, 
        VCAM_VIDEO_WIDTH * VCAM_VIDEO_HEIGHT * PIXEL_BYTE);

    dxgiSurface->Unmap();
}

STDMETHODIMP NMVCamPin::Notify(IBaseFilter *pSelf, Quality q) 
{
    return E_NOTIMPL;
}

HRESULT NMVCamPin::GetMediaType(CMediaType *pMediaType) 
{
    HRESULT hr=NOERROR;
    VIDEOINFO *pvi=(VIDEOINFO *)pMediaType->AllocFormatBuffer(sizeof(VIDEOINFO));
    ZeroMemory(pvi, sizeof(VIDEOINFO));

    pvi->AvgTimePerFrame=_rtFrameLength;

    BITMAPINFOHEADER *pBmi=&(pvi->bmiHeader);
    pBmi->biSize=sizeof(BITMAPINFOHEADER);
    pBmi->biWidth = VCAM_VIDEO_WIDTH;
    pBmi->biHeight = VCAM_VIDEO_HEIGHT;
    pBmi->biPlanes=1;
    pBmi->biBitCount=PIXEL_BIT;
    pBmi->biCompression=BI_RGB;
    pvi->bmiHeader.biSizeImage=DIBSIZE(pvi->bmiHeader);

    SetRectEmpty(&(pvi->rcSource));
    SetRectEmpty(&(pvi->rcTarget));

    pMediaType->SetType(&MEDIATYPE_Video);
    pMediaType->SetFormatType(&FORMAT_VideoInfo);

    const GUID subtype=GetBitmapSubtype(&pvi->bmiHeader);
    pMediaType->SetSubtype(&subtype);

    pMediaType->SetTemporalCompression(FALSE);
    const int bmpsize=DIBSIZE(*pBmi);
    pMediaType->SetSampleSize(bmpsize);
    
    return hr;
}

HRESULT NMVCamPin::CheckMediaType(const CMediaType *pMediaType) 
{
    HRESULT hr=NOERROR;
    CheckPointer(pMediaType,E_POINTER);
    CMediaType mt;
    GetMediaType(&mt);
    if(mt==*pMediaType) 
    {
        FreeMediaType(mt);
        return S_OK;
    }

    FreeMediaType(mt);
    return E_FAIL;
}

HRESULT NMVCamPin::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pRequest) 
{
    HRESULT hr=NOERROR;
    VIDEOINFO *pvi=reinterpret_cast<VIDEOINFO*>(m_mt.Format());
    ASSERT(pvi != NULL);
    pRequest->cBuffers=1;
    // バッファサイズはビットマップ1枚分以上である必要がある
    if(pvi->bmiHeader.biSizeImage > (DWORD)pRequest->cbBuffer) 
    {
        pRequest->cbBuffer=pvi->bmiHeader.biSizeImage;
    }

    // アロケータプロパティを設定しなおす
    ALLOCATOR_PROPERTIES Actual;
    hr=pAlloc->SetProperties(pRequest, &Actual);
    if(FAILED(hr)) 
    {
        return hr;
    }

    if(Actual.cbBuffer < pRequest->cbBuffer) 
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT NMVCamPin::FillBuffer(IMediaSample *pSample) 
{
    HRESULT hr=E_FAIL;
    CheckPointer(pSample,E_POINTER);
    // ダウンストリームフィルタが
    // フォーマットを動的に変えていないかチェック
    ASSERT(m_mt.formattype == FORMAT_VideoInfo);
    ASSERT(m_mt.cbFormat >= sizeof(VIDEOINFOHEADER));
    // フレームに書き込み
    LPBYTE pSampleData=NULL;
    const long size=pSample->GetSize();
    pSample->GetPointer(&pSampleData);

    CRefTime ref;
    m_pFilter->StreamTime(ref);

    GetSharedTextureFromHandle();
    GetSampleOnCaptureWindow(pSampleData);

    const REFERENCE_TIME delta=_rtFrameLength;
    REFERENCE_TIME start_time=ref;
    FILTER_STATE state;
    m_pFilter->GetState(0, &state);
    if(state==State_Paused)
        start_time=0;
    REFERENCE_TIME end_time=(start_time+delta);
    pSample->SetTime(&start_time, &end_time);
    pSample->SetActualDataLength(size);
    pSample->SetSyncPoint(TRUE);

    //CPU使用率を抑えて、ZoomなどのUIの反応をしやすくするために適度にSleepする。
    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    return S_OK;
}


