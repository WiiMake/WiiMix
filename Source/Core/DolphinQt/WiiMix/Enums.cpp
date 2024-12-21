#include "DolphinQt/WiiMix/Enums.h"

#include <string>

namespace WiiMixEnums {

    std::string DifficultyToString(Difficulty difficulty) {
        switch (difficulty) {
            case Difficulty::NORMAL:
                return DIFFICULTY_NORMAL;
            case Difficulty::HARD:
                return DIFFICULTY_HARD;
            case Difficulty::WIISANITY:
                return DIFFICULTY_WIISANITY;
            default:
                return WII_MIX_UNKNOWN;
        }
    }

    Difficulty DifficultyFromString(const std::string& str) {
        if (str == DIFFICULTY_NORMAL) {
            return Difficulty::NORMAL;
        } else if (str == DIFFICULTY_HARD) {
            return Difficulty::HARD;
        } else if (str == DIFFICULTY_WIISANITY) {
            return Difficulty::WIISANITY;
        } else {
            return Difficulty::END;
        }
    }


    // Color StringToColor(const std::string& color) {
    //     if (color == WII_MIX_RED) {
    //         return Color::RED;
    //     } else if (color == WII_MIX_GREEN) {
    //         return Color::GREEN;
    //     } else if (color == WII_MIX_BLUE) {
    //         return Color::BLUE;
    //     } else if (color == WII_MIX_PURPLE) {
    //         return Color::PURPLE;
    //     }
    //     else {
    //         return Color::END;
    //     }
    // }

    Color PlayerToColor(Player player) {
        switch (player) {
            case Player::ONE:
                return Color::RED;
            case Player::TWO:
                return Color::GREEN;
            case Player::THREE:
                return Color::BLUE;
            case Player::FOUR:
                return Color::PURPLE;
            default:
                return Color::END;
        }
    }

    std::string ColorToHex(Color color) {
        switch (color) {
            case Color::RED:
                return WII_MIX_RED;
            case Color::GREEN:
                return WII_MIX_GREEN;
            case Color::BLUE:
                return WII_MIX_BLUE;
            case Color::PURPLE:
                return WII_MIX_PURPLE;
            default:
                return WII_MIX_UNKNOWN;
        }
    }

    std::string BingoNetplaySettingsToString(BingoNetplaySettings setting) {
        switch (setting) {
            case BingoNetplaySettings::BINGO_TYPE:
                return BINGO_NETPLAY_SETTINGS_BINGO_TYPE;
            case BingoNetplaySettings::TEAMS:
                return BINGO_NETPLAY_SETTINGS_TEAMS;
            case BingoNetplaySettings::CARD_SIZE:
                return BINGO_NETPLAY_SETTINGS_CARD_SIZE;
            case BingoNetplaySettings::PLAYERS:
                return BINGO_NETPLAY_SETTINGS_PLAYERS;
            case BingoNetplaySettings::LOBBY_ID:
                return BINGO_NETPLAY_SETTINGS_LOBBY_ID;
            case BingoNetplaySettings::LOBBY_PASSWORD:
                return BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD;
            case BingoNetplaySettings::COLOR:
                return BINGO_NETPLAY_SETTINGS_COLOR;
            case BingoNetplaySettings::NAME:
                return BINGO_NETPLAY_SETTINGS_NAME;
            default:
                return WII_MIX_UNKNOWN;
        }
    }

    // BingoNetplaySettings StringToBingoNetplaySettings(const std::string& setting) {
    //     if (setting == BINGO_NETPLAY_SETTINGS_BINGO_TYPE) {
    //         return BingoNetplaySettings::BINGO_TYPE;
    //     } else if (setting == BINGO_NETPLAY_SETTINGS_TEAMS) {
    //         return BingoNetplaySettings::TEAMS;
    //     } else if (setting == BINGO_NETPLAY_SETTINGS_CARD_SIZE) {
    //         return BingoNetplaySettings::CARD_SIZE;
    //     } else if (setting == BINGO_NETPLAY_SETTINGS_PLAYERS) {
    //         return BingoNetplaySettings::PLAYERS;
    //     } else if (setting == BINGO_NETPLAY_SETTINGS_LOBBY_ID) {
    //         return BingoNetplaySettings::LOBBY_ID;
    //     } else if (setting == BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD) {
    //         return BingoNetplaySettings::LOBBY_PASSWORD;
    //     } else if (setting == BINGO_NETPLAY_SETTINGS_COLOR) {
    //         return BingoNetplaySettings::COLOR;
    //     } else if (setting == BINGO_NETPLAY_SETTINGS_NAME) {
    //         return BingoNetplaySettings::NAME;
    //     } else {
    //         return BingoNetplaySettings::END;
    //     }
    // }

    std::string CommonNetplaySettingsToString(CommonNetplaySettings setting) {
        switch (setting) {
            case CommonNetplaySettings::MODE:
                return COMMON_NETPLAY_SETTINGS_MODE;
            case CommonNetplaySettings::SAVE_STATE_BANK:
                return COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK;
            case CommonNetplaySettings::OBJECTIVES:
                return COMMON_NETPLAY_SETTINGS_OBJECTIVES;
            case CommonNetplaySettings::DIFFICULTY:
                return COMMON_NETPLAY_SETTINGS_DIFFICULTY;
            case CommonNetplaySettings::GAMES_LIST:
                return COMMON_NETPLAY_SETTINGS_GAMES_LIST;
            case CommonNetplaySettings::END:
                return WII_MIX_UNKNOWN;
            default:
                return WII_MIX_UNKNOWN;
        }
    }

    // CommonNetplaySettings StringToCommonNetplaySettings(const std::string& setting) {
    //     if (setting == COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK) {
    //         return CommonNetplaySettings::SAVE_STATE_BANK;
    //     } else if (setting == COMMON_NETPLAY_SETTINGS_OBJECTIVES) {
    //         return CommonNetplaySettings::OBJECTIVES;
    //     } else if (setting == COMMON_NETPLAY_SETTINGS_DIFFICULTY) {
    //         return CommonNetplaySettings::DIFFICULTY;
    //     } else if (setting == COMMON_NETPLAY_SETTINGS_GAMES_LIST) {
    //         return CommonNetplaySettings::GAMES_LIST;
    //     } else {
    //         return CommonNetplaySettings::END;
    //     }
    // }

    std::string ObjectiveTypeToString(ObjectiveType type)
    {
        switch (type) {
            case ObjectiveType::END:
                return WII_MIX_UNKNOWN;
            default:
                return WII_MIX_UNKNOWN;
        }
    }

    ObjectiveType ObjectiveTypeFromString(const std::string& str)
    {
        if (str == WII_MIX_UNKNOWN) {
            return ObjectiveType::END;
        }
        return ObjectiveType::END;
    }

    std::string GameGenreToString(GameGenre genre)
    {
        switch (genre) {
            case GameGenre::END:
                return WII_MIX_UNKNOWN;
            default:
                return WII_MIX_UNKNOWN;
        }
    }
    GameGenre GameGenreFromString(const std::string& str)
    {
        if (str == WII_MIX_UNKNOWN) {
            return GameGenre::END;
        }
        return GameGenre::END;
    }

};
