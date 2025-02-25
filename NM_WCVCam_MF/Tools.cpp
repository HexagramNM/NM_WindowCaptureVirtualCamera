
#include "pch.h"
#include "Undocumented.h"
#include "Tools.h"
#include "../global_config.h"

std::string to_string(const std::wstring& ws)
{
    if (ws.empty())
        return std::string();

    auto wsize = (int)ws.size();
    auto ssize = WideCharToMultiByte(CP_THREAD_ACP, 0, ws.data(), wsize, nullptr, 0, nullptr, nullptr);
    if (!ssize)
        return std::string();

    std::string s;
    s.resize(ssize);
    ssize = WideCharToMultiByte(CP_THREAD_ACP, 0, ws.data(), wsize, &s[0], ssize, nullptr, nullptr);
    if (!ssize)
        return std::string();

    return s;
}

#define IFIID(x) if (guid == __uuidof(##x)) return L#x;
#define IFGUID(x) if (guid == ##x) return L#x;

const std::string GUID_ToStringA(const GUID& guid, bool resolve) { return to_string(GUID_ToStringW(guid, resolve)); }
const std::wstring GUID_ToStringW(const GUID& guid, bool resolve)
{
    if (resolve)
    {
        // list of known GUIDs we're interested in
        IFGUID(GUID_NULL);
        IFGUID(CLSID_NM_WCVCam_MF);
        IFGUID(PINNAME_VIDEO_CAPTURE);
        IFGUID(MF_DEVICESTREAM_STREAM_CATEGORY);
        IFGUID(MF_DEVICESTREAM_STREAM_ID);
        IFGUID(MF_DEVICESTREAM_FRAMESERVER_SHARED);
        IFGUID(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES);
        IFGUID(MF_DEVICESTREAM_MULTIPLEXED_MANAGER);
        IFGUID(MF_DEVICEMFT_SENSORPROFILE_COLLECTION);
        IFGUID(MF_DEVSOURCE_ATTRIBUTE_D3D_ADAPTERLUID);
        IFGUID(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME);
        IFGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK);
        IFGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE);
        IFGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
        IFGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_CATEGORY);
        IFGUID(MF_DEVSOURCE_ATTRIBUTE_DEVICETYPE);
        IFGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_HW_SOURCE);
        IFGUID(MF_VIRTUALCAMERA_PROVIDE_ASSOCIATED_CAMERA_SOURCES);
        IFGUID(MF_VIRTUALCAMERA_CONFIGURATION_APP_PACKAGE_FAMILY_NAME);
        IFGUID(MF_VIRTUALCAMERA_ASSOCIATED_CAMERA_SOURCES);
        IFGUID(MF_CAPTURE_ENGINE_SELECTEDCAMERAPROFILE_INDEX);
        IFGUID(MF_CAPTURE_ENGINE_SELECTEDCAMERAPROFILE);
        IFGUID(MF_MEDIACAPTURE_INIT_ENABLE_MULTIPLEXOR);
        IFGUID(MF_FRAMESERVER_CLIENTCONTEXT_CLIENTPID);
        IFGUID(MF_FRAMESERVER_VCAM_CONFIGURATION_APP);
        IFGUID(MF_DEVICE_DSHOW_BRIDGE_FILTER);
        IFGUID(MF_DEVPROXY_COMPRESSED_MEDIATYPE_PASSTHROUGH_MODE);
        IFGUID(MF_DEVICESTREAM_ATTRIBUTE_PLUGIN_ENABLED);
        IFGUID(MEDIA_TELEMETRY_SESSION_ID);
        IFGUID(MFT_TRANSFORM_CLSID_Attribute);

        IFGUID(MF_MT_FRAME_SIZE);
        IFGUID(MF_MT_AVG_BITRATE);
        IFGUID(MF_MT_MAJOR_TYPE);
        IFGUID(MF_MT_FRAME_RATE);
        IFGUID(MF_MT_PIXEL_ASPECT_RATIO);
        IFGUID(MF_MT_ALL_SAMPLES_INDEPENDENT);
        IFGUID(MF_MT_INTERLACE_MODE);
        IFGUID(MF_MT_SUBTYPE);
        IFGUID(MF_MT_SUBTYPE);

        IFGUID(MFT_SUPPORT_3DVIDEO);
        IFGUID(MF_SA_D3D11_AWARE);

        IFGUID(KSCATEGORY_VIDEO_CAMERA);
        IFGUID(KSDATAFORMAT_TYPE_VIDEO);
        IFGUID(CLSID_VideoInputDeviceCategory);
        IFGUID(MFVideoFormat_RGB32);
        IFGUID(MFVideoFormat_NV12);

        IFGUID(KSPROPSETID_Pin);
        IFGUID(KSPROPSETID_Topology);
        IFGUID(KSPROPSETID_Connection);
        IFGUID(PROPSETID_VIDCAP_CAMERACONTROL);
        IFGUID(PROPSETID_VIDCAP_VIDEOPROCAMP);
        IFGUID(PROPSETID_VIDCAP_CAMERACONTROL_REGION_OF_INTEREST);
        IFGUID(PROPSETID_VIDCAP_CAMERACONTROL_IMAGE_PIN_CAPABILITY);
        IFGUID(KSPROPERTYSETID_PerFrameSettingControl);
        IFGUID(KSPROPERTYSETID_ExtendedCameraControl);

        IFIID(IUnknown);
        IFIID(IInspectable);
        IFIID(IClassFactory);
        IFIID(IPersistPropertyBag);
        IFIID(IUndocumented1);
        IFIID(INoMarshal);
        IFIID(IMFMediaStream2);
        IFIID(IKsControl);
        IFIID(IMFMediaSourceEx);
        IFIID(IMFMediaSource);
        IFIID(IMFMediaSource2);
        IFIID(IMFDeviceController);
        IFIID(IMFDeviceController2);
        IFIID(IMFDeviceTransformManager);
        IFIID(IMFSampleAllocatorControl);
        IFIID(IMFDeviceSourceInternal);
        IFIID(IMFDeviceSourceInternal2);
        IFIID(IMFCollection);
        IFIID(IMFRealTimeClientEx);
        IFIID(IMFDeviceSourceStatus);
        IFIID(IMFAttributes);
    }

    wchar_t name[64];
    std::ignore = StringFromGUID2(guid, name, _countof(name));
    return name;
}
