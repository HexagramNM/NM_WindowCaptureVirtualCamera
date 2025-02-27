#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#ifdef _DEBUG
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

// Windows Header Files
#include <windows.h>
#include <initguid.h>
#include <propvarutil.h>
#include <mfapi.h>
#include <mfvirtualcamera.h>
#include <mferror.h>
#include <mfcaptureengine.h>
#include <ks.h>
#include <ksproxy.h>
#include <ksmedia.h>
#include <dxgi.h>
#include <d3d11.h>
#include <dxgi.h>
#include <uuids.h>

// std
#include <string>
#include <format>

// WIL, requires "Microsoft.Windows.ImplementationLibrary" nuget
#include "wil/win32_helpers.h"
#include "wil/com.h"

// C++/WinRT, requires "Microsoft.Windows.CppWinRT" nuget
#include "winrt/Windows.ApplicationModel.h"

#pragma comment(lib, "mfsensorgroup")
#pragma comment(lib, "dxgi.lib")
