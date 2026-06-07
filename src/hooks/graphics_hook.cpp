#include "graphics_hook.h"

#include "core/event/event_bus.h"
#include "core/game/offsets.h"

using namespace GTASA::SDK;

namespace
{
    using t_SetResolution = void(__cdecl*)(int* param_1, unsigned int param_2,
                                           unsigned int param_3);
    t_SetResolution o_SetResolution = nullptr;

    void __cdecl hk_SetResolution(int* param_1, unsigned int param_2, unsigned int param_3)
    {
        auto resEvent = std::make_shared<Events::SetResolutionEvent>(param_1, param_2, param_3);
        EventBus::instance().dispatch(resEvent);

        if (o_SetResolution)
        {
            o_SetResolution(resEvent->resolutionData, resEvent->param_2, resEvent->param_3);
        }
    }

} // anonymous namespace

void GraphicsHook::install()
{
    o_SetResolution =
        reinterpret_cast<t_SetResolution>(GameBase::address(Offsets::Dynamic.SetResolution));

    HookManager::instance().addHook(reinterpret_cast<void**>(&o_SetResolution),
                                    reinterpret_cast<void*>(hk_SetResolution));

    LOG_INFO("[GraphicsHook] Native SetResolution hooked successfully!");
}

void GraphicsHook::uninstall() {}
namespace
{
    AutoHook<GraphicsHook> _autoGraphicsHook;
}