#pragma once

#include "NM_WindowCapture.h"

extern "C" __declspec(dllexport) void startupForMF();

extern "C" __declspec(dllexport) NM_WindowCapture *createWindowCaptureObject(HWND baseHwnd);

extern "C" __declspec(dllexport) void copyCapturePreviewToDXGIResource(NM_WindowCapture* windowCaptureObj, void* resourcePtr);

extern "C" __declspec(dllexport) void openWindowPicker(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void setTargetWindowForCapture(NM_WindowCapture* windowCaptureObj, HWND hwnd);

extern "C" __declspec(dllexport) void startVirtualCamera(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void switchReverseCaptureWindow(NM_WindowCapture *windowCaptureObj);

extern "C" __declspec(dllexport) void setLeftMargin(NM_WindowCapture* windowCaptureObj, int margin);

extern "C" __declspec(dllexport) void setRightMargin(NM_WindowCapture* windowCaptureObj, int margin);

extern "C" __declspec(dllexport) void setTopMargin(NM_WindowCapture* windowCaptureObj, int margin);

extern "C" __declspec(dllexport) void setBottomMargin(NM_WindowCapture* windowCaptureObj, int margin);

extern "C" __declspec(dllexport) int getLeftMargin(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int getRightMargin(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int getTopMargin(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int getBottomMargin(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void setEnabledCapturePreview(NM_WindowCapture* windowCaptureObj, bool enabled);

extern "C" __declspec(dllexport) int getCapturePreviewWidth();

extern "C" __declspec(dllexport) int getCapturePreviewHeight();

extern "C" __declspec(dllexport) bool isCapturing(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int getCaptureWindowWidth(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int getCaptureWindowHeight(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) float getCaptureFPS(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void stopVirtualCamera(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void deleteWindowCaptureObject(NM_WindowCapture *windowCaptureObj);

extern "C" __declspec(dllexport) void shutdownForMF();
