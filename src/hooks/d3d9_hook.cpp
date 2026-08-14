#include "d3d9_hook.h"

using namespace GTASA::SDK;

namespace
{
    // DirectX9 function pointer
    using tProcessFrame = int(__cdecl*)(int command, int param);
    tProcessFrame oProcessFrame = nullptr;

    using t_CreateDevice = HRESULT(WINAPI*)(IDirect3D9* pD3D, UINT Adapter, D3DDEVTYPE DeviceType,
                                            HWND hFocusWindow, DWORD BehaviorFlags,
                                            D3DPRESENT_PARAMETERS* pPresentationParameters,
                                            IDirect3DDevice9** ppReturnedDeviceInterface);

    t_CreateDevice o_CreateDevice = nullptr;

    bool g_deviceCreated = false;

    HRESULT WINAPI hk_CreateDevice(IDirect3D9* pD3D, UINT Adapter, D3DDEVTYPE DeviceType,
                                   HWND hFocusWindow, DWORD BehaviorFlags,
                                   D3DPRESENT_PARAMETERS* pPresentationParameters,
                                   IDirect3DDevice9** ppReturnedDeviceInterface)
    {
        // Solved GitHub Copilot's warning. Returning D3DERR_INVALIDCALL directly instead of forwarding null pointers to prevent game crashes
        if (!pPresentationParameters || !pD3D)
        {
            LOG_WARNING("[D3D9Hook] Invalid CreateDevice parameters.");
            return D3DERR_INVALIDCALL;
        }

        if (!g_deviceCreated)
        {
            LOG_INFO("[D3D9Hook] Direct3D device creation intercepted.");
            g_deviceCreated = true;
        }

        auto createDeviceEvent = std::make_shared<Events::CreateDeviceEvent>(
            pD3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters,
            ppReturnedDeviceInterface);

        EventBus::instance().dispatch(createDeviceEvent);

        HRESULT result = o_CreateDevice(
            createDeviceEvent->pD3D, createDeviceEvent->Adapter, createDeviceEvent->DeviceType,
            createDeviceEvent->hFocusWindow, createDeviceEvent->BehaviorFlags,
            createDeviceEvent->pPresentationParameters,
            createDeviceEvent->ppReturnedDeviceInterface);

        if (FAILED(result))
        {
            LOG_ERROR("[D3D9Hook] CreateDevice failed. HRESULT: %ld", result);
        }
        else
        {
            LOG_INFO("[D3D9Hook] Direct3D device created successfully.");
        }

        return result;
    }

} // anonymous namespace

// IHook implementation

bool D3D9Hook::install()
{
    LOG_INFO("[D3D9Hook] Installing D3D9 hook...");

    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
    {
        LOG_ERROR("[D3D9Hook] Failed to create Direct3D9 interface.");
        return false;
    }

    void** vTable = *reinterpret_cast<void***>(d3d);

    if (!vTable)
    {
        LOG_ERROR("[D3D9Hook] Failed to retrieve D3D9 vTable.");
        d3d->Release();
        return false;
    }

    o_CreateDevice = reinterpret_cast<decltype(&hk_CreateDevice)>(vTable[16]);

    if (!o_CreateDevice)
    {
        LOG_ERROR("[D3D9Hook] Failed to locate CreateDevice address.");
        d3d->Release();
        return false;
    }

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_CreateDevice),
                                    reinterpret_cast<void*>(hk_CreateDevice));

    LOG_DEBUG("[D3D9Hook] CreateDevice address: %p", reinterpret_cast<void*>(o_CreateDevice));

    d3d->Release();

    LOG_INFO("[D3D9Hook] D3D9 hook installed successfully.");

    return true;
}

bool D3D9Hook::uninstall()
{
    LOG_INFO("[D3D9Hook] Uninstall requested.");
    // handled by HookManager
    LOG_INFO("[D3D9Hook] D3D9 hook uninstalled.");
    return true;
}

// Auto register
namespace
{
    AutoHook<D3D9Hook> _autoD3D9Hook;
}
