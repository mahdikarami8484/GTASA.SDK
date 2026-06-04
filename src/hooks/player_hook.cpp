#include "player_hook.h"

using namespace GTASA::SDK;
using namespace Logging;

namespace
{

    // --------------------------------------------------
    // GTA function pointer
    // --------------------------------------------------
    using t_OnWanted = void(__fastcall*)(void* this_ptr, int* param_1);
    t_OnWanted o_OnWanted = nullptr;

    void __fastcall hk_OnWanted(void* this_ptr, int* param_1)
    {
        auto crimeEvent = std::make_shared<Events::CrimeCommittedEvent>(param_1, nullptr, nullptr);
        // EventBus::instance().dispatch(crimeEvent);
        //  o_OnCrimeCommitted(this_ptr, param_1);
    }

    using t_OnCrimeCommited = void(__thiscall*)(void* this_ptr, void* param_1, void* param_2,
                                                void* param_3);
    t_OnCrimeCommited o_OnCrimeCommited = nullptr;

    void __fastcall hk_OnCrimeCommited(void* this_ptr, void* param_1, void* param_2, void* param_3)
    {
        // auto crimeEvent = std::make_shared<Events::CrimeCommittedEvent>(param_1, nullptr,
        // nullptr); EventBus::instance().dispatch(crimeEvent);
        return o_OnCrimeCommited(this_ptr, param_1, param_2, param_3);
    }
} // anonymous namespace

// --------------------------------------------------
// IHook implementation
// --------------------------------------------------
void PlayerHook::install()
{
    // Offset: 0x619b60 - 0x400000 = 0x219b60
    // gta_sa.exe+0x16279B
    // gta_sa.exe+162760

    // gta_sa.exe+20EA93
    // esi + 480

    // gta_sa.exe+162120

    o_OnWanted = reinterpret_cast<t_OnWanted>(GameBase::address(0x161C90));

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_OnWanted),
                                    reinterpret_cast<void*>(hk_OnWanted));

    LOG_INFO("[PlayerHook] Player OnWanted hooked!");

    o_OnCrimeCommited = reinterpret_cast<t_OnCrimeCommited>(GameBase::address(0x162120));

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

// gta_sa.exe+29DBB0
