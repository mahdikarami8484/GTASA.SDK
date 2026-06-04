#pragma once

#include <windows.h>

#include <detours.h>

#include <vector>
#pragma comment(lib, "detours.lib")

struct HookEntry
{
    void** target;
    void* detour;
};

class HookManager
{
public:
    static HookManager& instance();

    void addHook(void** target, void* detour);
    void enableHooks();
    void disableHooks();

    HookManager(const HookManager&) = delete;
    HookManager& operator=(const HookManager&) = delete;

private:
    HookManager() = default;
    ~HookManager() = default;

private:
    std::vector<HookEntry> hooks;
};