#pragma once

#define VCAM_VIDEO_WIDTH 1920
#define VCAM_VIDEO_HEIGHT 1080
#define MUTEX_KEY 5556

#define SHARED_CAPTURE_WINDOW_TEXTURE_PATH (L"Global\\NM_WCVCam_Captured_Window")

// {be5f13f7-0667-47a8-a5cc-38c53373135b}
DEFINE_GUID(CLSID_NM_WCVCam_DS,
    0xbe5f13f7, 0x667, 0x47a8, 0xa5, 0xcc, 0x38, 0xc5, 0x33, 0x73, 0x13, 0x5b);
#define NAME_NM_WCVCam_DS (L"NM_WCVCam_DS")

// {cc593221-3915-4084-bf70-dd9d8b481b4e}
DEFINE_GUID(CLSID_NM_WCVCam_MF,
    0xcc593221, 0x3915, 0x4084, 0xbf, 0x70, 0xdd, 0x9d, 0x8b, 0x48, 0x1b, 0x4e);
#define CLSID_TEXT_NM_WCVCam_MF (L"{cc593221-3915-4084-bf70-dd9d8b481b4e}")
#define NAME_NM_WCVCam_MF (L"NM_WCVCam_MF")
