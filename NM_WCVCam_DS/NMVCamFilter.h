#pragma once

#include "stdafx.h"

// ピンタイプの定義
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Video,
    &MEDIASUBTYPE_RGB24
};
// 入力用、出力用ピンの情報
const AMOVIESETUP_PIN sudPins =
{
    OUTPUT_PIN_NAME,
    FALSE,
    TRUE,
    FALSE,
    FALSE,
    &CLSID_NULL,
    NULL,
    1,
    &sudPinTypes
};

// フィルタ情報
const AMOVIESETUP_FILTER afFilterInfo =
{
    &CLSID_NM_WCVCam_DS,
    NAME_NM_WCVCam_DS,
    MERIT_NORMAL,
    1,
    &sudPins
};

class NMVCamPin;

//ソースフィルタクラス
class NMVCamSource : public CSource
{
private:
    NMVCamPin *m_pin;
public:

    NMVCamSource(LPUNKNOWN pUnk, HRESULT *phr);
    virtual ~NMVCamSource();
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
    
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFilterGraph *GetGraph() { return m_pGraph; }
};

// プッシュピンクラス
class NMVCamPin : public CSourceStream, public IAMStreamConfig , public IKsPropertySet, public IAMFilterMiscFlags
{
public:
    NMVCamPin(HRESULT *phr, NMVCamSource *pFilter);
    virtual			~NMVCamPin();
    STDMETHODIMP	Notify(IBaseFilter *pSelf, Quality q) override;

    // CSourceStreamの実装
    HRESULT			GetMediaType(CMediaType *pMediaType) override;
    HRESULT			CheckMediaType(const CMediaType *pMediaType) override;
    HRESULT			DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest) override;
    HRESULT			FillBuffer(IMediaSample *pSample) override;

    //IUnknown
    STDMETHODIMP QueryInterface(REFIID   riid, LPVOID * ppvObj) override;

    STDMETHODIMP_(ULONG) AddRef() override;

    STDMETHODIMP_(ULONG) Release() override;

    //IKsPropertySet
    HRESULT STDMETHODCALLTYPE Get(
        REFGUID PropSet,
        ULONG   Id,
        LPVOID  InstanceData,
        ULONG   InstanceLength,
        LPVOID  PropertyData,
        ULONG   DataLength,
        ULONG   *BytesReturned
    ) override;

    HRESULT STDMETHODCALLTYPE Set(
        REFGUID PropSet,
        ULONG   Id,
        LPVOID  InstanceData,
        ULONG   InstanceLength,
        LPVOID  PropertyData,
        ULONG   DataLength
    ) override;

    HRESULT STDMETHODCALLTYPE QuerySupported(
        REFGUID PropSet,
        ULONG   Id,
        ULONG   *TypeSupport
    ) override;

    //IAMStreamConfig
    HRESULT STDMETHODCALLTYPE GetFormat(
        AM_MEDIA_TYPE **ppmt
    ) override;

    HRESULT STDMETHODCALLTYPE GetNumberOfCapabilities(
        int *piCount,
        int *piSize
    ) override;

    HRESULT STDMETHODCALLTYPE GetStreamCaps(
        int           iIndex,
        AM_MEDIA_TYPE **ppmt,
        BYTE          *pSCC
    ) override;

    HRESULT STDMETHODCALLTYPE SetFormat(
        AM_MEDIA_TYPE *pmt
    ) override;

    //IAMFilterMiscFlags
    ULONG STDMETHODCALLTYPE GetMiscFlags() override;

protected:
private:
    VIDEOINFOHEADER videoInfo 
    {
        RECT{0, 0, VCAM_VIDEO_WIDTH, VCAM_VIDEO_HEIGHT},
        RECT{0, 0, VCAM_VIDEO_WIDTH, VCAM_VIDEO_HEIGHT},
        30 * VCAM_VIDEO_WIDTH * VCAM_VIDEO_HEIGHT * PIXEL_BIT,
        0,
        160000,
        BITMAPINFOHEADER{
            sizeof(BITMAPINFOHEADER),
            VCAM_VIDEO_WIDTH,
            VCAM_VIDEO_HEIGHT,
            1,
            PIXEL_BIT,
            BI_RGB,
            0,
            2500,
            2500,
            0,
            0
        }
    };

    // 共有テクスチャのために追加した部分
    com_ptr<ID3D11Device1> _dxDevice;
    com_ptr<ID3D11DeviceContext> _dxDeviceContext;
    com_ptr<ID3D11Texture2D> _sharedCaptureWindowTexture;

    com_ptr<ID3D11ShaderResourceView> _formatterSRV;
    com_ptr<ID3D11UnorderedAccessView> _formatterUAV;
    com_ptr<ID3D11ComputeShader> _formatterCS;
    com_ptr<ID3D11Buffer> _gpuFormatterBuffer;
    com_ptr<ID3D11Buffer> _cpuSampleBuffer;

    com_ptr<ID3D11Texture2D> _placeholderTexture;
    BYTE _placeholderBitmapData[VCAM_VIDEO_WIDTH * VCAM_VIDEO_HEIGHT * PIXEL_BYTE];

    void FindDeviceAndGetSharedTexture(ID3D11Texture2D** sharedTexture);

    void GetSharedTextureFromHandle();

    void SetupSampleFormatter();

    void SetupPlaceholderTexture();

    void GetSampleOnCaptureWindow(LPBYTE sampleData);

    // 共有テクスチャのために追加した部分 END

    NMVCamSource*		_pFilter;			//このピンが所属しているフィルタへのポインタ

    REFERENCE_TIME	_rtFrameLength;	//1フレームあたりの時間
};
