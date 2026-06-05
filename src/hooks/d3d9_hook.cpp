#include "d3d9_hook.h"

using namespace GTASA::SDK;

namespace
{

    // --------------------------------------------------
    // DirectX9 function pointer
    // --------------------------------------------------
    using tProcessFrame = int(__cdecl*)(int command, int param);
    tProcessFrame oProcessFrame = nullptr;

    using t_CreateDevice = HRESULT(WINAPI*)(IDirect3D9* pD3D, UINT Adapter, D3DDEVTYPE DeviceType,
                                            HWND hFocusWindow, DWORD BehaviorFlags,
                                            D3DPRESENT_PARAMETERS* pPresentationParameters,
                                            IDirect3DDevice9** ppReturnedDeviceInterface);
    t_CreateDevice o_CreateDevice = nullptr;

    HRESULT WINAPI hk_CreateDevice(IDirect3D9* pD3D, UINT Adapter, D3DDEVTYPE DeviceType,
                                   HWND hFocusWindow, DWORD BehaviorFlags,
                                   D3DPRESENT_PARAMETERS* pPresentationParameters,
                                   IDirect3DDevice9** ppReturnedDeviceInterface)
    {

        if (!pPresentationParameters || !pD3D)
        {
            return o_CreateDevice(pD3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags,
                                  pPresentationParameters, ppReturnedDeviceInterface);
        }

        auto createDeviceEvent = std::make_shared<Events::CreateDeviceEvent>(
            pD3D, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters,
            ppReturnedDeviceInterface);

        EventBus::instance().dispatch(createDeviceEvent);

        return o_CreateDevice(createDeviceEvent->pD3D, createDeviceEvent->Adapter,
                              createDeviceEvent->DeviceType, createDeviceEvent->hFocusWindow,
                              createDeviceEvent->BehaviorFlags,
                              createDeviceEvent->pPresentationParameters,
                              createDeviceEvent->ppReturnedDeviceInterface);
    }

} // anonymous namespace

// --------------------------------------------------
// IHook implementation
// --------------------------------------------------
void D3D9Hook::install()
{
    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return;

    void** vTable = *reinterpret_cast<void***>(d3d);
    o_CreateDevice = reinterpret_cast<decltype(&hk_CreateDevice)>(vTable[16]);

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_CreateDevice),
                                    reinterpret_cast<void*>(hk_CreateDevice));

    LOG_INFO("[D3D9Hook] CreateDevice hooked!");
    d3d->Release();
}

void D3D9Hook::uninstall()
{
    // handled by HookManager
}

// --------------------------------------------------
// Auto register
// --------------------------------------------------
namespace
{
    AutoHook<D3D9Hook> _autoD3D9Hook;
}
