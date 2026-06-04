#pragma once

#include <string>

#include "core/event/event_bus.h"
#include "core/game/hud.h"
#include "core/game/player_info.h"
#include "core/logging/logger.hpp"
#include "core/script/auto_register_script.h"
#include "core/script/script.h"
#include "hooks/game_loop_hook.h"
#include "hooks/player_hook.h"

using namespace Logging;

namespace GTASA
{
    namespace SDK
    {

        class SampleScript : public Script
        {

        public:
            SampleScript() {}

            virtual void onEvent(std::shared_ptr<BaseEvent> event) override
            {
                if (const auto* initializeEvent =
                        dynamic_cast<const Events::InitializeEvent*>(event.get()))
                    LOG_INFO("[SampleScript] initialized.");

                if (const auto* processFrameEvent =
                        dynamic_cast<const Events::ProcessFrameEvent*>(event.get()))
                {
                    if ((GetAsyncKeyState(VK_F5) & 1))
                    {
                        std::unique_ptr<PlayerInfo> playerInfo = PlayerInfo::getLocal();
                        if (!playerInfo) return;
                        playerInfo->getPed()->setHealth(1000.0f);
                        playerInfo->getPed()->setArmor(1000.0f);
                        float hp = playerInfo->getPed()->getHealth();
                        float arm = playerInfo->getPed()->getArmor();
                        LOG_INFO("Player Health: %.2f", hp);
                        LOG_INFO("Player Armor: %.2f", arm);

                        Vector3 pos = playerInfo->getPed()->getMatrix()->getPosition();
                        LOG_INFO("Player pos_x: %.2f, pos_y: %.2f, pos_z:%.2f", pos.x, pos.y,
                                 pos.z);

                        playerInfo->getPed()->getMatrix()->setPosition(
                            {pos.x, pos.y, pos.z + 5.0f});

                        playerInfo->setMoney(200);
                        int wantedLevel = playerInfo->getWantedLevel();
                        // playerInfo->setWantedLevel(wantedLevel > 0 ? 0 : 6);

                        int chaos = playerInfo->getChaos();
                        wantedLevel = playerInfo->getWantedLevel();
                        LOG_INFO("chaos:%d, wantedLevel: %d", chaos, wantedLevel);
                        HUD::sendHelpMessage("Hello GTA, I'm Mahdi...!");
                    }
                }

                if (const auto* crimeEvent =
                        dynamic_cast<const Events::CrimeCommittedEvent*>(event.get()))
                {
                    LOG_INFO("[Sample Script] crime event...");
                }
            }
        };

    } // namespace SDK
} // namespace GTASA

namespace
{
    GTASA::SDK::AutoRegisterScript<GTASA::SDK::SampleScript> _autoReg;
}

/*


                switch (type)
                {
                case GTASA::SDK::EventType::GameProcess:
                {
                    if (GetAsyncKeyState(VK_F5) & 1)
                    {
                        std::unique_ptr<Player> player = Player::getLocal();
                        if(!player || !player->isValid())
                            break;
                        player->setHealth(1000.0f);
                        player->setArmor(1000.0f);
                        float hp = player->getHealth();
                        float arm = player->getArmor();
                        LOG_INFO("Player Health: %.2f", hp);
                        LOG_INFO("Player Armor: %.2f", arm);

                        Vector3 pos = player->getMatrix()->getPosition();
                        LOG_INFO("Player pos_x: %.2f, pos_y: %.2f, pos_z:%.2f",
                        pos.x, pos.y, pos.z);

                        player->getMatrix()->setPosition({pos.x, pos.y, pos.z + 5.0f});
                    }

                    break;
                }


                case GTASA::SDK::EventType::OnCrimeCommitted:
                    LOG_INFO("[Sample Script] player OnCrimeCommitted.!");
                    break;

                case GTASA::SDK::EventType::Initialize:
                    LOG_INFO("Sample Script initialized.");
                    break;
                case GTASA::SDK::EventType::Shutdown:
                    break;
                default:
                    break;
                }

*/