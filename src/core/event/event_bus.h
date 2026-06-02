#pragma once

#include "base_event.h"
#include "core/script/script.h"
#include "memory"

#include <unordered_map>
#include <vector>

namespace GTASA
{
    namespace SDK
    {

        class EventBus
        {
        public:
            static EventBus& instance();

            EventBus(const EventBus&) = delete;
            EventBus& operator=(const EventBus&) = delete;

            // add a script
            void add(std::unique_ptr<Script> script);

            // Dispatch an event
            void dispatch(std::shared_ptr<BaseEvent> event);

        private:
            EventBus() = default;
            ~EventBus() = default;

        private:
            std::vector<std::unique_ptr<Script>> m_scripts;
        };

    } // namespace SDK
} // namespace GTASA
