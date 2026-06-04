#pragma once

#include <memory>

#include "core/math/vector3.h"
#include "offsets.h"

namespace GTASA
{
    namespace SDK
    {
        class Matrix
        {
        private:
            uintptr_t m_address;

        public:
            explicit Matrix(uintptr_t address) : m_address(address) {}

            Vector3 getPosition() const;
            void setPosition(Vector3 pos);

            Vector3 getForward() const;
            Vector3 getRight() const;
            Vector3 getUp() const;
        };
    } // namespace SDK
} // namespace GTASA