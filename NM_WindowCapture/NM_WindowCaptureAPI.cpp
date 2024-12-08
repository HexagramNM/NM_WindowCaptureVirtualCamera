
#include <mfapi.h>
#include "NM_WindowCaptureAPI.h"

void startupForMF() {
	MFStartup(MF_VERSION);
}

NM_WindowCapture* createWindowCaptureObject(HWND hwnd)
{
	NM_WindowCapture* result = new NM_WindowCapture();

	result->CreateDirect3DDeviceForCapture();
	result->CreateSharedCaptureWindowTexture();
	result->SetupOffscreenRendering();
	result->SetTargetWindowForCapture(hwnd);
	result->CreateVirtualCamera();

	return result;
}

void switchReverseCaptureWindow(NM_WindowCapture* captureWindowObj)
{
	if (captureWindowObj != nullptr)
	{
		captureWindowObj->SwitchReverseCamera();
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
