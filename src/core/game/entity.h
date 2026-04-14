#pragma once

#include <memory>
#include "game_base.h"
#include "offsets.h"

namespace GTASA {
    namespace SDK {

        class Entity {
            protected:
            uintptr_t m_ptr;

            public:
            explicit Entity(uintptr_t ptr) : m_ptr(ptr) {}

            bool isValid() const { return (m_ptr != 0); }
        };

    }
}