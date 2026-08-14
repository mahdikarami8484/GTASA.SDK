#include "event_bus.h"

using namespace GTASA::SDK;

EventBus& EventBus::instance()
{
    static EventBus instance;
    return instance;
}

void EventBus::add(std::unique_ptr<Script> script)
{
    if (!script)
    {
        LOG_WARNING("[EventBus] Tried to register null script.");
        return;
    }

    LOG_INFO("[EventBus] Registering script: %s", script->name());
    m_scripts.emplace_back(std::move(script));
}

void EventBus::dispatch(std::shared_ptr<BaseEvent> event)
{
    if (!event)
    {
        LOG_WARNING("[EventBus] Tried to dispatch null event.");
        return;
    }

    for (auto& script : m_scripts)
    {
        if (script && script->isEnabled())
        {
            script->onEvent(event);
        }
    }

    LOG_DEBUG("[EventBus] Dispatched event: %s", event->getName());
}
