#pragma once

#include <memory>

#include "core/event/event_bus.h"

namespace GTASA
{
    namespace SDK
    {

        template <typename T> class AutoRegisterScript
        {
        public:
            AutoRegisterScript()
            {
                EventBus::instance().add(std::make_unique<T>());
            }
        };

    } // namespace SDK
} // namespace GTASA
