#include <Windows.h>

#include "core/game/game_base.h"

namespace GTASA
{
    namespace SDK
    {

        uintptr_t GameBase::s_base = 0;
        bool GameBase::s_initialized = false;

        void GameBase::initialize()
        {
            if (s_initialized) return;

            // base address of gta_sa.exe
            HMODULE hModule = GetModuleHandleA(nullptr);
            if (hModule == nullptr)
            {
                // Critical initialization error: failed to get module handle
                // Set error state and return - address() will return 0 for safety
                s_base = 0;
                s_initialized = false;
                return;
            }
            s_base = reinterpret_cast<uintptr_t>(hModule);
            s_initialized = true;
        }

        uintptr_t GameBase::base()
        {
            return s_base;
        }

        uintptr_t GameBase::address(uintptr_t offset)
        {
            if (!s_initialized || s_base == 0)
            {
                // Base not initialized - return 0 to prevent invalid memory access
                // Callers should check for 0 return value
                return 0;
            }
            return s_base + offset;
        }

    } // namespace SDK
} // namespace GTASA
