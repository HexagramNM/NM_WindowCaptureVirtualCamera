
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

void copyCapturePreviewToDXGIResource(NM_WindowCapture* captureWindowObj, void* resourcePtr) {
	if (captureWindowObj != nullptr)
	{
		captureWindowObj->CopyCapturePreviewToDXGIResource(resourcePtr);
	}
}

void openWindowPicker(NM_WindowCapture* captureWindowObj)
{
	if (captureWindowObj != nullptr)
	{
		captureWindowObj->OpenWindowPicker();
	}
}

void setTargetWindowForCapture(NM_WindowCapture* captureWindowObj, HWND hwnd)
{
	if (captureWindowObj != nullptr)
	{
		captureWindowObj->SetTargetWindowForCapture(hwnd);
	}
}

void startVirtualCamera(NM_WindowCapture* captureWindowObj) 
{
	if (captureWindowObj != nullptr)
	{
		captureWindowObj->CreateSharedCaptureWindowTexture();
		captureWindowObj->CreateVirtualCamera();
	}
}

void switchReverseCaptureWindow(NM_WindowCapture* captureWindowObj)
{
	if (captureWindowObj != nullptr)
	{
		captureWindowObj->SwitchReverseCamera();
	}
}

void stopVirtualCamera(NM_WindowCapture* captureWindowObj)
{
	if (captureWindowObj != nullptr)
	{
		captureWindowObj->StopVirtualCamera();
		captureWindowObj->CloseSharedCaptureWindowTextureHandle();
	}
}

void deleteWindowCaptureObject(NM_WindowCapture* captureWindowObj)
{
	if (captureWindowObj != nullptr)
	{
		delete captureWindowObj;
	}
}

void shutdownForMF() {
	MFShutdown();
}
