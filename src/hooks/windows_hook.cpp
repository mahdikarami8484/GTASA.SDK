#include "windows_hook.h"

using namespace GTASA::SDK;

namespace
{

    // --------------------------------------------------
    // Windows function pointer
    // --------------------------------------------------

    using t_CreateWindowExA = HWND(__stdcall*)(DWORD dwExStyle, LPCSTR lpClassName,
                                               LPCSTR lpWindowName, DWORD dwStyle, int X, int Y,
                                               int nWidth, int nHeight, HWND hWndParent,
                                               HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);

    t_CreateWindowExA o_CreateWindowExA = nullptr;

    HWND __stdcall hk_CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
                                      DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                      HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                      LPVOID lpParam)
    {
        if (!o_CreateWindowExA)
        {
            LOG_ERROR("[WindowsFuncsHook] CreateWindowExA original function is null.");

            return CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth,
                                   nHeight, hWndParent, hMenu, hInstance, lpParam);
        }

        auto createWindowExAEvent = std::make_shared<Events::CreateWindowExAEvent>(
            dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu,
            hInstance, lpParam);

        EventBus::instance().dispatch(createWindowExAEvent);

        return o_CreateWindowExA(createWindowExAEvent->dwExStyle, createWindowExAEvent->lpClassName,
                                 createWindowExAEvent->lpWindowName, createWindowExAEvent->dwStyle,
                                 createWindowExAEvent->X, createWindowExAEvent->Y,
                                 createWindowExAEvent->nWidth, createWindowExAEvent->nHeight,
                                 createWindowExAEvent->hWndParent, createWindowExAEvent->hMenu,
                                 createWindowExAEvent->hInstance, createWindowExAEvent->lpParam);
    }

} // anonymous namespace

// --------------------------------------------------
// IHook implementation
// --------------------------------------------------

bool WindowsFuncsHook::install()
{
    LOG_INFO("[WindowsFuncsHook] Installing Windows function hooks...");

    HMODULE user32 = GetModuleHandleA("user32.dll");

    if (!user32)
    {
        LOG_ERROR("[WindowsFuncsHook] Failed to get user32.dll module handle.");

        return false;
    }

    o_CreateWindowExA =
        reinterpret_cast<t_CreateWindowExA>(GetProcAddress(user32, "CreateWindowExA"));

    if (!o_CreateWindowExA)
    {
        LOG_ERROR("[WindowsFuncsHook] Failed to resolve CreateWindowExA address.");

        return false;
    }

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_CreateWindowExA),
                                    reinterpret_cast<void*>(hk_CreateWindowExA));

    LOG_DEBUG("[WindowsFuncsHook] CreateWindowExA resolved at: %p",
              reinterpret_cast<void*>(o_CreateWindowExA));

    LOG_INFO("[WindowsFuncsHook] Windows function hooks installed successfully.");

    return true;
}

bool WindowsFuncsHook::uninstall()
{
    // HookManager handles hook removal.

    LOG_DEBUG("[WindowsFuncsHook] Uninstall requested.");

    return true;
}

// --------------------------------------------------
// Auto register
// --------------------------------------------------

namespace
{
    AutoHook<WindowsFuncsHook> _autoWindowsFuncsHook;
}