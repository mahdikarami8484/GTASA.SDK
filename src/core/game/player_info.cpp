#include "player_info.h"

#include "core/logging/logger.hpp"

using namespace GTASA::SDK;

std::unique_ptr<PlayerInfo> PlayerInfo::getLocal()
{
    uintptr_t playerInfoAddr = GameBase::address(Offsets::Globals::LocalPlayer);

    if (!playerInfoAddr) return nullptr;

    return std::make_unique<PlayerInfo>(playerInfoAddr);
}

std::unique_ptr<Ped> PlayerInfo::getPed() const
{
    uintptr_t pedAddr = *reinterpret_cast<uintptr_t*>(m_ptr + Offsets::PlayerInfo::Ped);
    if (!pedAddr) return std::make_unique<Ped>(0);
    return std::make_unique<Ped>(pedAddr);
}

int PlayerInfo::getMoney() const
{
    uintptr_t moneyAddress = m_ptr + Offsets::PlayerInfo::Money;
    if (!moneyAddress) return 0;
    return *reinterpret_cast<int*>(moneyAddress);
}

void PlayerInfo::setMoney(int amount)
{
    uintptr_t moneyAddress = m_ptr + Offsets::PlayerInfo::Money;
    if (!moneyAddress) return;
    *reinterpret_cast<int*>(moneyAddress) = amount;
}

uintptr_t GTASA::SDK::PlayerInfo::getChaosAddr() const
{
    uintptr_t chaos_ptr = m_ptr + Offsets::PlayerInfo::Chaos;

    if (!chaos_ptr) return 0;

    return *reinterpret_cast<uintptr_t*>(chaos_ptr);
}

int GTASA::SDK::PlayerInfo::getChaos() const
{
    uintptr_t chaos = getChaosAddr();

    if (!chaos) return -1;

    return *reinterpret_cast<int*>(chaos);
}

void GTASA::SDK::PlayerInfo::setChaos(int amount)
{
    uintptr_t chaos = getChaosAddr();

    if (!chaos) return;

    *reinterpret_cast<int*>(chaos) = amount;
}

int GTASA::SDK::PlayerInfo::getWantedLevel() const
{
    uintptr_t wantedLevel = getChaosAddr() + Offsets::PlayerInfo::WantedLevel;

    if (!wantedLevel) return -1;

    return *reinterpret_cast<int*>(wantedLevel);
}

void GTASA::SDK::PlayerInfo::setWantedLevel(uint8_t amount, bool chaos)
{
    uintptr_t wantedLevel = getChaosAddr() + Offsets::PlayerInfo::WantedLevel;

    if (!wantedLevel) return;

    *reinterpret_cast<int*>(wantedLevel) = amount;

    int chaosValues[] = {0, 50, 180, 550, 1200, 2400, 4600};

    if (amount > 6) return;

    setChaos(chaosValues[amount]);
}
