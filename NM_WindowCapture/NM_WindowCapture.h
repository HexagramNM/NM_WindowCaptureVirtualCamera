﻿#pragma once

#include <shobjidl_core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3d11.h>
#include <d3d11_4.h>
#include <dxgi1_2.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <Windows.Graphics.Capture.Interop.h>
#include <mfvirtualcamera.h>
#include <mutex>

#include "../global_config.h"

using namespace winrt;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;
using namespace winrt::Windows::Graphics::Capture;

#pragma comment(lib, "windowsapp")
#pragma comment(lib, "advapi32.lib")

class NM_WindowCapture
{
public:
	NM_WindowCapture(HWND baseHwnd):
		_baseHwnd(baseHwnd),
		_vbDesc{},
		_polygonVertex{},
		_graphicsCaptureItem(nullptr),
		_framePoolForCapture(nullptr),
		_captureSession(nullptr),
		_sharedCaptureWindowHandle(nullptr),
		_previewHandle(nullptr),
		_reverseWindow(false)
	{
		_capWinSize.Width = 1;
		_capWinSize.Height = 1;
		CreateDirect3DDeviceForCapture();
		CreatePreviewTexture();
		SetupOffscreenRendering();
	}

	~NM_WindowCapture()
	{
		StopVirtualCamera();
		StopCapture();
		_graphicsCaptureItem = nullptr;
		CloseSharedCaptureWindowTextureHandle();
		ClosePreviewTexture();
	}

	void CreateVirtualCamera();
	void StopVirtualCamera();
	void SwitchReverseCamera();

	void CreateDirect3DDeviceForCapture();
	void CreatePreviewTexture();
	void ClosePreviewTexture();
	void CreateSharedCaptureWindowTexture();
	void CloseSharedCaptureWindowTextureHandle();
	void SetupOffscreenRendering();
	void DrawPreview();
	void DrawSharedCaptureWindow();

	bool IsCapturing();
	void StopCapture();
	void ChangeWindow();
	winrt::Windows::Foundation::IAsyncAction OpenWindowPicker();
	void SetTargetWindowForCapture(HWND targetWindow);
	void OnFrameArrived(Direct3D11CaptureFramePool const& sender,
		winrt::Windows::Foundation::IInspectable const& args);

private:
	static constexpr DXGI_FORMAT _dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

	struct VertexType
	{
		::DirectX::XMFLOAT3 Pos;
		::DirectX::XMFLOAT2 Tex;
	};

	HWND _baseHwnd;
	com_ptr<IMFVirtualCamera> _vcam;

	com_ptr<ID3D11Device> _dxDevice;
	com_ptr<ID3D11DeviceContext> _dxDeviceContext;
	com_ptr<ID3D11Texture2D> _sharedCaptureWindowTexture;
	HANDLE _sharedCaptureWindowHandle;

	com_ptr<ID3D11Texture2D> _previewTexture;
	HANDLE _previewHandle;

	com_ptr<ID3D11RenderTargetView> _renderTargetViewForSharedCaptureWindow;
	com_ptr<ID3D11RenderTargetView> _renderTargetViewForPreview;
	com_ptr<ID3D11ShaderResourceView> _shaderResourceView;
	com_ptr<ID3D11VertexShader> _spriteVS;
	com_ptr<ID3D11PixelShader> _spritePS;
	com_ptr<ID3D11InputLayout> _spriteInputLayout;

	D3D11_BUFFER_DESC _vbDesc;
	VertexType _polygonVertex[4];
	com_ptr<ID3D11Buffer> _vertexBuffer;

	SizeInt32 _capWinSize;
	bool _reverseWindow;

	IDirect3DDevice _dxDeviceForCapture;
	GraphicsCaptureItem _graphicsCaptureItem;
	Direct3D11CaptureFramePool _framePoolForCapture;
	event_revoker<IDirect3D11CaptureFramePool> _frameArrivedForCapture;
	GraphicsCaptureSession _captureSession;

	std::mutex _sharedCaptureWindowLock;
};
