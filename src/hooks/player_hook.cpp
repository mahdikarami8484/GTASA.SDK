#include "player_hook.h"

#include "core/game/offsets.h"

using namespace GTASA::SDK;

namespace
{

    // --------------------------------------------------
    // GTA function pointer
    // --------------------------------------------------
    using t_OnWanted = void(__fastcall*)(void* this_ptr, void* edx_dummy);
    t_OnWanted o_OnWanted = nullptr;

    void __fastcall hk_OnWanted(void* this_ptr, void* edx_dummy)
    {
        o_OnWanted(this_ptr, edx_dummy);
    }

    using t_OnCrimeCommited = void(__thiscall*)(void* this_ptr, void* param_1, void* param_2,
                                                void* param_3);
    t_OnCrimeCommited o_OnCrimeCommited = nullptr;

    void __fastcall hk_OnCrimeCommited(void* this_ptr, void* edx_dummy, void* param_1,
                                       void* param_2, void* param_3)
    {

        auto crimeEvent = std::make_shared<Events::CrimeCommittedEvent>(
            reinterpret_cast<int*>(param_1), param_2, param_3);
        EventBus::instance().dispatch(crimeEvent);
        return o_OnCrimeCommited(this_ptr, param_1, param_2, param_3);
    }
} // anonymous namespace

// --------------------------------------------------
// IHook implementation
// --------------------------------------------------
void PlayerHook::install()
{
    o_OnWanted = reinterpret_cast<t_OnWanted>(GameBase::address(Offsets::Dynamic.OnWanted));

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_OnWanted),
                                    reinterpret_cast<void*>(hk_OnWanted));

    LOG_INFO("[PlayerHook] Player OnWanted hooked!");

    o_OnCrimeCommited =
        reinterpret_cast<t_OnCrimeCommited>(GameBase::address(Offsets::Dynamic.OnCrimeCommited));

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_OnCrimeCommited),
                                    reinterpret_cast<void*>(hk_OnCrimeCommited));

    LOG_INFO("[PlayerHook] Player OnCrimeCommited hooked!");
}

void PlayerHook::uninstall()
{
    // handled by HookManager
}

// --------------------------------------------------
// Auto register
// --------------------------------------------------
namespace
{
    AutoHook<PlayerHook> _autoPlayerHook;
}