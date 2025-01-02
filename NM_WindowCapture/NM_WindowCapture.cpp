
#include <windows.h>
#include <sddl.h>
#include <d3dcompiler.h>
#include "NM_WindowCapture.h"

#define HLSL_EXTERNAL_INCLUDE(...) #__VA_ARGS__

const char* hlslOffscreenRenderingCode =
#include "SpriteShader.hlsl"
;

/****************************************************************/
/*  MediaFoundation Virtual Camera Function Start               */
/****************************************************************/
void NM_WindowCapture::CreateVirtualCamera()
{
    if (_vcam != nullptr) 
    {
        return;
    }

    HRESULT hr = MFCreateVirtualCamera(MFVirtualCameraType_SoftwareCameraSource,
        MFVirtualCameraLifetime_Session,
        MFVirtualCameraAccess_CurrentUser,
        NAME_NM_WCVCam_MF,
        CLSID_TEXT_NM_WCVCam_MF,
        nullptr, 0, _vcam.put());

    if (hr == S_OK)
    {
        _vcam->Start(nullptr);
    }
}

void NM_WindowCapture::SwitchReverseCamera() 
{ 
    std::lock_guard lock(_captureWindowLock);
    _reverseWindow = !_reverseWindow; 
}

void NM_WindowCapture::SetLeftMargin(int margin) 
{
    std::lock_guard lock(_captureWindowLock);
    if (margin < 0) 
    {
        _leftMargin = 0;
    }
    else if (margin >= _capWinSize.Width - _rightMargin) 
    {
        _leftMargin = _capWinSize.Width - _rightMargin - 1;
    }
    else 
    {
        _leftMargin = margin;
    }
}

void NM_WindowCapture::SetRightMargin(int margin) 
{
    std::lock_guard lock(_captureWindowLock);
    if (margin < 0) 
    {
        _rightMargin = 0;
    }
    else if (margin >= _capWinSize.Width - _leftMargin) 
    {
        _rightMargin = _capWinSize.Width - _leftMargin - 1;
    }
    else 
    {
        _rightMargin = margin;
    }
}

void NM_WindowCapture::SetTopMargin(int margin)
{
    std::lock_guard lock(_captureWindowLock);
    if (margin < 0) 
    {
        _topMargin = 0;
    }
    else if (margin >= _capWinSize.Height - _bottomMargin) 
    {
        _topMargin = _capWinSize.Height - _bottomMargin - 1;
    }
    else 
    {
        _topMargin = margin;
    }
}

void NM_WindowCapture::SetBottomMargin(int margin)
{
    std::lock_guard lock(_captureWindowLock);
    if (margin < 0) 
    {
        _bottomMargin = 0;
    }
    else if (margin >= _capWinSize.Height - _topMargin) 
    {
        _bottomMargin = _capWinSize.Height - _topMargin - 1;
    }
    else 
    {
        _bottomMargin = margin;
    }
}

void NM_WindowCapture::StopVirtualCamera()
{
    if (_vcam == nullptr)
    {
        return;
    }

    _vcam->Stop();
    _vcam->Remove();
    _vcam = nullptr;
}

int NM_WindowCapture::GetLeftMargin()
{ 
    std::lock_guard lock(_captureWindowLock);
    return _leftMargin; 
}

int NM_WindowCapture::GetRightMargin() 
{ 
    std::lock_guard lock(_captureWindowLock);
    return _rightMargin; 
}

int NM_WindowCapture::GetTopMargin() 
{ 
    std::lock_guard lock(_captureWindowLock);
    return _topMargin; 
}

int NM_WindowCapture::GetBottomMargin() 
{ 
    std::lock_guard lock(_captureWindowLock);
    return _bottomMargin; 
}

void NM_WindowCapture::SetEnabledCapturePreview(bool enabled) 
{ 
    std::lock_guard lock(_captureWindowLock);
    _enabledCapturePreview = enabled; 
}

/****************************************************************/
/*  MediaFoundation Virtual Camera Function End                 */
/****************************************************************/

/****************************************************************/
/*  DirectX Function Start                                      */
/****************************************************************/
template<typename T>
auto getDXGIInterfaceFromObject(winrt::Windows::Foundation::IInspectable const& object) 
{
    auto access = object.as<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
    com_ptr<T> result;
    check_hresult(access->GetInterface(guid_of<T>(), result.put_void()));
    return result;
}

