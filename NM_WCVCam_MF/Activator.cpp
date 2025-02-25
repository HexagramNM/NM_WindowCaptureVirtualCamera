
#include "pch.h"
#include "Undocumented.h"
#include "Tools.h"
#include "Activator.h"
#include "../global_config.h"

HRESULT Activator::Initialize()
{
    _source = winrt::make_self<MediaSource>();
    RETURN_IF_FAILED(SetUINT32(MF_VIRTUALCAMERA_PROVIDE_ASSOCIATED_CAMERA_SOURCES, 1));
    RETURN_IF_FAILED(SetGUID(MFT_TRANSFORM_CLSID_Attribute, CLSID_NM_WCVCam_MF));
    RETURN_IF_FAILED(_source->Initialize(this));
    return S_OK;
}

// IMFActivate
STDMETHODIMP Activator::ActivateObject(REFIID riid, void** ppv)
{
    RETURN_HR_IF_NULL(E_POINTER, ppv);
    *ppv = nullptr;

    RETURN_IF_FAILED_MSG(_source->QueryInterface(riid, ppv), "Activator::ActivateObject failed on IID %s", GUID_ToStringW(riid).c_str());
    return S_OK;
}

STDMETHODIMP Activator::ShutdownObject()
{
    return S_OK;
}

STDMETHODIMP Activator::DetachObject()
{
    _source = nullptr;
    return S_OK;
}
