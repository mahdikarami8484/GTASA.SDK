#pragma once

#include "core/event/event_bus.h"

#include <memory>

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
