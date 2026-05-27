#pragma once

#include <cstdint>

namespace GTASA
{
    namespace SDK
    {

        class GameBase
        {
        public:
            static void initialize();

            static uintptr_t base();

            static uintptr_t address(uintptr_t offset);

        private:
            GameBase() = delete; // static-only class

        private:
            static uintptr_t s_base;
            static bool s_initialized;
        };

    } // namespace SDK
} // namespace GTASA
