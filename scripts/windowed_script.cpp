#include "core/event/event_bus.h"
#include "core/logging/logger.hpp"
#include "core/script/auto_register_script.h"
#include "core/script/script.h"
#include "hooks/d3d9_hook.h"
#include "hooks/graphics_hook.h"
#include "hooks/windows_hook.h"

namespace GTASA
{
    namespace SDK
    {
        class WindowedScript : public Script
        {

        private:
            int m_windowWidth = 0;
            int m_windowHeight = 0;

            // New Code
            int m_desktopW = 0;
            int m_desktopH = 0;

            // New Code
            bool isOversized() const
            {
                return (m_windowWidth >= m_desktopW || m_windowHeight >= m_desktopH);
            }

            // New Code
            void ApplyWindowState(HWND hwnd)
            {
                if (!hwnd) return;

                HICON hIcon = (HICON)SendMessageA(hwnd, WM_GETICON, ICON_SMALL, 0);
                if (!hIcon)
                {
                    hIcon = (HICON)GetClassLongPtrA(hwnd, GCLP_HICONSM);
                }

                if (isOversized())
                {
                    LOG_INFO("[WindowedScript] Mode: BORDERLESS FULLSCREEN (%dx%d)", m_windowWidth,
                             m_windowHeight);
                    SetWindowLongA(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
                    SetWindowPos(hwnd, HWND_TOP, 0, 0, m_desktopW, m_desktopH,
                                 SWP_FRAMECHANGED | SWP_SHOWWINDOW);
                }
                else
                {
                    LOG_INFO("[WindowedScript] Mode: WINDOWED (%dx%d)", m_windowWidth,
                             m_windowHeight);
                    LONG_PTR style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
                    SetWindowLongA(hwnd, GWL_STYLE, style);

                    RECT rc = {0, 0, m_windowWidth, m_windowHeight};
                    AdjustWindowRectEx(&rc, style, FALSE, 0);

                    int w = rc.right - rc.left;
                    int h = rc.bottom - rc.top;
                    int x = (m_desktopW - w) / 2;
                    int y = (m_desktopH - h) / 2;

                    SetWindowPos(hwnd, HWND_NOTOPMOST, x, y, w, h,
                                 SWP_FRAMECHANGED | SWP_SHOWWINDOW);

                    RedrawWindow(hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW);
                }

                if (hIcon)
                {
                    SendMessageA(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
                    SendMessageA(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
                }
            }

        public:
            WindowedScript() {}

            // Modified Code
            virtual void onEvent(std::shared_ptr<BaseEvent> event) override
            {
                if (const auto* initializeEvent =
                        dynamic_cast<const Events::InitializeEvent*>(event.get()))
                {
                    LOG_INFO("[WindowedScript] initialized.");
                    m_desktopW = GetSystemMetrics(SM_CXSCREEN);
                    m_desktopH = GetSystemMetrics(SM_CYSCREEN);
                    LOG_INFO("[WindowedScript] initialized. Native Desktop: %dx%d", m_desktopW,
                             m_desktopH);
                }

                if (auto* setLongEvent = dynamic_cast<Events::SetWindowLongAEvent*>(event.get()))
                {
                    if (setLongEvent->nIndex == GWL_STYLE && m_windowWidth > 0 &&
                        m_windowHeight > 0)
                    {
                        if (isOversized())
                        {
                            setLongEvent->dwNewLong = WS_POPUP | WS_VISIBLE;
                        }
                        else
                        {
                            setLongEvent->dwNewLong &= ~(WS_POPUP | WS_MAXIMIZE);
                            setLongEvent->dwNewLong |= WS_OVERLAPPEDWINDOW;
                        }
                    }
                }

                /*				
				Removed Code
				if(auto* createWindowExAEvent = dynamic_cast<Events::CreateWindowExAEvent*>(event.get()))
				{
					LONG_PTR finalStyle = (createWindowExAEvent->dwStyle & ~WS_POPUP) | WS_OVERLAPPEDWINDOW;
					LONG_PTR finalExStyle = createWindowExAEvent->dwExStyle | WS_EX_APPWINDOW;
					
					RECT rcClient = { 0, 0, m_windowWidth, m_windowHeight };
					AdjustWindowRectEx(&rcClient, finalStyle, FALSE, finalExStyle);
					
					int finalWidth = rcClient.right - rcClient.left;
					int finalHeight = rcClient.bottom - rcClient.top;
					
					createWindowExAEvent->dwExStyle = finalExStyle;
					createWindowExAEvent->dwStyle = finalStyle;
					
					createWindowExAEvent->nWidth = finalWidth;
					createWindowExAEvent->nHeight = finalHeight;
                }
*/

                // New Code
                if (auto* resEvent = dynamic_cast<Events::SetResolutionEvent*>(event.get()))
                {

                    m_windowWidth = resEvent->resolutionData[0];
                    m_windowHeight = resEvent->resolutionData[1];

                    //					*reinterpret_cast<int*>(GameBase::address(0x8A2DB0)) = m_windowWidth;
                    //					*reinterpret_cast<int*>(GameBase::address(0x8A2DB4)) = m_windowHeight;

                    LOG_INFO("[WindowedScript] Resolution locked at: %dx%d.", m_windowWidth,
                             m_windowHeight);
                }

                // Modified Code
                if (auto* createDeviceEvent = dynamic_cast<Events::CreateDeviceEvent*>(event.get()))
                {
                    HWND hwnd = FindWindowA(NULL, "GTA: San Andreas");
                    if (hwnd)
                    {

                        // **Fix:** Update m_windowWidth/Height to match the resolution we want the game to be at.
                        // This ensures that isOversized and ApplyWindowState use the correct values.
                        //m_windowWidth = createDeviceEvent->pPresentationParameters->BackBufferWidth;
                        //m_windowHeight = createDeviceEvent->pPresentationParameters->BackBufferHeight;

                        if (m_windowWidth == 0 || m_windowHeight == 0)
                        {
                            m_windowWidth = 800;
                            m_windowHeight = 600;
                            LOG_INFO("[WindowedScript] Game's initial requested resolution was "
                                     "invalid, falling back to %dx%d.",
                                     m_windowWidth, m_windowHeight);
                        }

                        LOG_INFO("[WindowedScript] Configuring DirectX for game-requested "
                                 "resolution: %dx%d...",
                                 m_windowWidth, m_windowHeight);

                        ApplyWindowState(hwnd);

                        createDeviceEvent->pPresentationParameters->BackBufferCount = 1;
                        createDeviceEvent->pPresentationParameters->SwapEffect =
                            D3DSWAPEFFECT_DISCARD;
                        createDeviceEvent->pPresentationParameters->EnableAutoDepthStencil = TRUE;
                        createDeviceEvent->pPresentationParameters->AutoDepthStencilFormat =
                            D3DFMT_D24S8;
                        createDeviceEvent->pPresentationParameters->PresentationInterval =
                            D3DPRESENT_INTERVAL_IMMEDIATE;
                        createDeviceEvent->pPresentationParameters->hDeviceWindow = hwnd;

                        // Force the backbuffer size to match the fixed client size used by this script
                        createDeviceEvent->pPresentationParameters->BackBufferWidth = m_windowWidth;
                        createDeviceEvent->pPresentationParameters->BackBufferHeight =
                            m_windowHeight;

                        if (isOversized())
                        {
                            createDeviceEvent->pPresentationParameters->Windowed = FALSE;

                            D3DDISPLAYMODE dm = {};
                            if (SUCCEEDED(createDeviceEvent->pD3D->GetAdapterDisplayMode(
                                    createDeviceEvent->Adapter, &dm)))
                            {
                                createDeviceEvent->pPresentationParameters->BackBufferFormat =
                                    dm.Format;
                                createDeviceEvent->pPresentationParameters
                                    ->FullScreen_RefreshRateInHz = dm.RefreshRate;
                            }
                            else
                            {
                                createDeviceEvent->pPresentationParameters->BackBufferFormat =
                                    D3DFMT_X8R8G8B8;
                                createDeviceEvent->pPresentationParameters
                                    ->FullScreen_RefreshRateInHz = 60;
                            }
                            LOG_INFO("[WindowedScript] Resolution is too big! Switching to TRUE "
                                     "FULLSCREEN...");
                        }
                        else
                        {
                            createDeviceEvent->pPresentationParameters->Windowed = TRUE;
                            createDeviceEvent->pPresentationParameters->FullScreen_RefreshRateInHz =
                                0; // required for windowed
                            createDeviceEvent->pPresentationParameters->BackBufferFormat =
                                D3DFMT_UNKNOWN;
                            LOG_INFO("[WindowedScript] Applying Windowed Mode %dx%d with borders.",
                                     m_windowWidth, m_windowHeight);
                        }
                    }
                }

                // New Code
                if (auto* resetEvent = dynamic_cast<Events::ResetDeviceEvent*>(event.get()))
                {
                    if (m_windowWidth > 0 && m_windowHeight > 0)
                    {
                        HWND hwnd = FindWindowA(NULL, "GTA: San Andreas");
                        ApplyWindowState(hwnd);

                        resetEvent->pPresentationParameters->BackBufferWidth = m_windowWidth;
                        resetEvent->pPresentationParameters->BackBufferHeight = m_windowHeight;

                        if (isOversized())
                        {
                            resetEvent->pPresentationParameters->Windowed = FALSE;
                            resetEvent->pPresentationParameters->BackBufferFormat = D3DFMT_X8R8G8B8;
                            resetEvent->pPresentationParameters->FullScreen_RefreshRateInHz = 60;
                        }
                        else
                        {
                            resetEvent->pPresentationParameters->Windowed = TRUE;
                            resetEvent->pPresentationParameters->BackBufferFormat = D3DFMT_UNKNOWN;
                            resetEvent->pPresentationParameters->FullScreen_RefreshRateInHz = 0;
                        }
                        LOG_INFO("[WindowedScript] DirectX Reset handled successfully.");
                    }
                }
            }
        };

    } // namespace SDK
} // namespace GTASA

namespace
{
    GTASA::SDK::AutoRegisterScript<GTASA::SDK::WindowedScript> _autoReg;
}