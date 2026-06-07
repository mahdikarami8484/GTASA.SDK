#pragma once

#include "core/event/base_event.h"
#include "core/game/game_base.h"
#include "core/hook/auto_hook.h"
#include "core/hook/hook_manager.h"
#include "core/hook/i_hook.h"
#include "core/logging/logger.hpp"

namespace GTASA
{
    namespace SDK
    {

        class GraphicsHook final : public IHook
        {
        public:
            virtual const char* getName() const override
            {
                return "GraphicsHook";
            }
            virtual int Priority() const override
            {
                return 150;
            }
            virtual void install() override;
            virtual void uninstall() override;
        };

        namespace Events
        {
            class SetResolutionEvent : public BaseEvent
            {
            public:
                int* resolutionData;
                unsigned int param_2;
                unsigned int param_3;

                SetResolutionEvent(int* resData, unsigned int p2, unsigned int p3)
                    : resolutionData(resData), param_2(p2), param_3(p3)
                {
                }

                virtual const char* getName() const override
                {
                    return "SetResolutionEvent";
                }
            };
        } // namespace Events
    } // namespace SDK
} // namespace GTASA