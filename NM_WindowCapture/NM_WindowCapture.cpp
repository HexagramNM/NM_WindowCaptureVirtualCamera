
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
	HRESULT hr = MFCreateVirtualCamera(MFVirtualCameraType_SoftwareCameraSource,
		MFVirtualCameraLifetime_Session,
		MFVirtualCameraAccess_CurrentUser,
		TEXT("NM_WCVCam_MF"),
		TEXT("{cc593221-3915-4084-bf70-dd9d8b481b4e}"),
		nullptr, 0, _vcam.put());

	if (hr == S_OK)
	{
		_vcam->Start(nullptr);
	}
}

void NM_WindowCapture::SwitchReverseCamera()
{
	_reverseWindow = !_reverseWindow;
}

void NM_WindowCapture::StopVirtualCamera()
{
	if (_vcam != nullptr)
	{
		_vcam->Stop();
		_vcam->Remove();
		_vcam = nullptr;
	}
}
/****************************************************************/
/*  MediaFoundation Virtual Camera Function End                 */
/****************************************************************/

/****************************************************************/
/*  DirectX Function Start                                      */
/****************************************************************/
template<typename T>
auto getDXGIInterfaceFromObject(winrt::Windows::Foundation::IInspectable const& object) {
	auto access = object.as<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
	com_ptr<T> result;
	check_hresult(access->GetInterface(guid_of<T>(), result.put_void()));
	return result;
}

