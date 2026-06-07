#include "windows_hook.h"

#include "player_hook.h"

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

    using t_SetWindowLongA = LONG(__stdcall*)(HWND, int, LONG);
    t_SetWindowLongA o_SetWindowLongA = nullptr;

    LONG __stdcall hk_SetWindowLongA(HWND hWnd, int nIndex, LONG dwNewLong)
    {
        if (!o_SetWindowLongA) return NULL;
        auto setWindowEvent =
            std::make_shared<Events::SetWindowLongAEvent>(hWnd, nIndex, dwNewLong);
        EventBus::instance().dispatch(setWindowEvent);
        return o_SetWindowLongA(hWnd, nIndex, setWindowEvent->dwNewLong);
    }

} // anonymous namespace

// --------------------------------------------------
// IHook implementation
// --------------------------------------------------
void WindowsFuncsHook::install()
{
    HMODULE user32 = LoadLibraryA("user32.dll");
    if (user32)
    {
        o_CreateWindowExA = reinterpret_cast<decltype(&hk_CreateWindowExA)>(
            GetProcAddress(user32, "CreateWindowExA"));
        HookManager::instance().addHook(reinterpret_cast<void**>(&o_CreateWindowExA),
                                        reinterpret_cast<void*>(hk_CreateWindowExA));
        LOG_INFO("[WindowsFuncsHook] CreateWindowExA func hooked!");

        o_SetWindowLongA = reinterpret_cast<decltype(&hk_SetWindowLongA)>(
            GetProcAddress(user32, "SetWindowLongA"));
        if (o_SetWindowLongA)
        {
            HookManager::instance().addHook(reinterpret_cast<void**>(&o_SetWindowLongA),
                                            reinterpret_cast<void*>(hk_SetWindowLongA));
            LOG_INFO("[WindowsFuncsHook] SetWindowLongA func hooked!");
        }
    }
}

void WindowsFuncsHook::uninstall()
{
    // handled by HookManager
}

// --------------------------------------------------
// Auto register
// --------------------------------------------------
namespace
{
    AutoHook<WindowsFuncsHook> _autoWindowsFuncsHook;
}
