#include "player_hook.h"

using namespace GTASA::SDK;

namespace
{

    // GTA function pointers

    using t_OnWanted = void(__fastcall*)(void* this_ptr, int* param_1);
    t_OnWanted o_OnWanted = nullptr;

    void __fastcall hk_OnWanted(void* this_ptr, int* param_1)
    {
        auto crimeEvent = std::make_shared<Events::CrimeCommittedEvent>(param_1, nullptr, nullptr);

        // EventBus::instance().dispatch(crimeEvent);
        // o_OnCrimeCommitted(this_ptr, param_1);

        if (o_OnWanted)
        {
            o_OnWanted(this_ptr, param_1);
        }
    }

    using t_OnCrimeCommited = void(__thiscall*)(void* this_ptr, void* param_1, void* param_2,
                                                void* param_3);
    t_OnCrimeCommited o_OnCrimeCommited = nullptr;

    void __thiscall hk_OnCrimeCommited(void* this_ptr, void* param_1, void* param_2, void* param_3)
    {
        if (o_OnCrimeCommited)
        {
            o_OnCrimeCommited(this_ptr, param_1, param_2, param_3);
        }
    }

} // anonymous namespace

// IHook implementation
bool PlayerHook::install()
{
    LOG_INFO("[PlayerHook] Installing player hooks...");

    // gta_sa.exe + 0x161C90
    o_OnWanted = reinterpret_cast<t_OnWanted>(GameBase::address(0x161C90));

    if (!o_OnWanted)
    {
        LOG_ERROR("[PlayerHook] Failed to resolve OnWanted address.");
        return false;
    }

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_OnWanted),
                                    reinterpret_cast<void*>(hk_OnWanted));

    LOG_DEBUG("[PlayerHook] OnWanted resolved at: %p", reinterpret_cast<void*>(o_OnWanted));

    // gta_sa.exe + 0x162120
    o_OnCrimeCommited = reinterpret_cast<t_OnCrimeCommited>(GameBase::address(0x162120));

    if (!o_OnCrimeCommited)
    {
        LOG_ERROR("[PlayerHook] Failed to resolve OnCrimeCommited address.");
        return false;
    }

    // Uncommited the hook registration line to ensure OnCrimeCommited is actively hooked into the runtime
    HookManager::instance().addHook(reinterpret_cast<void**>(&o_OnCrimeCommited),
                                    reinterpret_cast<void*>(hk_OnCrimeCommited));

    LOG_DEBUG("[PlayerHook] OnCrimeCommited resolved at: %p",
              reinterpret_cast<void*>(o_OnCrimeCommited));

    LOG_INFO("[PlayerHook] Player hooks installed successfully.");
    return true;
}

bool PlayerHook::uninstall()
{
    // HookManager handles hook removal.
    LOG_DEBUG("[PlayerHook] Uninstall requested.");
    return true;
}

// Auto register
namespace
{
    AutoHook<PlayerHook> _autoPlayerHook;
}
