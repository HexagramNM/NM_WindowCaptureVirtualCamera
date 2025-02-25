
#include "pch.h"
#include "Tools.h"
#include "Activator.h"
#include "../global_config.h"

HMODULE _hModule;


// レジストリ操作関連
struct registry_traits
{
    using type = HKEY;

    static void close(type value) noexcept
    {
        WINRT_VERIFY_(ERROR_SUCCESS, RegCloseKey(value));
    }

    static constexpr type invalid() noexcept
    {
        return nullptr;
    }
};

using registry_key = winrt::handle_type<registry_traits>;

const LSTATUS RegWriteKey(HKEY key, PCWSTR path, HKEY* outKey)
{
    *outKey = nullptr;
    return RegCreateKeyEx(key, path, 0, nullptr, 0, KEY_WRITE, nullptr, outKey, nullptr);
}

const LSTATUS RegWriteValue(HKEY key, PCWSTR name, const std::wstring& value)
{
    return RegSetValueEx(key, name, 0, REG_SZ, reinterpret_cast<BYTE const*>(value.c_str()), static_cast<uint32_t>((value.size() + 1) * sizeof(wchar_t)));
}

const LSTATUS RegWriteValue(HKEY key, PCWSTR name, DWORD value)
{
    return RegSetValueEx(key, name, 0, REG_DWORD, reinterpret_cast<BYTE const*>(&value), sizeof(value));
}
// レジストリ操作関連 ここまで


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        _hModule = hModule;
        DisableThreadLibraryCalls(hModule);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    
    return TRUE;
}

struct ClassFactory : winrt::implements<ClassFactory, IClassFactory>
{
    STDMETHODIMP CreateInstance(IUnknown* outer, GUID const& riid, void** result) noexcept final
    {
        RETURN_HR_IF_NULL(E_POINTER, result);
        *result = nullptr;
        if (outer)
            RETURN_HR(CLASS_E_NOAGGREGATION);

        auto vcam = winrt::make_self<Activator>();
        RETURN_IF_FAILED(vcam->Initialize());
        auto hr = vcam->QueryInterface(riid, result);
        return hr;
    }

    STDMETHODIMP LockServer(BOOL) noexcept final
    {
        return S_OK;
    }
};

__control_entrypoint(DllExport)
STDAPI DllCanUnloadNow()
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }

    winrt::clear_factory_cache();
    return S_OK;
}

_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID FAR* ppv)
{
    RETURN_HR_IF_NULL(E_POINTER, ppv);
    *ppv = nullptr;

    if (rclsid == CLSID_NM_WCVCam_MF)
        return winrt::make_self<ClassFactory>()->QueryInterface(riid, ppv);

    RETURN_HR(E_NOINTERFACE);
}

STDAPI DllRegisterServer()
{
    std::wstring exePath = wil::GetModuleFileNameW(_hModule).get();
    auto clsid = GUID_ToStringW(CLSID_NM_WCVCam_MF, false);
    std::wstring path = L"Software\\Classes\\CLSID\\" + clsid + L"\\InprocServer32";

    // note: a vcam *must* be registered in HKEY_LOCAL_MACHINE
    // for the frame server to be able to talk with it.
    registry_key key;
    RETURN_IF_WIN32_ERROR(RegWriteKey(HKEY_LOCAL_MACHINE, path.c_str(), key.put()));
    RETURN_IF_WIN32_ERROR(RegWriteValue(key.get(), nullptr, exePath));
    RETURN_IF_WIN32_ERROR(RegWriteValue(key.get(), L"ThreadingModel", L"Both"));
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    std::wstring exePath = wil::GetModuleFileNameW(_hModule).get();
    auto clsid = GUID_ToStringW(CLSID_NM_WCVCam_MF, false);
    std::wstring path = L"Software\\Classes\\CLSID\\" + clsid;
    RETURN_IF_WIN32_ERROR(RegDeleteTree(HKEY_LOCAL_MACHINE, path.c_str()));
    return S_OK;
}