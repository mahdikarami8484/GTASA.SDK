#include "d3d9_hook.h"

using namespace GTASA::SDK;

namespace
{
    // --------------------------------------------------
    // DirectX9 function pointer
    // --------------------------------------------------
    using t_Reset = HRESULT(WINAPI*)(IDirect3DDevice9* pDevice,
                                     D3DPRESENT_PARAMETERS* pPresentationParameters);
    t_Reset o_Reset = nullptr;

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

    HRESULT WINAPI hk_Reset(IDirect3DDevice9* pDevice,
                            D3DPRESENT_PARAMETERS* pPresentationParameters)
    {
        if (!pPresentationParameters) return o_Reset(pDevice, pPresentationParameters);

        auto resetEvent =
            std::make_shared<Events::ResetDeviceEvent>(pDevice, pPresentationParameters);
        EventBus::instance().dispatch(resetEvent);

        return o_Reset(pDevice, resetEvent->pPresentationParameters);
    }

} // anonymous namespace

// --------------------------------------------------
// IHook implementation
// --------------------------------------------------
void D3D9Hook::install()
{
    HWND dummyHwnd =
        CreateWindowA("BUTTON", "Dummy", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, nullptr, nullptr);
    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (d3d && dummyHwnd)
    {
        D3DPRESENT_PARAMETERS d3dpp = {};
        d3dpp.Windowed = TRUE;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow = dummyHwnd;

        IDirect3DDevice9* dummyDevice = nullptr;
        d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dummyHwnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &dummyDevice);

        if (dummyDevice)
        {
            void** vTableDevice = *reinterpret_cast<void***>(dummyDevice);
            o_Reset = reinterpret_cast<t_Reset>(vTableDevice[16]);

            HookManager::instance().addHook(reinterpret_cast<void**>(&o_Reset),
                                            reinterpret_cast<void*>(hk_Reset));

            LOG_INFO("[D3D9Hook] Device Reset hooked safely!");

            dummyDevice->Release();
        }

        void** vTableD3D = *reinterpret_cast<void***>(d3d);
        o_CreateDevice = reinterpret_cast<decltype(&hk_CreateDevice)>(vTableD3D[16]);

        HookManager::instance().addHook(reinterpret_cast<void**>(&o_CreateDevice),
                                        reinterpret_cast<void*>(hk_CreateDevice));

        LOG_INFO("[D3D9Hook] CreateDevice hooked!");
        d3d->Release();
    }

    if (dummyHwnd) DestroyWindow(dummyHwnd);
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
