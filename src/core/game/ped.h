#pragma once

#include "entity.h"
#include "matrix.h"

namespace GTASA
{
    namespace SDK
    {
        class Ped : public Entity
        {
        public:
            using Entity::Entity;
            std::unique_ptr<Matrix> getMatrix() const;

            float getHealth() const;
            void setHealth(float amount);

            float getArmor() const;
            void setArmor(float amount);
        };

    } // namespace SDK
} // namespace GTASA