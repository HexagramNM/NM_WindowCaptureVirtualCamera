
#include <mfapi.h>
#include "NM_WindowCaptureAPI.h"

void startupForMF() {
	MFStartup(MF_VERSION);
}

NM_WindowCapture* createWindowCaptureObject(HWND baseHwnd)
{
	NM_WindowCapture* result = new NM_WindowCapture(baseHwnd);

	return result;
}

void copyCapturePreviewToDXGIResource(NM_WindowCapture* windowCaptureObj, void* resourcePtr) {
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->CopyCapturePreviewToDXGIResource(resourcePtr);
	}
}

void openWindowPicker(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->OpenWindowPicker();
	}
}

void setTargetWindowForCapture(NM_WindowCapture* windowCaptureObj, HWND hwnd)
{
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->SetTargetWindowForCapture(hwnd);
	}
}

void startVirtualCamera(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->CreateSharedCaptureWindowTexture();
		windowCaptureObj->CreateVirtualCamera();
	}
}

void switchReverseCaptureWindow(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->SwitchReverseCamera();
	}
}

void setLeftMargin(NM_WindowCapture* windowCaptureObj, int margin)
{
	if (windowCaptureObj != nullptr) 
	{
		windowCaptureObj->SetLeftMargin(margin);
	}
}

void setRightMargin(NM_WindowCapture* windowCaptureObj, int margin)
{
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->SetRightMargin(margin);
	}
}

void setTopMargin(NM_WindowCapture* windowCaptureObj, int margin)
{
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->SetTopMargin(margin);
	}
}

void setBottomMargin(NM_WindowCapture* windowCaptureObj, int margin)
{
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->SetBottomMargin(margin);
	}
}

int getLeftMargin(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		return windowCaptureObj->GetLeftMargin();
	}

	return 0;
}

int getRightMargin(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		return windowCaptureObj->GetRightMargin();
	}

	return 0;
}

int getTopMargin(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		return windowCaptureObj->GetTopMargin();
	}

	return 0;
}

int getBottomMargin(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		return windowCaptureObj->GetBottomMargin();
	}

	return 0;
}

void setEnabledCapturePreview(NM_WindowCapture* windowCaptureObj, bool enabled)
{
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->SetEnabledCapturePreview(enabled);
	}
}

int getCapturePreviewWidth()
{
	return VCAM_VIDEO_WIDTH;
}

int getCapturePreviewHeight()
{
	return VCAM_VIDEO_HEIGHT;
}

bool isCapturing(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		return windowCaptureObj->IsCapturing();
	}

	return false;
}

int getCaptureWindowWidth(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		return windowCaptureObj->GetCaptureWindowWidth();
	}

	return 0;
}

int getCaptureWindowHeight(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		return windowCaptureObj->GetCaptureWindowHeight();
	}

	return 0;
}

float getCaptureFPS(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		return windowCaptureObj->GetCaptureFPS();
	}

	return 0.0f;
}

void stopVirtualCamera(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		windowCaptureObj->StopVirtualCamera();
		windowCaptureObj->CloseSharedCaptureWindowTextureHandle();
	}
}

void deleteWindowCaptureObject(NM_WindowCapture* windowCaptureObj)
{
	if (windowCaptureObj != nullptr)
	{
		delete windowCaptureObj;
	}
}

void shutdownForMF() {
	MFShutdown();
}
