#include "hud.h"
#include "core/logging/logger.h"

using namespace GTASA::SDK;

void HUD::sendHelpMessage(const std::string& msg)
{
    if (!Internal::Functions::CHUD::sendHelpMessage)
    {
        Internal::Functions::CHUD::sendHelpMessage =
            reinterpret_cast<Internal::Functions::CHUD::sendHelpMessage_t>(
                GameBase::address(Offsets::CHud::sendHelpMessageFunc));

        if (!Internal::Functions::CHUD::sendHelpMessage)
        {
            LOG_ERROR("[HUD] Failed to resolve sendHelpMessage function.");
            return;
        }

        LOG_DEBUG("[HUD] sendHelpMessage function resolved.");
    }

    // Changed formatting to %s and converted std::string using msg.c_str() to prevent runtime crashes
    LOG_DEBUG("[HUD] Sending help message: %s", msg.c_str());

    Internal::Functions::CHUD::sendHelpMessage(msg.c_str(), false, false, false);
}
