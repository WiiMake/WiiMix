#pragma once

#include <string>

namespace WiiMixEnums {
    enum class Difficulty {
        EASY,
        NORMAL,
        HARD,
        WIISANITY,
        END // Default/size value
    };

    #define DIFFICULTY_EASY "EASY"
    #define DIFFICULTY_NORMAL "NORMAL"
    #define DIFFICULTY_HARD "HARD"
    #define DIFFICULTY_WIISANITY "WIISANITY"

    std::string DifficultyToString(Difficulty difficulty);
    Difficulty DifficultyFromString(const std::string& str);

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
        // TIME_ATTACK, // You can take achievements away from other people
        END, // Default/size value
    };

    enum class Player {
        ONE,
        TWO,
        THREE,
        FOUR,
        END, // Default/size value
    };

    enum class ObjectiveType {
        BOSS,
        LEVEL,
        PUZZLE,
        SCORE,
        RNG,
        TIMER,
        SURVIVAL,
        EASTER_EGG,
        COMPLETIONIST,
        PACIFIST,
        NO_HIT,
        GLITCH,
        END, // Default/size value
    };

    #define OBJECTIVE_TYPE_BOSS "BOSS"
    #define OBJECTIVE_TYPE_LEVEL "LEVEL"
    #define OBJECTIVE_TYPE_PUZZLE "PUZZLE"
    #define OBJECTIVE_TYPE_SCORE "SCORE"
    #define OBJECTIVE_TYPE_RNG "RNG"
    #define OBJECTIVE_TYPE_TIMER "TIMER"
    #define OBJECTIVE_TYPE_SURVIVAL "SURVIVAL"
    #define OBJECTIVE_TYPE_EASTER_EGG "EASTER_EGG"
    #define OBJECTIVE_TYPE_COMPLETIONIST "COMPLETIONIST"
    #define OBJECTIVE_TYPE_PACIFIST "PACIFIST"
    #define OBJECTIVE_TYPE_NO_HIT "NO_HIT"
    

    std::string ObjectiveTypeToString(ObjectiveType type);
    ObjectiveType ObjectiveTypeFromString(const std::string& str);

    // Sourced from the retroachievements genre list at https://retroachievements.org/game/9553
    enum class GameGenre {
        ACTION_ADVENTURE,
        ACTION,
        ADVENTURE,
        ARCADE,
        BOARD_AND_CARD,
        EDUCATIONAL,
        FIGHTING,
        HORROR,
        LITERATURE,
        OTHER,
        PLATFORMER,
        PUZZLE,
        RACING,
        RHYTHM,
        RPG,
        SHOOTER,
        SIMULATION,
        SPORTS,
        STRATEGY,
        END, // Default/size value
    };

    #define GAME_GENRE_ACTION_ADVENTURE "ACTION_ADVENTURE"
    #define GAME_GENRE_ACTION "ACTION"
    #define GAME_GENRE_ADVENTURE "ADVENTURE"
    #define GAME_GENRE_ARCADE "ARCADE"
    #define GAME_GENRE_BOARD_AND_CARD "BOARD_AND_CARD"
    #define GAME_GENRE_EDUCATIONAL "EDUCATIONAL"
    #define GAME_GENRE_FIGHTING "FIGHTING"
    #define GAME_GENRE_HORROR "HORROR"
    #define GAME_GENRE_LITERATURE "LITERATURE"
    #define GAME_GENRE_OTHER "OTHER"
    #define GAME_GENRE_PLATFORMER "PLATFORMER"
    #define GAME_GENRE_PUZZLE "PUZZLE"
    #define GAME_GENRE_RACING "RACING"
    #define GAME_GENRE_RHYTHM "RHYTHM"
    #define GAME_GENRE_RPG "RPG"
    #define GAME_GENRE_SHOOTER "SHOOTER"
    #define GAME_GENRE_SIMULATION "SIMULATION"
    #define GAME_GENRE_SPORTS "SPORTS"
    #define GAME_GENRE_STRATEGY "STRATEGY"

    // NOTE: instead of storing genres ourselves, we'll be pulling tags from the Retroachievements API
    // To populate genre

    Color PlayerToColor(Player player);

    std::string ColorToHex(Color color);

    // Action is an enum class that represents the different actions that can be taken by the server
    // This is included in the client request json to instruct the server what to do
    enum class Action {
        CREATE_BINGO_LOBBY,
        CONNECT_TO_BINGO_LOBBY,
        UPDATE_BINGO_LOBBY,
        UPDATE_OBJECTIVE_DB,
        ADD_OBJECTIVE, // Also adds the state
        DELETE_OBJECTIVE_DB,
        REMOVE_GENRE_FROM_OBJECTIVE_DB,
        ADD_GENRE_TO_OBJECTIVE_DB,
        REMOVE_OBJECTIVE_TYPE_FROM_OBJECTIVE_DB,
        ADD_OBJECTIVE_TYPE_TO_OBJECTIVE_DB,
        UPDATE_PLAYER,
        GET_PLAYERS, // you can search by username
        INCREMENT_PLAYER_STATS,
        GET_OBJECTIVE_HISTORY, // you can provide any of the objective history fields
        // GET_COMPLETED_OBJECTIVES_BY_PLAYER,
        // GET_FAILED_OBJECTIVES_BY_PLAYER,
        ADD_PLAYER,
        DELETE_PLAYER,
        MARK_OBJECTIVE_AS_COMPLETED,
        MARK_OBJECTIVE_AS_UNCOMPLETED,
        GET_OBJECTIVE_BY_ID,
        // GET_OBJECTIVES_BY_GAME_ID,
        // GET_OBJECTIVES_BY_RETROACHIEVEMENTS_GAME_ID,
        // GET_OBJECTIVES_BY_ACHIEVEMENT_ID,
        // GET_OBJECTIVES_BY_PLAYER_ID,
        // GET_OBJECTIVES_BY_GENRE,
        // GET_OBJECTIVES_BY_TYPE,
        // GET_OBJECTIVES_BY_DIFFICULTY,
        // GET_OBJECTIVES_BY_TIME,
        // GET_OBJECTIVES_BY_CREATOR,
        GET_OBJECTIVES, // you can provide any of the objective fields
        GET_OBJECTIVES_AND_STATES, // returns the json plus the save states; supports any objective field specifiers
        END // Default/size value
    };

    #define SERVER_ACTION "ACTION"
    #define CLIENT_RESPONSE "RESPONSE"
    
    // DBOps is used by the client to dictate which operation to perform on the database
    // An operation is associated with a column (or columns) in the json which is then parsed and acted out by the server

    // NOTE that filtering based on seeds is ALL handled server side, so the client doesn't need to worry about it

    // enum class DBOps {
    //     SORT_ASC,
    //     SORT_DESC,
    //     FILTER_GREATER_THAN,
    //     FILTER_LESS_THAN,
    //     SELECT,
    //     NUM_ROWS,
    //     END
    // };
    #define SORT_ASC "SORT_ASC"
    #define SORT_DESC "SORT_DESC"
    #define FILTER_GREATER_THAN "FILTER_GREATER_THAN"
    #define FILTER_LESS_THAN "FILTER_LESS_THAN"
    #define SELECT "SELECT"
    #define NUM_ROWS "NUM_ROWS"

    // Response is an enum class that represents the different responses that can be sent by the server
    // This is included in the server response json to instruct the client what to do
    enum class Response {
        UPDATE_BINGO_CONFIG, // This needs to update the lobby; update bingo settings should be a signal chain
        UPDATE_ROGUE_OBJECTIVES,
        UPDATE_BINGO_OBJECTIVES,
        GET_OBJECTIVES, // NOTE: this ONLY returns json; if you want to get the save states, you need to use either update bingo or update rogue
        GET_PLAYERS, // Retrieve a list of all wiimix players (not implemented yet)
        GET_PLAYER,
        GET_OBJECTIVE_HISTORY, // Retrieve a list of all objective history (not implemented yet)
        END
    };

    // Counter for the number of rows to retrieve

    // enum class BingoNetplaySettings {
    //     BINGO_TYPE,
    //     TEAMS,
    //     CARD_SIZE,
    //     PLAYERS,
    //     LOBBY_ID,
    //     LOBBY_PASSWORD,
    //     SEED,
    //     COLOR,
    //     NAME,
    //     CURRENT_OBJECTIVES,
    //     PLAYERS_READY,
    //     END // Default/size value
    // };

    // std::string BingoNetplaySettingsToString(BingoNetplaySettings setting);
    // BingoNetplaySettings BingoNetplaySettingsFromString(const std::string& str);

    // enum class CommonNetplaySettings {
    //     MODE,
    //     SAVE_STATE_BANK,
    //     OBJECTIVES,
    //     DIFFICULTY,
    //     GAMES_LIST,
    //     END // Default/size value
    // };
    
    // std::string CommonNetplaySettingsToString(CommonNetplaySettings setting);
    // CommonNetplaySettings CommonNetplaySettingsFromString(const std::string& str);    

    // Rogue Settings
    enum class RogueItem {
        REROLL_EFFECT, // Rerolls the effect for a round
        SWAP_OBJECTIVE, // Used to replace the current objective with a different one
        EXTRA_TIME, // Used automatically if you run out of time; gives you 15 extra seconds
        PAUSE, // Grants the ability to pause for a round
        SPOTLIGHT, // Grants the ability to view the next 3 objectives/challenges
        DOUBLE_TIME, // Make the current challenge slightly more difficult, but skip the next challenge on completion
        END
    };
    // There's 10 objectives; the 2nd, 4th, and 7th rounds are always items
    // The final stage is a different objective, but always the same Event:
    // A multiobjective guantlet of boss objectives between different games:
    // The first challenge is limited button presses
    // The next challenge has inverted movement every x seconds (random interval)
    // The final challenge is a survival shuffle round between 10 states marked as survival with save state manipulation,
    // pausing and playing, and speedup and slowdown
    // but if you survive for 100 seconds the next load will fail and you'll win
    enum class RogueEvent {
        GET_ITEM,
        LIMITED_A_BUTTON,
        LIMITED_B_BUTTON,
        INVERTED_X,
        INVERTED_Y,
        // INDUCED_LATENCY,
        INCREASED_SPEED,
        VARIABLE_SPEED,
        RANDOMIZED_CONTROLS,
        STRICTER_TIME_LIMIT,
        // DVD_SCREENSAVER_WINDOW,
        LIMITED_BUTTONS,
        INVERTED_MOVEMENT,
        SAVE_STATE_MANIPULATION,
        END
    };
    enum class RogueLength {
        SHORT, // 5
        MEDIUM, // 7
        MARATHON, // 10
        END
    };

    std::string GameGenreToString(GameGenre genre);
    GameGenre GameGenreFromString(const std::string& str);

    enum class Role {
        ADMIN, // Can create objectives, delete objectives, and update objectives. Can edit users (permissions/deletion).
        MODERATOR, // Can create objectives, delete objectives, and update objectives
        USER, // Can create objectives
        END
    };

    #define ROLES_ADMIN "ADMIN"
    #define ROLES_MODERATOR "MODERATOR"
    #define ROLES_USER "USER"

    std::string RoleToString(Role role);
    Role RoleFromString(const std::string& str);

    enum class ObjectiveStatus {
        COMPLETED,
        UNCOMPLETED,
        END
    };

    #define OBJECTIVE_STATUS_COMPLETED "COMPLETED"
    #define OBJECTIVE_STATUS_UNCOMPLETED "UNCOMPLETED"

    std::string ObjectiveStatusToString(ObjectiveStatus status);
    ObjectiveStatus ObjectiveStatusFromString(const std::string& str);
}

