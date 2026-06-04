#pragma once

#include "core/event/base_event.h"

namespace GTASA
{
    namespace SDK
    {

        class Script
        {
        public:
            Script() = default;
            virtual ~Script() = default;

            // ----------------------------
            // Lifecycle Events
            // ----------------------------
            virtual void onEvent(std::shared_ptr<BaseEvent> event) = 0;

            // ----------------------------
            // State Control
            // ----------------------------
            bool isEnabled() const
            {
                return m_enabled;
            }
            void setEnabled(bool enabled)
            {
                m_enabled = enabled;
            }

        private:
            bool m_enabled = true;
        };

    } // namespace SDK
} // namespace GTASA
