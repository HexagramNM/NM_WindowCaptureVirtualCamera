#pragma once

#include "NM_WindowCapture.h"

extern "C" __declspec(dllexport) void startupForMF();

extern "C" __declspec(dllexport) NM_WindowCapture *createWindowCaptureObject(HWND baseHwnd);

extern "C" __declspec(dllexport) void copyCapturePreviewToDXGIResource(NM_WindowCapture* captureWindowObj, void* resourcePtr);

extern "C" __declspec(dllexport) void openWindowPicker(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void setTargetWindowForCapture(NM_WindowCapture* windowCaptureObj, HWND hwnd);

extern "C" __declspec(dllexport) void startVirtualCamera(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void switchReverseCaptureWindow(NM_WindowCapture *windowCaptureObj);

extern "C" __declspec(dllexport) void stopVirtualCamera(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void deleteWindowCaptureObject(NM_WindowCapture *windowCaptureObj);

extern "C" __declspec(dllexport) void shutdownForMF();