void NM_WindowCapture::CreateDirect3DDeviceForCapture() 
{
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    if (_dxDeviceForCapture != nullptr) 
    {
        _dxDeviceForCapture.Close();
    }

    D3D_FEATURE_LEVEL d3dFeatures[1] = 
    {
        D3D_FEATURE_LEVEL_11_1
    };

    D3D_FEATURE_LEVEL resultFeature;
    check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        nullptr, createDeviceFlags, d3dFeatures, 1, D3D11_SDK_VERSION,
        _dxDevice.put(), &resultFeature, _dxDeviceContext.put()));

    com_ptr<IDXGIDevice> dxgiDevice = _dxDevice.as<IDXGIDevice>();
    com_ptr<::IInspectable> device = nullptr;
    check_hresult(::CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), device.put()));
    _dxDeviceForCapture = device.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();
}

void NM_WindowCapture::CreateCapturePreviewTexture() 
{
    if (_dxDevice == nullptr) 
    {
        return;
    }

    //プレビュー用テクスチャ
    D3D11_TEXTURE2D_DESC bufferTextureDesc;

    bufferTextureDesc.Width = VCAM_VIDEO_WIDTH;
    bufferTextureDesc.Height = VCAM_VIDEO_HEIGHT;
    bufferTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bufferTextureDesc.ArraySize = 1;
    bufferTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    bufferTextureDesc.CPUAccessFlags = 0;
    bufferTextureDesc.MipLevels = 1;
    bufferTextureDesc.MiscFlags = 0;
    bufferTextureDesc.SampleDesc.Count = 1;
    bufferTextureDesc.SampleDesc.Quality = 0;
    bufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;

    check_hresult(_dxDevice->CreateTexture2D(&bufferTextureDesc, 0, _capturePreviewTexture.put()));

}

void NM_WindowCapture::CloseCapturePreviewTexture() 
{
    std::lock_guard lock(_captureWindowLock);
    if (_capturePreviewTexture != nullptr) 
    {
        _capturePreviewTexture = nullptr;
    }
}

void NM_WindowCapture::CreateSharedCaptureWindowTexture() 
{
    std::lock_guard lock(_captureWindowLock);
    if (_dxDevice == nullptr || _sharedCaptureWindowTexture != nullptr) 
    {
        return;
    }

    //他プロセスと共有するためのテクスチャ
    D3D11_TEXTURE2D_DESC bufferTextureDesc;

    bufferTextureDesc.Width = VCAM_VIDEO_WIDTH;
    bufferTextureDesc.Height = VCAM_VIDEO_HEIGHT;
    bufferTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bufferTextureDesc.ArraySize = 1;
    bufferTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    bufferTextureDesc.CPUAccessFlags = 0;
    bufferTextureDesc.MipLevels = 1;
    bufferTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_NTHANDLE | D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
    bufferTextureDesc.SampleDesc.Count = 1;
    bufferTextureDesc.SampleDesc.Quality = 0;
    bufferTextureDesc.Usage = D3D11_USAGE_DEFAULT;

    check_hresult(_dxDevice->CreateTexture2D(&bufferTextureDesc, 0, _sharedCaptureWindowTexture.put()));

    com_ptr<IDXGIResource1> sharedCaptureWindowResource;
    _sharedCaptureWindowTexture->QueryInterface(IID_PPV_ARGS(sharedCaptureWindowResource.put()));

    // Session0であるFrameServerで共有テクスチャにアクセスできるようにするため、
    // SECURITY_ATTRIBUTESを設定する必要がある。
    SECURITY_ATTRIBUTES secAttr;
    secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    secAttr.bInheritHandle = FALSE;
    secAttr.lpSecurityDescriptor = NULL;

    // Local Serviceにのみ読み取りのアクセス権（今回のFrameServerのパイプラインならこれで十分）
    // Interactive User（現在のユーザ）に全アクセス権を付与するなら(A;;GA;;;IU)を追加する。
    if (ConvertStringSecurityDescriptorToSecurityDescriptor(
        TEXT("D:(A;;GR;;;LS)(A;;GR;;;IU)"),
        SDDL_REVISION_1, &(secAttr.lpSecurityDescriptor), NULL)) 
    {
        HRESULT hr = sharedCaptureWindowResource->CreateSharedHandle(&secAttr,
            DXGI_SHARED_RESOURCE_READ,
            SHARED_CAPTURE_WINDOW_TEXTURE_PATH,
            &_sharedCaptureWindowHandle);
        com_ptr<IDXGIKeyedMutex> mutex;
        _sharedCaptureWindowTexture.as(mutex);
        mutex->AcquireSync(0, INFINITE);
        mutex->ReleaseSync(MUTEX_KEY);
    }

    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, _dxgiFormat);
    _dxDevice->CreateRenderTargetView(_sharedCaptureWindowTexture.get(),
        &renderTargetViewDesc, _renderTargetViewForSharedCaptureWindow.put());
}

