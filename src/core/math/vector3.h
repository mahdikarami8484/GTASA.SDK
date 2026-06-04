#pragma once

#include <cmath>

namespace GTASA::SDK
{

    struct Vector3
    {
        float x{};
        float y{};
        float z{};

        Vector3() = default;

        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        float length() const
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        Vector3 normalized() const
        {
            float len = length();
            if (len == 0.0f) return {};
            return {x / len, y / len, z / len};
        }

        Vector3 operator+(const Vector3& other) const
        {
            return {x + other.x, y + other.y, z + other.z};
        }

        Vector3 operator-(const Vector3& other) const
        {
            return {x - other.x, y - other.y, z - other.z};
        }

        Vector3 operator*(float scalar) const
        {
            return {x * scalar, y * scalar, z * scalar};
        }
    };

} // namespace GTASA::SDK
