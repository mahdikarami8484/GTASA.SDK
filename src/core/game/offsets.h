#pragma once
#include <cstdint>

namespace GTASA
{
    namespace SDK
    {

        namespace Offsets
        {
            namespace Globals
            {
                constexpr uintptr_t LocalPlayer = 0x77CD98;
            }

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

            namespace CHud
            {
                constexpr uintptr_t sendHelpMessageFunc = 0x188BE0;
            }
        } // namespace Offsets

    } // namespace SDK
} // namespace GTASA