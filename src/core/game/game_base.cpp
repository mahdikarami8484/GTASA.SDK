#include "core/game/game_base.h"
#include <Windows.h>
#include <stdexcept>

namespace GTASA {
    namespace SDK {

        uintptr_t GameBase::s_base = 0;
        bool GameBase::s_initialized = false;

        void GameBase::initialize()
        {
            if (s_initialized)
                return;

            // Obtain the base address of gta_sa.exe
            HMODULE hModule = GetModuleHandleA(nullptr);
            if (hModule == nullptr) {
                throw std::runtime_error("GameBase::initialize() failed: unable to obtain module handle");
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
            if (!s_initialized || s_base == 0) {
                throw std::runtime_error("GameBase::address() called before initialize()");
            }
            return s_base + offset;
        }

    } // namespace SDK
} // namespace GTASA
