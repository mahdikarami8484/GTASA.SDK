#include "sdk_runtime.h"

#include "core/game/game_base.h"
#include "logging/logger.hpp"

using namespace GTASA::SDK;

SDKRuntime& SDKRuntime::instance()
{
    static SDKRuntime instance;
    return instance;
}

SDKRuntime::SDKRuntime() : m_initialized(false)
{
    return;
}

SDKRuntime::~SDKRuntime()
{
    if (m_initialized)
    {
        shutdown();
    }
}

void SDKRuntime::init()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) return;

    Logging::Logger::Instance().Start();

    LOG_INFO("[SDKRuntime] Initializing SDK Runtime...");

    GameBase::initialize();

    LOG_INFO("[SDKRuntime] Initializing scripts...");
    auto initializeEvent = std::make_unique<Events::InitializeEvent>();
    EventBus::instance().dispatch(std::move(initializeEvent));
    LOG_INFO("[SDKRuntime] Scripts initialized successfully.");

    LOG_INFO("[SDKRuntime] Installing hooks...");
    HookRegistry::instance().sort();
    for (auto& hook : HookRegistry::instance().getAll())
        if (hook->isEnabled()) hook->install();

    LOG_INFO("[SDKRuntime] Hooks installed successfully.");

    HookManager::instance().enableHooks();

    m_initialized = true;
    LOG_INFO("[SDKRuntime] SDK Runtime initialized successfully.");
}

void SDKRuntime::shutdown()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_initialized) return;

        LOG_INFO("[SDKRuntime] Shutting down SDK Runtime...");

        // Disable hooks first
        HookManager::instance().disableHooks();

        // Dispatch shutdown event
        auto shutdownEvent = std::make_unique<Events::ShutdownEvent>();
        EventBus::instance().dispatch(std::move(shutdownEvent));

        m_initialized = false;
    }

    // Stop logger after releasing the lock to avoid any potential issues
    Logging::Logger::Instance().Stop();
}
