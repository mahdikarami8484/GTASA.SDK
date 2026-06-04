#pragma once

#include "core/event/base_event.h"
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

        class PlayerHook final : public IHook
        {
        public:
            virtual const char* getName() const override
            {
                return "PlayerHook";
            }

            virtual int Priority() const override
            {
                return 15;
            }

            virtual void install() override;

            virtual void uninstall() override;
        };

        namespace Events
        {
            class CrimeCommittedEvent : public BaseEvent
            {
            public:
                int* this_ptr;
                void* param_2;
                void* param_3;

                CrimeCommittedEvent(int* this_ptr, void* param_2, void* param_3)
                    : this_ptr(this_ptr), param_2(param_2), param_3(param_3)
                {
                }

                virtual const char* getName() const
                {
                    return "CrimeCommittedEvent";
                }
            };
        } // namespace Events

    } // namespace SDK
} // namespace GTASA