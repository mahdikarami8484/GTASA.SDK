#pragma once

#include "memory"

namespace GTASA
{
    namespace SDK
    {
        class BaseEvent
        {
        public:
            virtual const char* getName() const = 0;
            virtual ~BaseEvent() = default;
        };
    } // namespace SDK

} // namespace GTASA