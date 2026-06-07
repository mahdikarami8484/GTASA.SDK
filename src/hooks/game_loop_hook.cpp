#include "game_loop_hook.h"

#include "core/game/game_functions.h"
#include "core/game/offsets.h"
#include "core/game/version_manager.h"

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

    void __cdecl hk_Idle()
    {
        auto processFrameEvent = std::make_shared<Events::ProcessFrameEvent>(0x18, 0);
        EventBus::instance().dispatch(processFrameEvent);
        return o_Idle();
    }

} // anonymous namespace

// --------------------------------------------------
// IHook implementation
// --------------------------------------------------
void GameLoopHook::install()
{
    GameVersion currentVer = VersionManager::instance().getVersion();

    if (currentVer == GameVersion::V1_0_US_HOODLUM)
    {
        oProcessFrame =
            reinterpret_cast<tProcessFrame>(GameBase::address(Offsets::Dynamic.IdleLoop));
        HookManager::instance().addHook(reinterpret_cast<void**>(&oProcessFrame),
                                        reinterpret_cast<void*>(hk_ProcessFrame));
        LOG_INFO("[GameLoopHook] Hooked ProcessFrame for v1.0!");
    }
    else if (currentVer == GameVersion::RGL_1_0_0_22)
    {
        o_Idle = reinterpret_cast<t_Idle>(GameBase::address(Offsets::Dynamic.IdleLoop));
        HookManager::instance().addHook(reinterpret_cast<void**>(&o_Idle),
                                        reinterpret_cast<void*>(hk_Idle));
        LOG_INFO("[GameLoopHook] Hooked Idle for RGL!");
    }

    // todo: What is this for?
    /*
    // 0069db70 - 0x400000 = 29DB70
    o_strcpy = reinterpret_cast<t_strcpy>(
        GameBase::address(0x29DB70)
        );

    HookManager::instance().addHook(
        reinterpret_cast<void**>(&o_strcpy),
        reinterpret_cast<void*>(hk_strcpy)
    );

    LOG_INFO("[GameLoopHook] strcpy hooked!");
*/
}

void GameLoopHook::uninstall()
{
    // handled by HookManager
}

// --------------------------------------------------
// Auto register
// --------------------------------------------------
namespace
{
    AutoHook<GameLoopHook> _autoGameLoopHook;
}
