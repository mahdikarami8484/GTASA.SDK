#include "ped.h"

using namespace GTASA::SDK;

std::unique_ptr<Matrix> Ped::getMatrix() const
{
    if (!isValid()) return std::make_unique<Matrix>(0);

    return std::make_unique<Matrix>(*reinterpret_cast<uintptr_t*>((m_ptr) + Offsets::CPed::Matrix));
}

float Ped::getHealth() const
{
    if (!isValid()) return 0.0f;

    return *reinterpret_cast<float*>((m_ptr) + Offsets::CPed::Health);
}

void Ped::setHealth(float amount)
{
    if (!isValid()) return;

    *reinterpret_cast<float*>((m_ptr) + Offsets::CPed::Health) = amount;
}

float Ped::getArmor() const
{
    if (!isValid()) return 0.0f;

    return *reinterpret_cast<float*>((m_ptr) + Offsets::CPed::Armor);
}

void Ped::setArmor(float amount)
{
    if (!isValid()) return;

    *reinterpret_cast<float*>((m_ptr) + Offsets::CPed::Armor) = amount;
}
