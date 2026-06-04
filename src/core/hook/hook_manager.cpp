#include "hook_manager.h"

#include <core/logging/logger.hpp>

using namespace Logging;

HookManager& HookManager::instance()
{
    static HookManager instance;
    return instance;
}

void HookManager::addHook(void** target, void* detour)
{
    this->hooks.push_back({target, detour});
}

void HookManager::enableHooks()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (auto& hook : hooks)
    {
        DetourAttach(hook.target, hook.detour);
    }

    DetourTransactionCommit();
    LOG_INFO("[HookManager] hooks enabled.");
}

void HookManager::disableHooks()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    for (auto& hook : hooks)
    {
        DetourDetach(hook.target, hook.detour);
    }

    DetourTransactionCommit();
}
