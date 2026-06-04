#include "matrix.h"

using namespace GTASA::SDK;

Vector3 Matrix::getPosition() const
{
    if (!m_address) return Vector3(0, 0, 0);
    return {*reinterpret_cast<float*>(m_address + Offsets::Matrix::pos_x),
            *reinterpret_cast<float*>(m_address + Offsets::Matrix::pos_y),
            *reinterpret_cast<float*>(m_address + Offsets::Matrix::pos_z)};
}

void Matrix::setPosition(Vector3 pos)
{
    if (!m_address) return;
    *reinterpret_cast<float*>(m_address + Offsets::Matrix::pos_x) = pos.x;
    *reinterpret_cast<float*>(m_address + Offsets::Matrix::pos_y) = pos.y;
    *reinterpret_cast<float*>(m_address + Offsets::Matrix::pos_z) = pos.z;
}
