#pragma once
#include "i_hook.h"
#include "memory"
#include "vector"

#include <algorithm>

namespace GTASA
{
    namespace SDK
    {
        class HookRegistry
        {
        public:
            static HookRegistry& instance();

            void add(std::unique_ptr<IHook> hook);

            const std::vector<std::unique_ptr<IHook>>& getAll() const;

            void sort();

        private:
            HookRegistry() = default;

        private:
            std::vector<std::unique_ptr<IHook>> m_hooks;
        };
    } // namespace SDK
} // namespace GTASA
