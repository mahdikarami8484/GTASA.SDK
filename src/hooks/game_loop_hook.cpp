#include "game_loop_hook.h"

#include "core/game/game_functions.h"

using namespace GTASA::SDK;
using namespace GTASA::SDK::Internal::Functions::System;

namespace
{

    int __cdecl hk_ProcessFrame(int command, int param)
    {
        if (command == 0x1a)
        {
            auto processFrameEvent = std::make_shared<Events::ProcessFrameEvent>(command, param);

            EventBus::instance().dispatch(processFrameEvent);

            return oProcessFrame(processFrameEvent->command, processFrameEvent->param);
        }

        return oProcessFrame(command, param);
    }

    void __cdecl hk_strcpy(int destination, int source, unsigned short param_3)
    {
        o_strcpy(destination, source, param_3);
    }

    void __stdcall hk_ScreenLoad(void* param_1, LPCSTR param_2)
    {
        return oScreenLoad(param_1, param_2);
    }

} // anonymous namespace

// --------------------------------------------------
// IHook implementation
// --------------------------------------------------

bool GameLoopHook::install()
{
    LOG_INFO("[GameLoopHook] Installing game loop hooks...");

    // 0x619b60 - 0x400000 = 0x219b60
    oProcessFrame = reinterpret_cast<tProcessFrame>(GameBase::address(0x00219b60));

    if (!oProcessFrame)
    {
        LOG_ERROR("[GameLoopHook] Failed to resolve ProcessFrame address.");
        return false;
    }

    HookManager::instance().addHook(reinterpret_cast<void**>(&oProcessFrame),
                                    reinterpret_cast<void*>(hk_ProcessFrame));

    LOG_DEBUG("[GameLoopHook] ProcessFrame resolved at: {}",
              reinterpret_cast<void*>(oProcessFrame));

    // 0x69db70 - 0x400000 = 0x29db70
    o_strcpy = reinterpret_cast<t_strcpy>(GameBase::address(0x0029DB70));

    if (!o_strcpy)
    {
        LOG_ERROR("[GameLoopHook] Failed to resolve strcpy address.");
        return false;
    }

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_strcpy),
                                    reinterpret_cast<void*>(hk_strcpy));

    LOG_DEBUG("[GameLoopHook] strcpy resolved at: {}", reinterpret_cast<void*>(o_strcpy));

    LOG_INFO("[GameLoopHook] Game loop hooks installed successfully.");

    return true;
}

bool GameLoopHook::uninstall()
{
    // HookManager handles hook removal.

    LOG_DEBUG("[GameLoopHook] Uninstall requested.");

    return true;
}

// --------------------------------------------------
// Auto register
// --------------------------------------------------

namespace
{
    AutoHook<GameLoopHook> _autoGameLoopHook;
}