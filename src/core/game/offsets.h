#pragma once
#include <cstdint>

namespace GTASA
{
    namespace SDK
    {
        // Structure that holds all dynamic offsets (which change based on the version)
        struct GameOffsets
        {
            uintptr_t LocalPlayer;
            uintptr_t sendHelpMessageFunc;
            uintptr_t IdleLoop;
            uintptr_t OnWanted;
            uintptr_t OnCrimeCommited;
            uintptr_t SetResolution;
        };

        namespace Offsets
        {
            // This is a sample global of offsets available throughout the project with the keyword Offsets::Dynamic
            inline GameOffsets Dynamic = {};

            namespace PlayerInfo
            {
                constexpr uintptr_t Ped = 0x00;
                constexpr uintptr_t Money = 0xB8;
                constexpr uintptr_t Chaos = 0x4;
                constexpr uintptr_t WantedLevel = 0x2C;
            } // namespace PlayerInfo

            namespace CPed
            {
                constexpr uintptr_t Health = 0x540;
                constexpr uintptr_t Armor = 0x548;
                constexpr uintptr_t Matrix = 0x14;
            } // namespace CPed

            namespace Matrix
            {
                constexpr uintptr_t pos_x = 0x30;
                constexpr uintptr_t pos_y = 0x34;
                constexpr uintptr_t pos_z = 0x38;
            } // namespace Matrix
        } // namespace Offsets

    } // namespace SDK
} // namespace GTASA