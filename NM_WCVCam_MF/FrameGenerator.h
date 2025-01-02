#pragma once

#include <d3d11_4.h>
#include <dxgi1_2.h>

class FrameGenerator
{
    UINT _width;
    UINT _height;

    wil::com_ptr_nothrow<IMFDXGIDeviceManager> _dxgiManager;
    wil::com_ptr_nothrow<ID3D11Device1> _dxDevice;
    wil::com_ptr_nothrow<ID3D11DeviceContext> _dxDeviceContext;
    wil::com_ptr_nothrow<IMFTransform> _converter;
    wil::com_ptr_nothrow<ID3D11Texture2D> _sharedCaptureWindowTexture;
    wil::com_ptr_nothrow<ID3D11Texture2D> _bufferTexture;
    HANDLE _sharedCaptureWindowHandle;

    HRESULT SetupD3D11Device();

    HRESULT CreateSharedCaptureWindowTexture();

    HRESULT SetupNV12Converter();

public:
    FrameGenerator() :
        _width(0),
        _height(0),
        _sharedCaptureWindowHandle(NULL)
    {
    }

    ~FrameGenerator()
    {
        if (_sharedCaptureWindowHandle != NULL
            && _sharedCaptureWindowHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(_sharedCaptureWindowHandle);
            _sharedCaptureWindowHandle = NULL;
        }
    }

    HRESULT EnsureRenderTarget(UINT width, UINT height);
    HRESULT Generate(IMFSample* sample, REFGUID format, IMFSample** outSample);
};