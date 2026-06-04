#pragma once

#include "memory"
#include "ped.h"

namespace GTASA
{
    namespace SDK
    {
        class PlayerInfo
        {
        private:
            uintptr_t m_ptr;

        public:
            explicit PlayerInfo(uintptr_t ptr) : m_ptr(ptr) {}

            static std::unique_ptr<PlayerInfo> getLocal();

            std::unique_ptr<Ped> getPed() const;

            int getMoney() const;
            void setMoney(int amount);

            int getChaos() const;
            void setChaos(int amount);

            int getWantedLevel() const;
            void setWantedLevel(uint8_t amount, bool choas = true);

        private:
            uintptr_t getChaosAddr() const;
        };
    } // namespace SDK
} // namespace GTASA