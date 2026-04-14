#pragma once

#include <cstdint>
#include <stdexcept>

namespace GTASA {
    namespace SDK {

        /**
         * @brief Provides access to the GTA SA base module address.
         *
         * Static-only class that caches the base address of gta_sa.exe
         * and computes absolute addresses from relative offsets.
         */
        class GameBase {
        public:
            /**
             * @brief Initialize the game base address.
             * @throws std::runtime_error if the module handle cannot be obtained.
             */
            static void initialize();

            /// @return The cached base address of gta_sa.exe.
            static uintptr_t base();

            /**
             * @brief Compute an absolute address from a relative offset.
             * @param offset Relative offset from the module base.
             * @return Absolute address (base + offset).
             * @throws std::runtime_error if GameBase has not been initialized.
             */
            static uintptr_t address(uintptr_t offset);

        private:
            GameBase() = delete; // static-only class

        private:
            static uintptr_t s_base;
            static bool s_initialized;
        };

    } // namespace SDK
} // namespace GTASA