void NM_WindowCapture::CreateDirect3DDeviceForCapture() {
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	if (_dxDeviceForCapture != nullptr) {
		_dxDeviceForCapture.Close();
	}

	D3D_FEATURE_LEVEL d3dFeatures[1] = {
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

void NM_WindowCapture::CreateSharedCaptureWindowTexture() {
	if (_dxDevice == nullptr) {
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
		SDDL_REVISION_1, &(secAttr.lpSecurityDescriptor), NULL)) {

		HRESULT hr = sharedCaptureWindowResource->CreateSharedHandle(&secAttr,
			DXGI_SHARED_RESOURCE_READ,
			TEXT("Global\\NM_WCVCam_Captured_Window"),
			&_sharedCaptureWindowHandle);
		com_ptr<IDXGIKeyedMutex> mutex;
		_sharedCaptureWindowTexture.as(mutex);
		mutex->AcquireSync(0, INFINITE);
		mutex->ReleaseSync(MUTEX_KEY);
	}
}

void NM_WindowCapture::CloseSharedCaptureWindowTextureHandle() {
	if (_sharedCaptureWindowHandle != NULL
		&& _sharedCaptureWindowHandle != INVALID_HANDLE_VALUE) {

		CloseHandle(_sharedCaptureWindowHandle);
		_sharedCaptureWindowHandle = NULL;
	}
}

// _captureTextureへのオフスクリーンレンダリングの準備
void NM_WindowCapture::SetupOffscreenRendering() {
	DXGI_FORMAT dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(D3D11_RTV_DIMENSION_TEXTURE2D, dxgiFormat);
	_dxDevice->CreateRenderTargetView(_sharedCaptureWindowTexture.get(),
		&renderTargetViewDesc, _renderTargetView.put());

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

void NM_WindowCapture::DrawCaptureTexture(com_ptr<ID3D11Texture2D> currentTexture) {
	com_ptr<IDXGIKeyedMutex> mutex;
	_sharedCaptureWindowTexture.as(mutex);
	mutex->AcquireSync(MUTEX_KEY, INFINITE);

	ID3D11RenderTargetView* tempRenderTargetViewPtr = _renderTargetView.get();
	_dxDeviceContext->OMSetRenderTargets(1, &tempRenderTargetViewPtr, nullptr);

	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	_dxDeviceContext->ClearRenderTargetView(_renderTargetView.get(), color);

	// ポリゴン頂点位置の計算
	float xPosRate = 1.0f;
	float yPosRate = 1.0f;
	float rectRate = static_cast<float>(VCAM_VIDEO_WIDTH) / static_cast<float>(VCAM_VIDEO_HEIGHT);
	float floatWindowWidth = static_cast<float>(_capWinSize.Width);
	float floatWindowHeight = static_cast<float>(_capWinSize.Height);

	if (floatWindowWidth > floatWindowHeight * rectRate) {
		yPosRate = floatWindowHeight * rectRate / floatWindowWidth;
	}
	else {
		xPosRate = floatWindowWidth / (floatWindowHeight * rectRate);
	}

	_polygonVertex[0] = { {-xPosRate, yPosRate, 0.0f}, {0.0f, 0.0f} };
	_polygonVertex[1] = { {xPosRate, yPosRate, 0.0f}, {1.0f, 0.0f} };
	_polygonVertex[2] = { {-xPosRate, -yPosRate, 0.0f}, {0.0f, 1.0f} };
	_polygonVertex[3] = { {xPosRate, -yPosRate, 0.0f}, {1.0f, 1.0f} };

	// 左右反転
	if (_reverseWindow) {
		_polygonVertex[0].Tex.x = 1.0f;
		_polygonVertex[1].Tex.x = 0.0f;
		_polygonVertex[2].Tex.x = 1.0f;
		_polygonVertex[3].Tex.x = 0.0f;
	}

	_dxDeviceContext->UpdateSubresource(_vertexBuffer.get(), 0, nullptr, _polygonVertex, 0, 0);

	CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc(D3D11_SRV_DIMENSION_TEXTURE2D, DXGI_FORMAT_B8G8R8A8_UNORM);
	_dxDevice->CreateShaderResourceView(currentTexture.get(), &shaderResourceViewDesc, _shaderResourceView.put());
	ID3D11ShaderResourceView* tempShaderResourceViewPtr[] = { _shaderResourceView.get() };
	_dxDeviceContext->PSSetShaderResources(0, 1, tempShaderResourceViewPtr);

	_dxDeviceContext->Draw(4, 0);
	_dxDeviceContext->Flush();

	_dxDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	mutex->ReleaseSync(MUTEX_KEY);
}

/****************************************************************/
/*  DirectX Function End                                        */
/****************************************************************/

/****************************************************************/
/*  winRT GraphicsCapture Function Start                        */
/****************************************************************/
bool NM_WindowCapture::IsCapturing() { return _framePoolForCapture != nullptr; }

void NM_WindowCapture::StopCapture() {
	if (IsCapturing()) {
		_frameArrivedForCapture.revoke();
		_captureSession = nullptr;
		_framePoolForCapture.Close();
		_framePoolForCapture = nullptr;
	}
}

void NM_WindowCapture::ChangeWindow() {
	if (_graphicsCaptureItem == nullptr) {
		return;
	}

	if (IsCapturing()) {
		StopCapture();
	}

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
	if (targetWindow == NULL) {
		return;
	}

	auto factory = get_activation_factory<GraphicsCaptureItem>();
	auto interop = factory.as<::IGraphicsCaptureItemInterop>();
	check_hresult(interop->CreateForWindow(targetWindow, guid_of<abi::IGraphicsCaptureItem>(),
		reinterpret_cast<void**>(put_abi(_graphicsCaptureItem))));

	if (IsCapturing()) {
		ChangeWindow();
	}
}

winrt::Windows::Foundation::IAsyncAction NM_WindowCapture::OpenWindowPicker()
{
	init_apartment(winrt::apartment_type::single_threaded);
	SetWindowDisplayAffinity(_baseHwnd, WDA_EXCLUDEFROMCAPTURE);

	GraphicsCapturePicker picker;
	auto interop = picker.as<::IInitializeWithWindow>();
	interop->Initialize(_baseHwnd);
	GraphicsCaptureItem pickerResult = co_await picker.PickSingleItemAsync();
	if (pickerResult != nullptr) {
		_graphicsCaptureItem = pickerResult;
	}

	if (IsCapturing()) {
		ChangeWindow();
	}
}

void NM_WindowCapture::OnFrameArrived(Direct3D11CaptureFramePool const& sender,
	winrt::Windows::Foundation::IInspectable const& args)
{
	auto frame = sender.TryGetNextFrame();

	SizeInt32 itemSize = frame.ContentSize();
	if (itemSize.Width <= 0) {
		itemSize.Width = 1;
	}
	if (itemSize.Height <= 0) {
		itemSize.Height = 1;
	}

	if (itemSize.Width != _capWinSize.Width
		|| itemSize.Height != _capWinSize.Height) {
		_capWinSize = itemSize;
		_framePoolForCapture.Recreate(_dxDeviceForCapture,
			DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, _capWinSize);
	}

	//キャプチャしたフレームから得られるテクスチャを共有テクスチャにオフスクリーンレンダリング
	if (_sharedCaptureWindowTexture != nullptr) {
		com_ptr<ID3D11Texture2D> currentTexture = getDXGIInterfaceFromObject<::ID3D11Texture2D>(frame.Surface());
		DrawCaptureTexture(currentTexture);
	}
}
/****************************************************************/
/*  winRT GraphicsCapture Function End                          */
/****************************************************************/
