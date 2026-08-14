#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

#include "base_event.h"
#include "core/logging/logger.h"
#include "core/script/script.h"

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