void NM_WindowCapture::CloseSharedCaptureWindowTextureHandle() 
{
    std::lock_guard lock(_captureWindowLock);
    if (_sharedCaptureWindowHandle != NULL
        && _sharedCaptureWindowHandle != INVALID_HANDLE_VALUE) 
    {
        CloseHandle(_sharedCaptureWindowHandle);
        _sharedCaptureWindowHandle = NULL;
    }

    if (_sharedCaptureWindowTexture != nullptr) 
    {
        _sharedCaptureWindowTexture = nullptr;
    }
}

// _capturePreviewTextureや_sharedCaptureWindowTextureへのオフスクリーンレンダリングの準備
void NM_WindowCapture::SetupOffscreenRendering() 
{
    CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, _dxgiFormat);
    _dxDevice->CreateRenderTargetView(_capturePreviewTexture.get(),
        &renderTargetViewDesc, _renderTargetViewForCapturePreview.put());

    D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)VCAM_VIDEO_WIDTH, (float)VCAM_VIDEO_HEIGHT, 0.0f, 1.0f };
    _dxDeviceContext->RSSetViewports(1, &vp);

    size_t hlslSize = std::strlen(hlslOffscreenRenderingCode);
    com_ptr<ID3DBlob> compiledVS;
    D3DCompile(hlslOffscreenRenderingCode, hlslSize, nullptr, nullptr, nullptr,
        "VS", "vs_5_0", 0, 0, compiledVS.put(), nullptr);

    com_ptr<ID3DBlob> compiledPS;
    D3DCompile(hlslOffscreenRenderingCode, hlslSize, nullptr, nullptr, nullptr,
        "PS", "ps_5_0", 0, 0, compiledPS.put(), nullptr);

    _dxDevice->CreateVertexShader(compiledVS->GetBufferPointer(),
        compiledVS->GetBufferSize(), nullptr, _spriteVS.put());
    _dxDeviceContext->VSSetShader(_spriteVS.get(), 0, 0);

    _dxDevice->CreatePixelShader(compiledPS->GetBufferPointer(),
        compiledPS->GetBufferSize(), nullptr, _spritePS.put());
    _dxDeviceContext->PSSetShader(_spritePS.get(), 0, 0);

    D3D11_INPUT_ELEMENT_DESC layout[2] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXUV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    _dxDevice->CreateInputLayout(layout, 2, compiledVS->GetBufferPointer(),
        compiledVS->GetBufferSize(), _spriteInputLayout.put());
    _dxDeviceContext->IASetInputLayout(_spriteInputLayout.get());

    _vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    _vbDesc.ByteWidth = sizeof(VertexType) * 4;
    _vbDesc.MiscFlags = 0;
    _vbDesc.StructureByteStride = 0;
    _vbDesc.Usage = D3D11_USAGE_DEFAULT;
    _vbDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {
        _polygonVertex, sizeof(_polygonVertex), 0
    };

    _dxDevice->CreateBuffer(&_vbDesc, &initData, _vertexBuffer.put());

    UINT stride = sizeof(VertexType);
    UINT offset = 0;
    ID3D11Buffer* tempBufferPtr = _vertexBuffer.get();
    _dxDeviceContext->IASetVertexBuffers(0, 1, &tempBufferPtr, &stride, &offset);
    _dxDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void NM_WindowCapture::DrawCapturePreview() 
{
    if (_capturePreviewTexture == nullptr || _renderTargetViewForCapturePreview == nullptr) 
    {
        return;
    }

    ID3D11RenderTargetView* tempRenderTargetViewPtr = _renderTargetViewForCapturePreview.get();
    _dxDeviceContext->OMSetRenderTargets(1, &tempRenderTargetViewPtr, nullptr);

    float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    _dxDeviceContext->ClearRenderTargetView(_renderTargetViewForCapturePreview.get(), color);

    if (_shaderResourceView == nullptr) 
    {
        _dxDeviceContext->Flush();
        _dxDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
        return;
    }

    // ポリゴン頂点位置の計算
    float xPosRate = 1.0f;
    float yPosRate = 1.0f;
    float rectRate = static_cast<float>(VCAM_VIDEO_WIDTH) / static_cast<float>(VCAM_VIDEO_HEIGHT);
    float floatWindowWidth = static_cast<float>(_capWinSize.Width);
    float floatWindowHeight = static_cast<float>(_capWinSize.Height);

    if (floatWindowWidth > floatWindowHeight * rectRate) 
    {
        yPosRate = floatWindowHeight * rectRate / floatWindowWidth;
    }
    else 
    {
        xPosRate = floatWindowWidth / (floatWindowHeight * rectRate);
    }

    _polygonVertex[0] = { {-xPosRate, yPosRate, 0.0f}, {0.0f, 0.0f} };
    _polygonVertex[1] = { {xPosRate, yPosRate, 0.0f}, {1.0f, 0.0f} };
    _polygonVertex[2] = { {-xPosRate, -yPosRate, 0.0f}, {0.0f, 1.0f} };
    _polygonVertex[3] = { {xPosRate, -yPosRate, 0.0f}, {1.0f, 1.0f} };

    _dxDeviceContext->UpdateSubresource(_vertexBuffer.get(), 0, nullptr, _polygonVertex, 0, 0);

    _dxDeviceContext->Draw(4, 0);
    _dxDeviceContext->Flush();

    _dxDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void NM_WindowCapture::DrawSharedCaptureWindow() 
{
    if (_sharedCaptureWindowTexture == nullptr || _renderTargetViewForSharedCaptureWindow == nullptr) 
    {
        return;
    }

    com_ptr<IDXGIKeyedMutex> mutex;
    _sharedCaptureWindowTexture.as(mutex);
    mutex->AcquireSync(MUTEX_KEY, INFINITE);

    ID3D11RenderTargetView* tempRenderTargetViewPtr = _renderTargetViewForSharedCaptureWindow.get();
    _dxDeviceContext->OMSetRenderTargets(1, &tempRenderTargetViewPtr, nullptr);

    float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    _dxDeviceContext->ClearRenderTargetView(_renderTargetViewForSharedCaptureWindow.get(), color);

    if (_shaderResourceView == nullptr) 
    {
        _dxDeviceContext->Flush();
        _dxDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
        mutex->ReleaseSync(MUTEX_KEY);
        return;
    }

    // ポリゴン頂点位置の計算
    float xPosRate = 1.0f;
    float yPosRate = 1.0f;
    float rectRate = static_cast<float>(VCAM_VIDEO_WIDTH) / static_cast<float>(VCAM_VIDEO_HEIGHT);
    float floatWindowWidth = static_cast<float>(_capWinSize.Width - _leftMargin - _rightMargin);
    float floatWindowHeight = static_cast<float>(_capWinSize.Height - _topMargin - _bottomMargin);

    if (floatWindowWidth > floatWindowHeight * rectRate) 
    {
        yPosRate = floatWindowHeight * rectRate / floatWindowWidth;
    }
    else 
    {
        xPosRate = floatWindowWidth / (floatWindowHeight * rectRate);
    }

    float invFloatWindowWidth = 1.0f / static_cast<float>(_capWinSize.Width);
    float invFloatWindowHeight = 1.0f / static_cast<float>(_capWinSize.Height);
    float xTexMin = static_cast<float>(_leftMargin) * invFloatWindowWidth;
    float xTexMax = 1.0f - static_cast<float>(_rightMargin) * invFloatWindowWidth;
    float yTexMin = static_cast<float>(_topMargin) * invFloatWindowHeight;
    float yTexMax = 1.0f - static_cast<float>(_bottomMargin) * invFloatWindowHeight;

    _polygonVertex[0] = { {-xPosRate, yPosRate, 0.0f}, {xTexMin, yTexMin} };
    _polygonVertex[1] = { {xPosRate, yPosRate, 0.0f}, {xTexMax, yTexMin} };
    _polygonVertex[2] = { {-xPosRate, -yPosRate, 0.0f}, {xTexMin, yTexMax} };
    _polygonVertex[3] = { {xPosRate, -yPosRate, 0.0f}, {xTexMax, yTexMax} };

    // 左右反転
    if (_reverseWindow) 
    {
        _polygonVertex[0].Tex.x = xTexMax;
        _polygonVertex[1].Tex.x = xTexMin;
        _polygonVertex[2].Tex.x = xTexMax;
        _polygonVertex[3].Tex.x = xTexMin;
    }

    _dxDeviceContext->UpdateSubresource(_vertexBuffer.get(), 0, nullptr, _polygonVertex, 0, 0);

    _dxDeviceContext->Draw(4, 0);
    _dxDeviceContext->Flush();

    _dxDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    mutex->ReleaseSync(MUTEX_KEY);
}

void NM_WindowCapture::CopyCapturePreviewToDXGIResource(void* resourcePtr) 
{
    HRESULT hr = S_OK;

    if (resourcePtr == nullptr) 
    {
        return;
    }

    IUnknown* pUnk = static_cast<IUnknown*>(resourcePtr);
    winrt::com_ptr<IDXGIResource> pDxgiResource;
    hr = pUnk->QueryInterface(IID_PPV_ARGS(pDxgiResource.put()));
    if (FAILED(hr)) 
    {
        return;
    }

    HANDLE sharedHandle;
    hr = pDxgiResource->GetSharedHandle(&sharedHandle);
    if (FAILED(hr)) 
    {
        return;
    }
    
    winrt::com_ptr<ID3D11Resource> pD3D11Resource;
    hr = _dxDevice->OpenSharedResource(sharedHandle, IID_PPV_ARGS(pD3D11Resource.put()));
    if (FAILED(hr)) 
    {
        return;
    }

    D3D11_BOX range;
    range.front = 0;
    range.back = 1;
    range.left = 0;
    range.right = VCAM_VIDEO_WIDTH;
    range.top = 0;
    range.bottom = VCAM_VIDEO_HEIGHT;

    std::lock_guard lock(_captureWindowLock);
    _dxDeviceContext->CopySubresourceRegion(pD3D11Resource.get(), 0, 0, 0, 0,
        _capturePreviewTexture.get(), 0, &range);
}

/****************************************************************/
/*  DirectX Function End                                        */
/****************************************************************/

/****************************************************************/
/*  winRT GraphicsCapture Function Start                        */
/****************************************************************/
bool NM_WindowCapture::IsCapturing() { return _framePoolForCapture != nullptr; }

void NM_WindowCapture::StopCapture() 
{
    if (IsCapturing()) 
    {
        _frameArrivedForCapture.revoke();
        _captureSession = nullptr;
        _framePoolForCapture.Close();
        _framePoolForCapture = nullptr;
    }
}

void NM_WindowCapture::ChangeWindow() 
{
    if (_graphicsCaptureItem == nullptr) 
    {
        return;
    }

    if (IsCapturing()) 
    {
        StopCapture();
    }

    std::lock_guard lock(_captureWindowLock);

    _leftMargin = 0;
    _rightMargin = 0;
    _topMargin = 0;
    _bottomMargin = 0;
    _captureFrameCount = 0;

    _capWinSize = _graphicsCaptureItem.Size();
    _framePoolForCapture = Direct3D11CaptureFramePool::CreateFreeThreaded(_dxDeviceForCapture, 
        DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, _capWinSize);
    _frameArrivedForCapture = _framePoolForCapture.FrameArrived(auto_revoke, { this, &NM_WindowCapture::OnFrameArrived });
    _captureSession = _framePoolForCapture.CreateCaptureSession(_graphicsCaptureItem);
    //IsCursorCaptureEnabledでカーソルもキャプチャするか指定できる。
    _captureSession.IsCursorCaptureEnabled(false);
    _captureSession.StartCapture();
}

void NM_WindowCapture::SetTargetWindowForCapture(HWND targetWindow)
{
    namespace abi = ABI::Windows::Graphics::Capture;
    if (targetWindow == NULL) 
    {
        return;
    }

    auto factory = get_activation_factory<GraphicsCaptureItem>();
    auto interop = factory.as<::IGraphicsCaptureItemInterop>();
    check_hresult(interop->CreateForWindow(targetWindow, guid_of<abi::IGraphicsCaptureItem>(),
        reinterpret_cast<void**>(put_abi(_graphicsCaptureItem))));

    ChangeWindow();
}

winrt::Windows::Foundation::IAsyncAction NM_WindowCapture::OpenWindowPicker()
{
    init_apartment(winrt::apartment_type::single_threaded);
    SetWindowDisplayAffinity(_baseHwnd, WDA_EXCLUDEFROMCAPTURE);

    GraphicsCapturePicker picker;
    auto interop = picker.as<::IInitializeWithWindow>();
    interop->Initialize(_baseHwnd);
    GraphicsCaptureItem pickerResult = co_await picker.PickSingleItemAsync();
    if (pickerResult != nullptr) 
    {
        _graphicsCaptureItem = pickerResult;
    }

    SetWindowDisplayAffinity(_baseHwnd, WDA_NONE);
    ChangeWindow();
}

void NM_WindowCapture::OnFrameArrived(Direct3D11CaptureFramePool const& sender,
    winrt::Windows::Foundation::IInspectable const& args)
{
    std::lock_guard lock(_captureWindowLock);

    if (_captureFrameCount == 0) 
    {
        _captureStartTime = std::chrono::system_clock::now();
    }
    auto frame = sender.TryGetNextFrame();

    SizeInt32 itemSize = frame.ContentSize();
    if (itemSize.Width <= 0) 
    {
        itemSize.Width = 1;
    }
    if (itemSize.Height <= 0) 
    {
        itemSize.Height = 1;
    }

    if (itemSize.Width != _capWinSize.Width
        || itemSize.Height != _capWinSize.Height) 
    {
        _capWinSize = itemSize;

        float rate = 0.0f;
        int overMargin = (_leftMargin + _rightMargin) - _capWinSize.Width;
        if (overMargin > 0) 
        {
            rate = static_cast<float>(_leftMargin) / static_cast<float>(_leftMargin + _rightMargin);
            _leftMargin -= static_cast<int>(static_cast<float>(overMargin) * rate + 0.5f);
            _rightMargin -= static_cast<int>(static_cast<float>(overMargin) * (1.0f - rate) + 0.5f);
        }

        overMargin = (_topMargin + _bottomMargin) - _capWinSize.Height;
        if (overMargin > 0) 
        {
            rate = static_cast<float>(_topMargin) / static_cast<float>(_topMargin + _bottomMargin);
            _topMargin -= static_cast<int>(static_cast<float>(overMargin) * rate + 0.5f);
            _bottomMargin -= static_cast<int>(static_cast<float>(overMargin) * (1.0f - rate) + 0.5f);
        }

        _framePoolForCapture.Recreate(_dxDeviceForCapture,
            DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, _capWinSize);
    }

    //キャプチャしたフレームから得られるテクスチャを共有テクスチャにオフスクリーンレンダリング
    com_ptr<ID3D11Texture2D> currentTexture = getDXGIInterfaceFromObject<::ID3D11Texture2D>(frame.Surface());
    CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, _dxgiFormat);
    _dxDevice->CreateShaderResourceView(currentTexture.get(), &shaderResourceViewDesc, _shaderResourceView.put());
    ID3D11ShaderResourceView* tempShaderResourceViewPtr[] = { _shaderResourceView.get() };
    _dxDeviceContext->PSSetShaderResources(0, 1, tempShaderResourceViewPtr);
    
    if (_enabledCapturePreview) 
    {
        DrawCapturePreview();
    }

    DrawSharedCaptureWindow();

    _captureFrameCount++;
    if (_captureFrameCount >= FPS_MAX_FRAME) 
    {
        std::chrono::system_clock::time_point captureEndTime = std::chrono::system_clock::now();
        _captureFPS = static_cast<float>(_captureFrameCount) /
            (static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(captureEndTime - _captureStartTime).count()) * 0.001f);
        _captureFrameCount = 0;
    }
}
/****************************************************************/
/*  winRT GraphicsCapture Function End                          */
/****************************************************************/
