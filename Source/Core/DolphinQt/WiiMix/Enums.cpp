#include "DolphinQt/WiiMix/Enums.h"

#include <string>

namespace WiiMixEnums {

    std::string DifficultyToString(Difficulty difficulty) {
        switch (difficulty) {
            case Difficulty::EASY:
                return DIFFICULTY_EASY;
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
        if (str == DIFFICULTY_EASY) {
            return Difficulty::EASY;
        }
        else if (str == DIFFICULTY_NORMAL) {
            return Difficulty::NORMAL;
        } else if (str == DIFFICULTY_HARD) {
            return Difficulty::HARD;
        } else if (str == DIFFICULTY_WIISANITY) {
            return Difficulty::WIISANITY;
        } else {
            return Difficulty::END;
        }
    }

    std::string SaveStateBankToString(SaveStateBank bank) {
        switch (bank) {
            case SaveStateBank::VERIFIED:
                return SAVE_STATE_BANK_VERIFIED;
            case SaveStateBank::UNVERIFIED:
                return SAVE_STATE_BANK_UNVERIFIED;
            case SaveStateBank::END:
                return WII_MIX_UNKNOWN;
            default:
                return WII_MIX_UNKNOWN;
        }
    }

    SaveStateBank SaveStateBankFromString(const std::string& str) {
        if (str == SAVE_STATE_BANK_VERIFIED) {
            return SaveStateBank::VERIFIED;
        } else if (str == SAVE_STATE_BANK_UNVERIFIED) {
            return SaveStateBank::UNVERIFIED;
        } else {
            return SaveStateBank::END;
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

    // std::string BingoNetplaySettingsToString(BingoNetplaySettings setting) {
    //     switch (setting) {
    //         case BingoNetplaySettings::BINGO_TYPE:
    //             return BINGO_SETTINGS_BINGO_TYPE;
    //         case BingoNetplaySettings::TEAMS:
    //             return BINGO_SETTINGS_TEAMS;
    //         case BingoNetplaySettings::CARD_SIZE:
    //             return BINGO_SETTINGS_CARD_SIZE;
    //         case BingoNetplaySettings::PLAYERS:
    //             return BINGO_SETTINGS_PLAYERS;
    //         case BingoNetplaySettings::LOBBY_ID:
    //             return BINGO_SETTINGS_LOBBY_ID;
    //         case BingoNetplaySettings::LOBBY_PASSWORD:
    //             return BINGO_SETTINGS_LOBBY_PASSWORD;
    //         case BingoNetplaySettings::COLOR:
    //             return BINGO_SETTINGS_COLOR;
    //         case BingoNetplaySettings::NAME:
    //             return BINGO_SETTINGS_NAME;
    //         default:
    //             return WII_MIX_UNKNOWN;
    //     }
    // }

    // BingoNetplaySettings StringToBingoNetplaySettings(const std::string& setting) {
    //     if (setting == BINGO_SETTINGS_BINGO_TYPE) {
    //         return BingoNetplaySettings::BINGO_TYPE;
    //     } else if (setting == BINGO_SETTINGS_TEAMS) {
    //         return BingoNetplaySettings::TEAMS;
    //     } else if (setting == BINGO_SETTINGS_CARD_SIZE) {
    //         return BingoNetplaySettings::CARD_SIZE;
    //     } else if (setting == BINGO_SETTINGS_PLAYERS) {
    //         return BingoNetplaySettings::PLAYERS;
    //     } else if (setting == BINGO_SETTINGS_LOBBY_ID) {
    //         return BingoNetplaySettings::LOBBY_ID;
    //     } else if (setting == BINGO_SETTINGS_LOBBY_PASSWORD) {
    //         return BingoNetplaySettings::LOBBY_PASSWORD;
    //     } else if (setting == BINGO_SETTINGS_COLOR) {
    //         return BingoNetplaySettings::COLOR;
    //     } else if (setting == BINGO_SETTINGS_NAME) {
    //         return BingoNetplaySettings::NAME;
    //     } else {
    //         return BingoNetplaySettings::END;
    //     }
    // }

    // std::string CommonNetplaySettingsToString(CommonNetplaySettings setting) {
    //     switch (setting) {
    //         case CommonNetplaySettings::MODE:
    //             return COMMON_SETTINGS_MODE;
    //         case CommonNetplaySettings::SAVE_STATE_BANK:
    //             return COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK;
    //         case CommonNetplaySettings::OBJECTIVES:
    //             return COMMON_SETTINGS_OBJECTIVES;
    //         case CommonNetplaySettings::DIFFICULTY:
    //             return COMMON_SETTINGS_DIFFICULTY;
    //         case CommonNetplaySettings::GAMES_LIST:
    //             return COMMON_SETTINGS_GAMES_LIST;
    //         case CommonNetplaySettings::END:
    //             return WII_MIX_UNKNOWN;
    //         default:
    //             return WII_MIX_UNKNOWN;
    //     }
    // }

    // CommonNetplaySettings StringToCommonNetplaySettings(const std::string& setting) {
    //     if (setting == COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK) {
    //         return CommonNetplaySettings::SAVE_STATE_BANK;
    //     } else if (setting == COMMON_SETTINGS_OBJECTIVES) {
    //         return CommonNetplaySettings::OBJECTIVES;
    //     } else if (setting == COMMON_SETTINGS_DIFFICULTY) {
    //         return CommonNetplaySettings::DIFFICULTY;
    //     } else if (setting == COMMON_SETTINGS_GAMES_LIST) {
    //         return CommonNetplaySettings::GAMES_LIST;
    //     } else {
    //         return CommonNetplaySettings::END;
    //     }
    // }

    std::string ObjectiveTypeToString(ObjectiveType type)
    {
        switch (type) {
            case ObjectiveType::BOSS:
                return OBJECTIVE_TYPE_BOSS;
            case ObjectiveType::LEVEL:
                return OBJECTIVE_TYPE_LEVEL;
            case ObjectiveType::PUZZLE:
                return OBJECTIVE_TYPE_PUZZLE;
            case ObjectiveType::SCORE:
                return OBJECTIVE_TYPE_SCORE;
            case ObjectiveType::RNG:
                return OBJECTIVE_TYPE_RNG;
            case ObjectiveType::TIMER:
                return OBJECTIVE_TYPE_TIMER;
            case ObjectiveType::SURVIVAL:
                return OBJECTIVE_TYPE_SURVIVAL;
            case ObjectiveType::EASTER_EGG:
                return OBJECTIVE_TYPE_EASTER_EGG;
            case ObjectiveType::COMPLETIONIST:
                return OBJECTIVE_TYPE_COMPLETIONIST;
            case ObjectiveType::PACIFIST:
                return OBJECTIVE_TYPE_PACIFIST;
            case ObjectiveType::NO_HIT:
                return OBJECTIVE_TYPE_NO_HIT;
            case ObjectiveType::GLITCH:
                return OBJECTIVE_TYPE_GLITCH;
            case ObjectiveType::SPEEDRUN:
                return OBJECTIVE_TYPE_SPEEDRUN;
            // case ObjectiveType::VERSUS:
            //     return OBJECTIVE_TYPE_VERSUS;
            case ObjectiveType::END:
                return WII_MIX_UNKNOWN;
            default:
                return WII_MIX_UNKNOWN;
        }
    }

    ObjectiveType ObjectiveTypeFromString(const std::string& str)
    {
        if (str == OBJECTIVE_TYPE_BOSS) {
            return ObjectiveType::BOSS;
        } else if (str == OBJECTIVE_TYPE_LEVEL) {
            return ObjectiveType::LEVEL;
        } else if (str == OBJECTIVE_TYPE_PUZZLE) {
            return ObjectiveType::PUZZLE;
        } else if (str == OBJECTIVE_TYPE_SCORE) {
            return ObjectiveType::SCORE;
        } else if (str == OBJECTIVE_TYPE_RNG) {
            return ObjectiveType::RNG;
        } else if (str == OBJECTIVE_TYPE_TIMER) {
            return ObjectiveType::TIMER;
        } else if (str == OBJECTIVE_TYPE_SURVIVAL) {
            return ObjectiveType::SURVIVAL;
        } else if (str == OBJECTIVE_TYPE_EASTER_EGG) {
            return ObjectiveType::EASTER_EGG;
        } else if (str == OBJECTIVE_TYPE_COMPLETIONIST) {
            return ObjectiveType::COMPLETIONIST;
        } else if (str == OBJECTIVE_TYPE_PACIFIST) {
            return ObjectiveType::PACIFIST;
        } else if (str == OBJECTIVE_TYPE_NO_HIT) {
            return ObjectiveType::NO_HIT;
        } else if (str == OBJECTIVE_TYPE_GLITCH) {
            return ObjectiveType::GLITCH;
        // } else if (str == OBJECTIVE_TYPE_VERSUS) {
        //     return ObjectiveType::VERSUS;
        } else if (str == OBJECTIVE_TYPE_SPEEDRUN) {
            return ObjectiveType::SPEEDRUN;
        } else {
            return ObjectiveType::END;
        }
    }

    std::string GameGenreToString(GameGenre genre)
    {
        switch (genre) {
            case GameGenre::ACTION_ADVENTURE:
                return GAME_GENRE_ACTION_ADVENTURE;
            case GameGenre::ACTION:
                return GAME_GENRE_ACTION;
            case GameGenre::ADVENTURE:
                return GAME_GENRE_ADVENTURE;
            case GameGenre::ARCADE:
                return GAME_GENRE_ARCADE;
            case GameGenre::BOARD_AND_CARD:
                return GAME_GENRE_BOARD_AND_CARD;
            case GameGenre::EDUCATIONAL:
                return GAME_GENRE_EDUCATIONAL;
            case GameGenre::FIGHTING:
                return GAME_GENRE_FIGHTING;
            case GameGenre::HORROR:
                return GAME_GENRE_HORROR;
            case GameGenre::LITERATURE:
                return GAME_GENRE_LITERATURE;
            case GameGenre::OTHER:
                return GAME_GENRE_OTHER;
            case GameGenre::PLATFORMER:
                return GAME_GENRE_PLATFORMER;
            case GameGenre::PUZZLE:
                return GAME_GENRE_PUZZLE;
            case GameGenre::RACING:
                return GAME_GENRE_RACING;
            case GameGenre::RHYTHM:
                return GAME_GENRE_RHYTHM;
            case GameGenre::RPG:
                return GAME_GENRE_RPG;
            case GameGenre::SHOOTER:
                return GAME_GENRE_SHOOTER;
            case GameGenre::SIMULATION:
                return GAME_GENRE_SIMULATION;
            case GameGenre::SPORTS:
                return GAME_GENRE_SPORTS;
            case GameGenre::STRATEGY:
                return GAME_GENRE_STRATEGY;
            case GameGenre::END:
                return WII_MIX_UNKNOWN;
            default:
                return WII_MIX_UNKNOWN;
        }
    }
    GameGenre GameGenreFromString(const std::string& str)
    {
        if (str == GAME_GENRE_ACTION_ADVENTURE) {
        return GameGenre::ACTION_ADVENTURE;
        } else if (str == GAME_GENRE_ACTION) {
        return GameGenre::ACTION;
        } else if (str == GAME_GENRE_ADVENTURE) {
        return GameGenre::ADVENTURE;
        } else if (str == GAME_GENRE_ARCADE) {
        return GameGenre::ARCADE;
        } else if (str == GAME_GENRE_BOARD_AND_CARD) {
        return GameGenre::BOARD_AND_CARD;
        } else if (str == GAME_GENRE_EDUCATIONAL) {
        return GameGenre::EDUCATIONAL;
        } else if (str == GAME_GENRE_FIGHTING) {
        return GameGenre::FIGHTING;
        } else if (str == GAME_GENRE_HORROR) {
        return GameGenre::HORROR;
        } else if (str == GAME_GENRE_LITERATURE) {
        return GameGenre::LITERATURE;
        } else if (str == GAME_GENRE_OTHER) {
        return GameGenre::OTHER;
        } else if (str == GAME_GENRE_PLATFORMER) {
        return GameGenre::PLATFORMER;
        } else if (str == GAME_GENRE_PUZZLE) {
        return GameGenre::PUZZLE;
        } else if (str == GAME_GENRE_RACING) {
        return GameGenre::RACING;
        } else if (str == GAME_GENRE_RHYTHM) {
        return GameGenre::RHYTHM;
        } else if (str == GAME_GENRE_RPG) {
        return GameGenre::RPG;
        } else if (str == GAME_GENRE_SHOOTER) {
        return GameGenre::SHOOTER;
        } else if (str == GAME_GENRE_SIMULATION) {
        return GameGenre::SIMULATION;
        } else if (str == GAME_GENRE_SPORTS) {
        return GameGenre::SPORTS;
        } else if (str == GAME_GENRE_STRATEGY) {
        return GameGenre::STRATEGY;
        } else {
        return GameGenre::END;
        }
    }

    Role RoleFromString(const std::string& str) {
        if (str == ROLES_ADMIN) {
            return Role::ADMIN;
        } else if (str == ROLES_MODERATOR) {
            return Role::MODERATOR;
        } else if (str == ROLES_USER) {
            return Role::USER;
        } else {
            return Role::END;
        }
    }

    std::string RoleToString(Role role) {
        switch (role) {
            case Role::ADMIN:
                return ROLES_ADMIN;
            case Role::MODERATOR:
                return ROLES_MODERATOR;
            case Role::USER:
                return ROLES_USER;
            case Role::END:
                return WII_MIX_UNKNOWN;
            default:
                return WII_MIX_UNKNOWN;
        }
    }

    std::string ObjectiveStatusToString(ObjectiveStatus status) {
        switch (status) {
            case ObjectiveStatus::COMPLETED:
                return OBJECTIVE_STATUS_COMPLETED;
            case ObjectiveStatus::UNCOMPLETED:
                return OBJECTIVE_STATUS_UNCOMPLETED;
            case ObjectiveStatus::END:
                return WII_MIX_UNKNOWN;
            default:
                return WII_MIX_UNKNOWN;
        }
    }

    ObjectiveStatus ObjectiveStatusFromString(const std::string& str) {
        if (str == OBJECTIVE_STATUS_COMPLETED) {
            return ObjectiveStatus::COMPLETED;
        } else if (str == OBJECTIVE_STATUS_UNCOMPLETED) {
            return ObjectiveStatus::UNCOMPLETED;
        } else {
            return ObjectiveStatus::END;
        }
    }

    std::string GameGenresToString(std::vector<GameGenre> genres) {
        std::string result;
        for (const auto& genre : genres) {
            if (!result.empty()) {
                result += ",";
            }
            result += GameGenreToString(genre);
        }
        return result;
    }

    std::vector<GameGenre> GameGenresFromString(const std::string& str) {
        std::vector<GameGenre> genres;
        std::string::size_type start = 0;
        std::string::size_type end = str.find(',');

        while (end != std::string::npos) {
            genres.push_back(GameGenreFromString(str.substr(start, end - start)));
            start = end + 1;
            end = str.find(',', start);
        }

        genres.push_back(GameGenreFromString(str.substr(start)));
        return genres;
    }

    std::string ObjectiveTypesToString(std::vector<ObjectiveType> types) {
        std::string result;
        for (const auto& type : types) {
            if (!result.empty()) {
                result += ",";
            }
            result += ObjectiveTypeToString(type);
        }
        return result;
    }

    std::vector<ObjectiveType> ObjectiveTypesFromString(const std::string& str) {
        std::vector<ObjectiveType> types;
        std::string::size_type start = 0;
        std::string::size_type end = str.find(',');

        while (end != std::string::npos) {
            types.push_back(ObjectiveTypeFromString(str.substr(start, end - start)));
            start = end + 1;
            end = str.find(',', start);
        }

        types.push_back(ObjectiveTypeFromString(str.substr(start)));
        return types;
    }

    MultiplayerMode StringToMultiplayerMode(const std::string& str) {
        if (str == MULTIPLAYER_MODE_COOP) {
            return MultiplayerMode::COOP;
        } else if (str == MULTIPLAYER_MODE_VERSUS) {
            return MultiplayerMode::VERSUS;
        } else {
            return MultiplayerMode::END;
        }
    }

    std::string MultiplayerModeToString(MultiplayerMode mode) {
        switch (mode) {
            case MultiplayerMode::COOP:
                return MULTIPLAYER_MODE_COOP;
            case MultiplayerMode::VERSUS:
                return MULTIPLAYER_MODE_VERSUS;
            case MultiplayerMode::END:
                return WII_MIX_UNKNOWN;
            default:
                return WII_MIX_UNKNOWN;
        }
    }
};
