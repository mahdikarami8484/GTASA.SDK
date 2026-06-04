#include "core/event/event_bus.h"
#include "core/logging/logger.hpp"
#include "core/script/auto_register_script.h"
#include "core/script/script.h"
#include "core/sdk_runtime.h"
#include "hooks/d3d9_hook.h"
#include "hooks/windows_hook.h"

using namespace Logging;

namespace GTASA
{
    namespace SDK
    {

        class WindowedScript : public Script
        {

        private:
            int m_windowWidth = 640;
            int m_windowHeight = 480;

        public:
            WindowedScript() {}

            virtual void onEvent(std::shared_ptr<BaseEvent> event) override
            {
                if (const auto* initializeEvent =
                        dynamic_cast<const Events::InitializeEvent*>(event.get()))
                    LOG_INFO("[WindowedScript] initialized.");

                if (auto* createWindowExAEvent =
                        dynamic_cast<Events::CreateWindowExAEvent*>(event.get()))
                {
                    LONG_PTR finalStyle =
                        (createWindowExAEvent->dwStyle & ~WS_POPUP) | WS_OVERLAPPEDWINDOW;
                    LONG_PTR finalExStyle = createWindowExAEvent->dwExStyle | WS_EX_APPWINDOW;

                    RECT rcClient = {0, 0, m_windowWidth, m_windowHeight};
                    AdjustWindowRectEx(&rcClient, finalStyle, FALSE, finalExStyle);

                    int finalWidth = rcClient.right - rcClient.left;
                    int finalHeight = rcClient.bottom - rcClient.top;

                    createWindowExAEvent->dwExStyle = finalExStyle;
                    createWindowExAEvent->dwStyle = finalStyle;

                    createWindowExAEvent->nWidth = finalWidth;
                    createWindowExAEvent->nHeight = finalHeight;
                }

                if (auto* createDeviceEvent = dynamic_cast<Events::CreateDeviceEvent*>(event.get()))
                {
                    createDeviceEvent->pPresentationParameters->Windowed = TRUE;
                    createDeviceEvent->pPresentationParameters->BackBufferCount = 1;
                    createDeviceEvent->pPresentationParameters->SwapEffect = D3DSWAPEFFECT_DISCARD;
                    createDeviceEvent->pPresentationParameters->EnableAutoDepthStencil = TRUE;
                    createDeviceEvent->pPresentationParameters->AutoDepthStencilFormat =
                        D3DFMT_D24S8;
                    createDeviceEvent->pPresentationParameters->PresentationInterval =
                        D3DPRESENT_INTERVAL_IMMEDIATE;
                    createDeviceEvent->pPresentationParameters->FullScreen_RefreshRateInHz =
                        0; // required for windowed

                    // Try to use desktop display format (avoid incompatible backbuffer formats)
                    D3DDISPLAYMODE dm = {};
                    if (SUCCEEDED(createDeviceEvent->pD3D->GetAdapterDisplayMode(
                            createDeviceEvent->Adapter, &dm)))
                    {
                        createDeviceEvent->pPresentationParameters->BackBufferFormat = dm.Format;
                    }
                    else
                    {
                        // fallback to common format
                        createDeviceEvent->pPresentationParameters->BackBufferFormat =
                            D3DFMT_X8R8G8B8;
                    }

                    // Ensure device window is set to the focus window (game may override)
                    createDeviceEvent->pPresentationParameters->hDeviceWindow =
                        createDeviceEvent->hFocusWindow ? createDeviceEvent->hFocusWindow
                                                        : GetForegroundWindow();

                    // Force the backbuffer size to match the fixed client size used by this script
                    createDeviceEvent->pPresentationParameters->BackBufferWidth = m_windowWidth;
                    createDeviceEvent->pPresentationParameters->BackBufferHeight = m_windowHeight;
                }
            }
        };

    } // namespace SDK
} // namespace GTASA

namespace
{
    GTASA::SDK::AutoRegisterScript<GTASA::SDK::WindowedScript> _autoReg;
}
