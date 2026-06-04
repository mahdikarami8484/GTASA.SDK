#include "hook_registry.h"

using namespace GTASA::SDK;

HookRegistry& HookRegistry::instance()
{
    static HookRegistry instance;
    return instance;
}

void HookRegistry::add(std::unique_ptr<IHook> hook)
{
    m_hooks.push_back(std::move(hook));
}

const std::vector<std::unique_ptr<IHook>>& GTASA::SDK::HookRegistry::getAll() const
{
    return m_hooks;
}

void GTASA::SDK::HookRegistry::sort()
{
    std::sort(m_hooks.begin(), m_hooks.end(),
              [](const std::unique_ptr<IHook>& a, const std::unique_ptr<IHook>& b)
              { return a->Priority() < b->Priority(); });
}
