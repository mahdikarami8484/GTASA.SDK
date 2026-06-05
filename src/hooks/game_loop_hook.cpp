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
void GameLoopHook::install()
{
    // Offset: 0x619b60 - 0x400000 = 0x219b60
    oProcessFrame = reinterpret_cast<tProcessFrame>(GameBase::address(0x00219b60));

    HookManager::instance().addHook(reinterpret_cast<void**>(&oProcessFrame),
                                    reinterpret_cast<void*>(hk_ProcessFrame));

    LOG_INFO("[GameLoopHook] Game processor hooked!");

    // 0069db70 - 0x400000 = 29DB70
    o_strcpy = reinterpret_cast<t_strcpy>(GameBase::address(0x29DB70));

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_strcpy),
                                    reinterpret_cast<void*>(hk_strcpy));

    LOG_INFO("[GameLoopHook] strcpy hooked!");
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
