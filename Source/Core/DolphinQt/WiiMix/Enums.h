#pragma once

#include "DolphinQt/WiiMix/Objective.h"
#include "UICommon/GameFile.h"

namespace WiiMixEnums {

    enum class Difficulty {
        NORMAL,
        HARD,
        WIISANITY,
        END // Default/size value
    };

    enum class Mode {
        BINGO, // Sub-modes: lockout, capture the flag
        SHUFFLE, // Sub-modes: race
        ROGUE,
        END, // Default/size value
    };

    enum class SaveStateBank {
        USER,
        VERIFIED,
        UNVERIFIED,
        END, // Default/size value
    };
}

#define MIN_NUM_OBJECTIVES 1
#define MAX_NUM_OBJECTIVES 1000
constexpr WiiMixEnums::Difficulty DEFAULT_DIFFICULTY = WiiMixEnums::Difficulty::NORMAL;
constexpr WiiMixEnums::Mode DEFAULT_MODE = WiiMixEnums::Mode::BINGO;
constexpr WiiMixEnums::SaveStateBank DEFAULT_SAVE_STATE_BANK = WiiMixEnums::SaveStateBank::USER;
#define DEFAULT_TIME 0
#define DEFAULT_GAMES {}
#define DEFAULT_OBJECTIVES {}

// BINGO
#define DEFAULT_IS_LOCKOUT false
#define DEFAULT_CARD_SIZE 25

// ROGUE

// SHUFFLE
#define DEFAULT_MIN_SWITCH_TIME 15
#define DEFAULT_MAX_SWITCH_TIME 60
#define DEFAULT_NUMBER_OF_SWITCHES 10
#define DEFAULT_IS_ENDLESS false