#pragma once

#include "NM_WindowCapture.h"

extern "C" __declspec(dllexport) void startupForMF();

extern "C" __declspec(dllexport) NM_WindowCapture *createWindowCaptureObject(HWND hwnd);

extern "C" __declspec(dllexport) void switchReverseCaptureWindow(NM_WindowCapture *windowCaptureObj);

extern "C" __declspec(dllexport) void deleteWindowCaptureObject(NM_WindowCapture *windowCaptureObj);

extern "C" __declspec(dllexport) void shutdownForMF();