// Retroachievements currently (2024) supports around 400000 achievements
#define MAX_ACHIEVEMENT_ID_LENGTH 6
#define MAX_ACHIEVEMENT_ID "TODO"
// Retroachievements currently (2024) supports around 40000 games
#define MAX_GAME_ID_LENGTH 5
#define MAX_GAME_ID "TODO"

#define WII_MIX_UNKNOWN "UNKNOWN"

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
#define MAX_GAMES 10
#define DEFAULT_OBJECTIVES {}

// BINGO
#define DEFAULT_BINGO_DIFFICULTY WiiMixEnums::Difficulty::NORMAL
#define DEFAULT_BINGO_SAVE_STATE_BANK WiiMixEnums::SaveStateBank::UNVERIFIED
#define DEFAULT_BINGO_TYPE WiiMixEnums::BingoType::BINGO
#define DEFAULT_CARD_SIZE 25
#define DEFAULT_TEAMS false
#define MAX_PLAYERS 4
#define LOBBY_ID_LENGTH 32 // Uses QUuid, which ends up being length 32
#define MAX_LOBBY_PASSWORD_LENGTH 20
#define DEFAULT_CURRENT_OBJECTIVES {}

// BINGO NETWORKING
// inline int PORT = std::getenv("PORT") != nullptr ? std::stoi(std::getenv("PORT")) : -1;
// inline std::string IP = std::getenv("IP") ? std::getenv("IP") : "";

// ROGUE
#define DEFAULT_ROGUE_DIFFICULTY WiiMixEnums::Difficulty::NORMAL
#define DEFAULT_ROGUE_SAVE_STATE_BANK WiiMixEnums::SaveStateBank::UNVERIFIED
#define DEFAULT_ROGUE_LENGTH WiiMixEnums::RogueLength::MEDIUM

// SHUFFLE
#define DEFAULT_SHUFFLE_DIFFICULTY WiiMixEnums::Difficulty::NORMAL
#define DEFAULT_SHUFFLE_SAVE_STATE_BANK WiiMixEnums::SaveStateBank::UNVERIFIED
#define DEFAULT_MIN_SWITCH_TIME 15
#define DEFAULT_MAX_SWITCH_TIME 60
#define DEFAULT_NUMBER_OF_SWITCHES 10
#define DEFAULT_IS_ENDLESS false
