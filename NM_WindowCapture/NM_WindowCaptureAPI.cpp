
#include <mfapi.h>
#include "NM_WindowCaptureAPI.h"

void StartupForMF() {
    MFStartup(MF_VERSION);
}

NM_WindowCapture* CreateWindowCaptureObject(HWND baseHwnd)
{
    NM_WindowCapture* result = new NM_WindowCapture(baseHwnd);

    return result;
}

void CopyCapturePreviewToDXGIResource(NM_WindowCapture* windowCaptureObj, void* resourcePtr) {
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->CopyCapturePreviewToDXGIResource(resourcePtr);
    }
}

void OpenWindowPicker(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->OpenWindowPicker();
    }
}

void SetTargetWindowForCapture(NM_WindowCapture* windowCaptureObj, HWND hwnd)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->SetTargetWindowForCapture(hwnd);
    }
}

void StartVirtualCamera(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->CreateSharedCaptureWindowTexture();
        windowCaptureObj->CreateVirtualCamera();
    }
}

void SwitchReverseCaptureWindow(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->SwitchReverseCamera();
    }
}

void SetLeftMargin(NM_WindowCapture* windowCaptureObj, int margin)
{
    if (windowCaptureObj != nullptr) 
    {
        windowCaptureObj->SetLeftMargin(margin);
    }
}

void SetRightMargin(NM_WindowCapture* windowCaptureObj, int margin)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->SetRightMargin(margin);
    }
}

void SetTopMargin(NM_WindowCapture* windowCaptureObj, int margin)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->SetTopMargin(margin);
    }
}

void SetBottomMargin(NM_WindowCapture* windowCaptureObj, int margin)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->SetBottomMargin(margin);
    }
}

int GetLeftMargin(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        return windowCaptureObj->GetLeftMargin();
    }

    return 0;
}

int GetRightMargin(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        return windowCaptureObj->GetRightMargin();
    }

    return 0;
}

int GetTopMargin(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        return windowCaptureObj->GetTopMargin();
    }

    return 0;
}

int GetBottomMargin(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        return windowCaptureObj->GetBottomMargin();
    }

    return 0;
}

void SetEnabledCapturePreview(NM_WindowCapture* windowCaptureObj, bool enabled)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->SetEnabledCapturePreview(enabled);
    }
}

int GetCapturePreviewWidth()
{
    return VCAM_VIDEO_WIDTH;
}

int GetCapturePreviewHeight()
{
    return VCAM_VIDEO_HEIGHT;
}

bool IsCapturing(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        return windowCaptureObj->IsCapturing();
    }

    return false;
}

int GetCaptureWindowWidth(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        return windowCaptureObj->GetCaptureWindowWidth();
    }

    return 0;
}

int GetCaptureWindowHeight(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        return windowCaptureObj->GetCaptureWindowHeight();
    }

    return 0;
}

float GetCaptureFPS(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        return windowCaptureObj->GetCaptureFPS();
    }

    return 0.0f;
}

void SetCaptureCursor(NM_WindowCapture* windowCaptureObj, bool isCaptured)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->SetCaptureCursor(isCaptured);
    }
}

void StopVirtualCamera(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        windowCaptureObj->StopVirtualCamera();
        windowCaptureObj->CloseSharedCaptureWindowTextureHandle();
    }
}

void DeleteWindowCaptureObject(NM_WindowCapture* windowCaptureObj)
{
    if (windowCaptureObj != nullptr)
    {
        delete windowCaptureObj;
    }
}

void ShutdownForMF() {
    MFShutdown();
}
