#pragma once

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

    enum class Color {
        RED,
        BLUE,
        PURPLE,
        GREEN,
        END, // Default/size value
    };

    enum class BingoType {
        BINGO,
        LOCKOUT,
        TIME_ATTACK,
        END, // Default/size value
    };

    enum class Player {
        ONE,
        TWO,
        THREE,
        FOUR,
        END, // Default/size value
    };

    Color PlayerToColor(Player player);

    Color StringToColor(const std::string& color);
    std::string ColorToHex(Color color);
}

// THEME
#define WII_MIX_RED "#da5133"
#define WII_MIX_BLUE "#61bbe8"
#define WII_MIX_PURPLE "#a961e8"
#define WII_MIX_GREEN "#61e873"

#define MIN_NUM_OBJECTIVES 1
#define MAX_NUM_OBJECTIVES 1000
constexpr WiiMixEnums::Difficulty DEFAULT_DIFFICULTY = WiiMixEnums::Difficulty::NORMAL;
constexpr WiiMixEnums::Mode DEFAULT_MODE = WiiMixEnums::Mode::BINGO;
constexpr WiiMixEnums::SaveStateBank DEFAULT_SAVE_STATE_BANK = WiiMixEnums::SaveStateBank::USER;
#define DEFAULT_TIME 0
#define DEFAULT_GAMES {}
#define DEFAULT_OBJECTIVES {}

// BINGO
#define DEFAULT_BINGO_TYPE WiiMixEnums::BingoType::BINGO
#define DEFAULT_CARD_SIZE 25
#define DEFAULT_TEAMS false
#define MAX_PLAYERS 4

// ROGUE

// SHUFFLE
#define DEFAULT_MIN_SWITCH_TIME 15
#define DEFAULT_MAX_SWITCH_TIME 60
#define DEFAULT_NUMBER_OF_SWITCHES 10
#define DEFAULT_IS_ENDLESS false