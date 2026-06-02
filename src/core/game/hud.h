#pragma once

#include "game_base.h"
#include "game_functions.h"
#include "offsets.h"

#include <string>

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
