#pragma once

#include <string>

#include "game_base.h"
#include "game_functions.h"
#include "offsets.h"

namespace GTASA
{
    namespace SDK
    {

        class HUD
        {
        public:
            static void sendHelpMessage(const std::string& msg);
        };
    } // namespace SDK
} // namespace GTASA
