#pragma once

#include "core/game/game_base.h"
#include "core/hook/auto_hook.h"
#include "core/hook/hook_manager.h"
#include "core/hook/i_hook.h"
#include "core/logging/logger.hpp"
#include "core/sdk_runtime.h"

namespace GTASA
{
    namespace SDK
    {

        class WindowsFuncsHook final : public IHook
        {
        public:
            virtual const char* getName() const override
            {
                return "WindowsFuncsHook";
            }

            virtual int Priority() const override
            {
                return 100;
            }

            virtual void install() override;

            virtual void uninstall() override;
        };

        namespace Events
        {
            class CreateWindowExAEvent : public BaseEvent
            {
            public:
                // property
                DWORD dwExStyle;
                LPCSTR lpClassName;
                LPCSTR lpWindowName;
                DWORD dwStyle;
                int X;
                int Y;
                int nWidth;
                int nHeight;
                HWND hWndParent;
                HMENU hMenu;
                HINSTANCE hInstance;
                LPVOID lpParam;

            public:
                CreateWindowExAEvent(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
                                     DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
                                     HWND hWndParent, HMENU hMenu, HINSTANCE hInstance,
                                     LPVOID lpParam)
                    : dwExStyle(dwExStyle), lpClassName(lpClassName), lpWindowName(lpWindowName),
                      dwStyle(dwStyle), X(X), Y(Y), nWidth(nWidth), nHeight(nHeight),
                      hWndParent(hWndParent), hMenu(hMenu), hInstance(hInstance), lpParam(lpParam)
                {
                }

                virtual const char* getName() const
                {
                    return "CreateWindowExAEvent";
                }
            };

        } // namespace Events

    } // namespace SDK
} // namespace GTASA