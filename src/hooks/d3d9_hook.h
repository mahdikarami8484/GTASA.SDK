#pragma once

#include <d3d9.h>

#include "core/game/game_base.h"
#include "core/hook/auto_hook.h"
#include "core/hook/hook_manager.h"
#include "core/hook/i_hook.h"
#include "core/logging/logger.hpp"
#include "core/sdk_runtime.h"
#pragma comment(lib, "d3d9.lib")

namespace GTASA
{
    namespace SDK
    {

        class D3D9Hook final : public IHook
        {
        public:
            virtual const char* getName() const override
            {
                return "D3D9Hook";
            }

            virtual int Priority() const override
            {
                return 2;
            }

            virtual void install() override;

            virtual void uninstall() override;
        };

        namespace Events
        {
            class CreateDeviceEvent : public BaseEvent
            {
            public:
                IDirect3D9* pD3D;
                UINT Adapter;
                D3DDEVTYPE DeviceType;
                HWND hFocusWindow;
                DWORD BehaviorFlags;
                D3DPRESENT_PARAMETERS* pPresentationParameters;
                IDirect3DDevice9** ppReturnedDeviceInterface;

                CreateDeviceEvent(IDirect3D9* pD3D, UINT Adapter, D3DDEVTYPE DeviceType,
                                  HWND hFocusWindow, DWORD BehaviorFlags,
                                  D3DPRESENT_PARAMETERS* pPresentationParameters,
                                  IDirect3DDevice9** ppReturnedDeviceInterface)
                    : pD3D(pD3D), Adapter(Adapter), DeviceType(DeviceType),
                      hFocusWindow(hFocusWindow), BehaviorFlags(BehaviorFlags),
                      pPresentationParameters(pPresentationParameters),
                      ppReturnedDeviceInterface(ppReturnedDeviceInterface)
                {
                }

                virtual const char* getName() const
                {
                    return "CreateDeviceEvent";
                }
            };
        } // namespace Events
    } // namespace SDK
} // namespace GTASA