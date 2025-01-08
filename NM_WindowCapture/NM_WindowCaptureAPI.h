#pragma once

#include "NM_WindowCapture.h"

extern "C" __declspec(dllexport) NM_WindowCapture *CreateWindowCaptureObject(HWND baseHwnd);

extern "C" __declspec(dllexport) void CopyCapturePreviewToDXGIResource(NM_WindowCapture* windowCaptureObj, void* resourcePtr);

extern "C" __declspec(dllexport) void OpenWindowPicker(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void SetTargetWindowForCapture(NM_WindowCapture* windowCaptureObj, HWND hwnd);

extern "C" __declspec(dllexport) void StartVirtualCamera(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void SwitchReverseCaptureWindow(NM_WindowCapture *windowCaptureObj);

extern "C" __declspec(dllexport) void SetLeftMargin(NM_WindowCapture* windowCaptureObj, int margin);

extern "C" __declspec(dllexport) void SetRightMargin(NM_WindowCapture* windowCaptureObj, int margin);

extern "C" __declspec(dllexport) void SetTopMargin(NM_WindowCapture* windowCaptureObj, int margin);

extern "C" __declspec(dllexport) void SetBottomMargin(NM_WindowCapture* windowCaptureObj, int margin);

extern "C" __declspec(dllexport) int GetLeftMargin(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int GetRightMargin(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int GetTopMargin(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int GetBottomMargin(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void SetEnabledCapturePreview(NM_WindowCapture* windowCaptureObj, bool enabled);

extern "C" __declspec(dllexport) int GetCapturePreviewWidth();

extern "C" __declspec(dllexport) int GetCapturePreviewHeight();

extern "C" __declspec(dllexport) bool IsCapturing(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int GetCaptureWindowWidth(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) int GetCaptureWindowHeight(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) float GetCaptureFPS(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void SetCaptureCursor(NM_WindowCapture* windowCaptureObj, bool isCaptured);

extern "C" __declspec(dllexport) void StopVirtualCamera(NM_WindowCapture* windowCaptureObj);

extern "C" __declspec(dllexport) void DeleteWindowCaptureObject(NM_WindowCapture *windowCaptureObj);
