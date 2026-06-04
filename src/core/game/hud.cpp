#include "hud.h"

#include "core/logging/logger.hpp"

using namespace GTASA::SDK;

void HUD::sendHelpMessage(const std::string& msg)
{
    Internal::Functions::CHUD::sendHelpMessage =
        reinterpret_cast<Internal::Functions::CHUD::sendHelpMessage_t>(
            GameBase::address(Offsets::CHud::sendHelpMessageFunc));
    using namespace Logging;
    LOG_INFO("send message: %s", msg.c_str());
    if (Internal::Functions::CHUD::sendHelpMessage)
    {
        Internal::Functions::CHUD::sendHelpMessage(msg.c_str(), false, false, false);
        LOG_INFO("sended message: %s", msg.c_str());
    }
}
