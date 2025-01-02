
#include "stdafx.h"
#include "NMVCamFilter.h"

NMVCamSource::NMVCamSource(IUnknown *pUnk, HRESULT *phr) :
    CSource(NAME_NM_WCVCam_DS, pUnk, CLSID_NM_WCVCam_DS),
    m_pin(NULL)
{
    m_pin = new NMVCamPin(phr, this);

    if(m_pin == NULL) 
    {
        *phr=E_OUTOFMEMORY;
    }
}

NMVCamSource::~NMVCamSource() 
{
    delete m_pin;
}

CUnknown * WINAPI NMVCamSource::CreateInstance(IUnknown *pUnk, HRESULT *phr) 
{
    //カメラに設定されるときに一度呼ばれている。
    NMVCamSource *pNewFilter = new NMVCamSource(pUnk, phr);
    if (pNewFilter == NULL) 
    {
        *phr = E_OUTOFMEMORY;
    }

    return pNewFilter;
}

STDMETHODIMP NMVCamSource::QueryInterface(REFIID riid, void **ppv) 
{
    if (riid == _uuidof(IAMStreamConfig) || riid == _uuidof(IKsPropertySet)) 
    {
        return m_paStreams[0]->QueryInterface(riid, ppv);
    }
    else 
    {
        return CSource::QueryInterface(riid, ppv);
    }
}


