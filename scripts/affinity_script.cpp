#pragma once

#include "core/event/event_bus.h"
#include "core/logging/logger.hpp"
#include "core/script/auto_register_script.h"
#include "core/script/script.h"
#include "core/sdk_runtime.h"

using namespace Logging;

namespace GTASA
{
    namespace SDK
    {

        class AffinityScript : public Script
        {
        public:
            AffinityScript() {}

            virtual void onEvent(std::shared_ptr<BaseEvent> event) override
            {
                if (const auto* initializeEvent =
                        dynamic_cast<const Events::InitializeEvent*>(event.get()))
                {
                    LOG_INFO("[AffinityService] initialized.");

                    HANDLE hProcess = GetCurrentProcess();
                    DWORD_PTR processAffinityMask = 1; // First CPU core
                    if (SetProcessAffinityMask(hProcess, processAffinityMask))
                    {
                        LOG_INFO("[AffinityService] Process affinity set to first CPU core.\n");
                    }
                    else
                    {
                        LOG_ERROR("[AffinityService] Failed to set process affinity.\n");
                    }
                }
            }
        };

    } // namespace SDK
} // namespace GTASA

namespace
{
    GTASA::SDK::AutoRegisterScript<GTASA::SDK::AffinityScript> _autoReg;
}
