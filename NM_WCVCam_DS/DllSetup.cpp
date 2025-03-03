﻿
#include "stdafx.h"
#include "NMVCamFilter.h"

#define LOGING_LEVEL 5

extern "C" 
{
    BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);
};

CFactoryTemplate g_Templates[] = {
    { NAME_NM_WCVCam_DS, &CLSID_NM_WCVCam_DS, NMVCamSource::CreateInstance, NULL, &afFilterInfo}
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

REGFILTER2 rf2FilterReg = {
    1,
    MERIT_NORMAL,
    1,
    &sudPins
};

STDAPI DllRegisterServer() 
{
    HRESULT hr;
    IFilterMapper2 *pFM2 = NULL;
    hr = AMovieDllRegisterServer2(TRUE);
    if (FAILED(hr)) 
    {
        return hr;
    }

    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
        IID_IFilterMapper2, (void **)&pFM2);
    if (FAILED(hr)) 
    {
        return hr;
    }

    hr = pFM2->RegisterFilter(
        CLSID_NM_WCVCam_DS,
        NAME_NM_WCVCam_DS,
        NULL,
        &CLSID_VideoInputDeviceCategory,
        NAME_NM_WCVCam_DS,
        &rf2FilterReg
    );

    if (pFM2) 
    {
        pFM2->Release();
    }
    return hr;
}

STDAPI DllUnregisterServer() 
{
    HRESULT hr;
    IFilterMapper2 *pFM2 = NULL;
    hr = AMovieDllRegisterServer2(FALSE);
    if (FAILED(hr)) 
    {
        return hr;
    }

    hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER,
        IID_IFilterMapper2, (void **)&pFM2);
    if (FAILED(hr)) 
    {
        return hr;
    }

    hr = pFM2->UnregisterFilter(
        &CLSID_VideoInputDeviceCategory,
        NAME_NM_WCVCam_DS,
        CLSID_NM_WCVCam_DS
    );

    if (pFM2) 
    {
        pFM2->Release();
    }
    return hr;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved) 
{
    DbgSetModuleLevel(LOG_TRACE,LOGING_LEVEL);
    return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
