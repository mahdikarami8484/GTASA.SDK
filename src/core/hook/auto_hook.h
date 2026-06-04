#pragma once

#include <memory>

#include "hook_registry.h"
#include "i_hook.h"

namespace GTASA
{
    namespace SDK
    {

        template <typename T> class AutoHook
        {
        public:
            AutoHook()
            {
                HookRegistry::instance().add(std::make_unique<T>());
            }
        };
    } // namespace SDK
} // namespace GTASA