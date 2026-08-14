#include "hook_manager.h"
#include <core/logging/logger.h> 

HookManager& HookManager::instance()
{
    static HookManager instance;
    return instance;
}

void HookManager::addHook(void** target, void* detour)
{
    this->hooks.push_back({target, detour});
    // Changed format specifier from %d to %zu for size_t validation
    LOG_DEBUG("[HookManager] Hook registered. Total hooks: %zu", hooks.size());
}

void HookManager::enableHooks()
{
    // Changed format specifier from %d to %zu for size_t validation
    LOG_INFO("[HookManager] Enabling %zu hooks...", hooks.size());

    LONG result = DetourTransactionBegin();
    if (result != NO_ERROR)
    {
        LOG_ERROR("[HookManager] Failed to begin detour transaction. Error: %d", result);
        return;
    }

    DetourUpdateThread(GetCurrentThread());

    size_t attachedCount = 0;

    for (auto& hook : hooks)
    {
        result = DetourAttach(hook.target, hook.detour);

        if (result == NO_ERROR)
        {
            attachedCount++;
        }
        else
        {
            LOG_ERROR("[HookManager] Failed to attach hook. Error: %d", result);
        }
    }

    result = DetourTransactionCommit();

    if (result != NO_ERROR)
    {
        LOG_ERROR("[HookManager] Failed to commit hook transaction. Error: %d", result);
        return;
    }

    // Changed format specifiers from %d to %zu for both attachedCount and hooks.size()
    LOG_INFO("[HookManager] Hooks enabled: %zu/%zu.", attachedCount, hooks.size());
}

void HookManager::disableHooks()
{
    // Changed format specifier from %d to %zu for size_t validation
    LOG_INFO("[HookManager] Disabling %zu hooks...", hooks.size());

    LONG result = DetourTransactionBegin();
    if (result != NO_ERROR)
    {
        LOG_ERROR("[HookManager] Failed to begin detach transaction. Error: %d", result);
        return;
    }

    DetourUpdateThread(GetCurrentThread());

    size_t detachedCount = 0;

    for (auto& hook : hooks)
    {
        result = DetourDetach(hook.target, hook.detour);

        if (result == NO_ERROR)
        {
            detachedCount++;
        }
        else
        {
            LOG_ERROR("[HookManager] Failed to detach hook. Error: %d", result);
        }
    }

    result = DetourTransactionCommit();

    if (result != NO_ERROR)
    {
        LOG_ERROR("[HookManager] Failed to commit detach transaction. Error: %d", result);
        return;
    }

    // Changed format specifiers from %d to %zu for both detachedCount and hooks.size()
    LOG_INFO("[HookManager] Hooks disabled: %zu/%zu.", detachedCount, hooks.size());
}
