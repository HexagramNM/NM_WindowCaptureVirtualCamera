
#pragma once

#define WIN32_LEAN_AND_MEAN

#define _CRT_SECURE_NO_DEPRECATE
#pragma warning(disable : 4995)
#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#pragma comment(lib, "strmbasd.lib")
#else
#pragma comment(lib, "strmbase.lib")
#endif

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include <streams.h>
#include <initguid.h>

#include <chrono>

#define TEMPLATE_NAME	(L"NM_WCVCam_DS")
#define FILTER_NAME		(TEMPLATE_NAME)
#define OUTPUT_PIN_NAME (L"Output")

#include <d3d11_4.h>
#pragma comment(lib, "d3d11.lib")

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")

#include "winrt/base.h"
using namespace winrt;

#define PIXEL_BIT 24
#define PIXEL_BYTE 3
#define CS_THREADS_NUM 20

#include "../global_config.h"
#include "NMVCamFilter.h"
