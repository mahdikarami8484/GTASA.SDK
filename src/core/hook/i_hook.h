#pragma once

#include "string"

namespace GTASA
{
    namespace SDK
    {

        class IHook
        {
        public:
            virtual ~IHook() = default;

            /* Meta */
            virtual const char* getName() const = 0;
            virtual int Priority() const = 0;

            /* State */
            virtual bool isEnabled() const
            {
                return true;
            }

            /* Lifecycle */
            virtual void install() = 0;
            virtual void uninstall() = 0;
        };

    } // namespace SDK
} // namespace GTASA