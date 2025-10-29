#include "DolphinQt/WiiMix/RogueSettings.h"
#include <QList>
#ifdef QT_GUI_LIB
    #include "Common/Config/Config.h"
    #include "Core/Config/MainSettings.h"
#endif

#include <QJsonObject>
#include <QJsonArray>

#include "DolphinQt/WiiMix/Enums.h"

WiiMixRogueSettings::WiiMixRogueSettings(WiiMixEnums::Difficulty difficulty, WiiMixEnums::SaveStateBank bank, std::vector<WiiMixObjective> objectives, std::vector<WiiMixEnums::ObjectiveType> types,
 std::vector<WiiMixEnums::GameGenre> genres, WiiMixEnums::RogueLength length, int num_players, WiiMixEnums::MultiplayerMode multiplayer_mode) : WiiMixCommonSettings(
    difficulty,
    bank,
    objectives,
    types,
    genres
), m_length(length), m_num_players(num_players), m_multiplayer_mode(multiplayer_mode) {
    #ifdef QT_GUI_LIB
    if (length != DEFAULT_ROGUE_LENGTH) {
        m_length = length;
    }
    else if (Config::Get(Config::WIIMIX_ROGUE_LENGTH) != DEFAULT_ROGUE_LENGTH) {
        m_length = Config::Get(Config::WIIMIX_ROGUE_LENGTH);
    }
    else {
        m_length = DEFAULT_ROGUE_LENGTH;
    }
    #endif

    if (length != DEFAULT_ROGUE_LENGTH) {
        m_length = length;
    }

    #ifdef QT_GUI_LIB
    if (num_players != DEFAULT_NUM_PLAYERS_ROGUE) {
        m_num_players = num_players;
    } else if (Config::Get(Config::WIIMIX_NUM_PLAYERS_ROGUE) != DEFAULT_NUM_PLAYERS_ROGUE) {
        m_num_players = Config::Get(Config::WIIMIX_NUM_PLAYERS_ROGUE);
    } else {
        m_num_players = DEFAULT_NUM_PLAYERS_ROGUE;
    }
    #endif

    if (num_players != DEFAULT_NUM_PLAYERS_ROGUE) {
        m_num_players = num_players;
    }

    #ifdef QT_GUI_LIB
    if (multiplayer_mode != DEFAULT_MULTIPLAYER_MODE_ROGUE) {
        m_multiplayer_mode = multiplayer_mode;
    } else if (Config::Get(Config::WIIMIX_MULTIPLAYER_MODE_ROGUE) != DEFAULT_MULTIPLAYER_MODE_ROGUE) {
        m_multiplayer_mode = Config::Get(Config::WIIMIX_MULTIPLAYER_MODE_ROGUE);
    } else {
        m_multiplayer_mode = DEFAULT_MULTIPLAYER_MODE_ROGUE;
    }
    #endif

    if (multiplayer_mode != DEFAULT_MULTIPLAYER_MODE_ROGUE) {
        m_multiplayer_mode = multiplayer_mode;
    }
}

struct WiiMixRogueSettings::RogueTile{
    WiiMixEnums::RogueItem item;
    char type; // "%" for event, "$" for item, "@" for boss
    WiiMixEnums::RogueEvent event;
    int objectiveId;
};

QString WiiMixRogueSettings::GetSeed()
{
    return m_seed;
}

void WiiMixRogueSettings::SetSeed(QString seed) {
    m_seed = seed;
}

QList<WiiMixEnums::RogueEvent> WiiMixRogueSettings::GetEvents()
{
    return m_events; // TODOx: temp to build
}

void WiiMixRogueSettings::SetDifficulty(WiiMixEnums::Difficulty difficulty) {
    m_difficulty = difficulty;
    #ifdef QT_GUI_LIB
        Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_DIFFICULTY, difficulty);
    #endif
    // emit SettingsChanged(difficulty);
}

void WiiMixRogueSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) {
    m_save_state_bank = save_state_bank;
    #ifdef QT_GUI_LIB
        Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_SAVE_STATE_BANK, save_state_bank);
    #endif
    // emit SettingsChanged(save_state_bank);
}

void WiiMixRogueSettings::SetObjectiveTypes(std::vector<WiiMixEnums::ObjectiveType> types) {
    m_objective_types = types;
    #ifdef QT_GUI_LIB
        Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_OBJECTIVE_TYPES, WiiMixEnums::ObjectiveTypesToString(types));
    #endif
    // emit SettingsChanged(types);
}

void WiiMixRogueSettings::SetGameGenres(std::vector<WiiMixEnums::GameGenre> genres) {
    m_game_genres = genres;
    #ifdef QT_GUI_LIB
        Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_GAME_GENRES, WiiMixEnums::GameGenresToString(genres));
    #endif
    // emit SettingsChanged(genres);
}

QList<WiiMixEnums::RogueItem> WiiMixRogueSettings::GetItemSet()
{
    // Returns m_items
    // Populates m_items using GetLength() if m_items is empty
    if (m_items.isEmpty())
    {
        WiiMixEnums::RogueLength length = GetLength();
        for (int i = 0; i < static_cast<int>(length); i++)
        {
            // Assuming Item has a constructor that takes an int or some other logic to populate items
            m_items.append(WiiMixEnums::RogueItem(i));
        }
    }
    return m_items;
}

void WiiMixRogueSettings::SetEvents(QList<WiiMixEnums::RogueEvent> events)
{
    m_events = events;
}

void WiiMixRogueSettings::SetItems(QList<WiiMixEnums::RogueItem> items)
{
    m_items = items;
}

WiiMixEnums::RogueLength WiiMixRogueSettings::GetLength()
{
    return m_length;
}

void WiiMixRogueSettings::SetLength(WiiMixEnums::RogueLength length)
{
    m_length = length;
}

QString WiiMixRogueSettings::LengthToString(WiiMixEnums::RogueLength length)
{
    switch (length) {
        case WiiMixEnums::RogueLength::SHORT:
            return QStringLiteral(ROGUE_LENGTH_SHORT);
        case WiiMixEnums::RogueLength::MEDIUM:
            return QStringLiteral(ROGUE_LENGTH_MEDIUM);
        case WiiMixEnums::RogueLength::LONGLENGTH:
            return QStringLiteral(ROGUE_LENGTH_LONGLENGTH);
        case WiiMixEnums::RogueLength::MARATHON:
            return QStringLiteral(ROGUE_LENGTH_MARATHON);
        default:
            return QStringLiteral("");
    }
}

WiiMixEnums::RogueLength WiiMixRogueSettings::StringToLength(QString length)
{
    if (length == QStringLiteral(ROGUE_LENGTH_SHORT)) {
        return WiiMixEnums::RogueLength::SHORT;
    }
    else if (length == QStringLiteral(ROGUE_LENGTH_MEDIUM)) {
        return WiiMixEnums::RogueLength::MEDIUM;
    }
    else if (length == QStringLiteral(ROGUE_LENGTH_LONGLENGTH)) {
        return WiiMixEnums::RogueLength::LONGLENGTH;
    }
    else if (length == QStringLiteral(ROGUE_LENGTH_MARATHON)) {
        return WiiMixEnums::RogueLength::MARATHON;
    }
    else {
        return WiiMixEnums::RogueLength::END;
    }
}

int WiiMixRogueSettings::GetNumPlayers() const
{
    return m_num_players;
}

void WiiMixRogueSettings::SetNumPlayers(int value)
{
    m_num_players = value;
}

WiiMixEnums::MultiplayerMode WiiMixRogueSettings::GetMultiplayerMode() const
{
    return m_multiplayer_mode;
}

void WiiMixRogueSettings::SetMultiplayerMode(WiiMixEnums::MultiplayerMode multiplayer_mode)
{
    m_multiplayer_mode = multiplayer_mode;
}

QJsonDocument WiiMixRogueSettings::ToJson()
{
    // Take care of the common settings first
    QJsonObject json = ToJsonCommon();
    json[QStringLiteral(ROGUE_SETTINGS_SEED)] = m_seed;
    json[QStringLiteral(ROGUE_SETTINGS_DIFFICULTY)] = static_cast<int>(m_difficulty);
    json[QStringLiteral(ROGUE_SETTINGS_SAVE_STATE_BANK)] = static_cast<int>(m_save_state_bank);
    json[QStringLiteral(ROGUE_SETTINGS_LENGTH)] = static_cast<int>(m_length);
    json[QStringLiteral(ROGUE_SETTINGS_NUM_PLAYERS)] = m_num_players;
    return QJsonDocument(json);
}

void WiiMixRogueSettings::FromJson(QJsonDocument json)
{
    QJsonObject jsonObject = json.object();
    m_seed = jsonObject[QStringLiteral(ROGUE_SETTINGS_SEED)].toString();
    m_difficulty = static_cast<WiiMixEnums::Difficulty>(jsonObject[QStringLiteral(ROGUE_SETTINGS_DIFFICULTY)].toInt());
    m_save_state_bank = static_cast<WiiMixEnums::SaveStateBank>(jsonObject[QStringLiteral(ROGUE_SETTINGS_SAVE_STATE_BANK)].toInt());
    m_length = static_cast<WiiMixEnums::RogueLength>(jsonObject[QStringLiteral(ROGUE_SETTINGS_LENGTH)].toInt());
    m_num_players = jsonObject[QStringLiteral(ROGUE_SETTINGS_NUM_PLAYERS)].toInt();
}

// returns true when seed is valid
// A valid seed is: "052%00000101%00000202$01%00000303%00000404@000003"
bool WiiMixRogueSettings::VerifyRogueSeed(std::string seed) {
    // Seed must start with a length (2 digits) and num_players (1 digit)
    if (seed.size() < 3 || !isdigit(seed[0]) || !isdigit(seed[1]) || !isdigit(seed[2]))
        return false;

    int expected_tile_count = std::stoi(seed.substr(0, 2));
    int num_players = seed[2] - '0';
    int actual_tile_count = 0;
    size_t i = 3;

    // Parse tiles
    while (i < seed.size()) {
        if (seed[i] == '%') {
            // Event tile: '%' + 6 digits (objectiveId) + 2 digits (event)
            if (i + 8 >= seed.size())
                return false;
            for (int j = 1; j <= 8; ++j)
                if (!isdigit(seed[i + j]))
                    return false;
            i += 9;
            actual_tile_count++;
        } else if (seed[i] == '$') {
            // Item tile: '$' + 2 digits (item id)
            if (i + 2 >= seed.size())
                return false;
            if (!isdigit(seed[i + 1]) || !isdigit(seed[i + 2]))
                return false;
            i += 3;
            actual_tile_count++;
        } else if (seed[i] == '@') {
            // Boss tiles: '@' + (6 or more digits, but always at end)
            i++;
            // Accept any number of boss tiles, each 6 digits (objectiveId) or 6+2 digits (objectiveId+event)
            while (i + 5 < seed.size()) {
                // Check for 6 digits
                for (int j = 0; j < 6; ++j)
                    if (!isdigit(seed[i + j]))
                        return false;
                i += 6;
                // Optionally, check for 2 more digits (event id)
                if (i + 1 < seed.size() && isdigit(seed[i]) && isdigit(seed[i + 1])) {
                    i += 2;
                }
                actual_tile_count++;
            }
            break; // Boss tiles are always at the end
        } else {
            // Invalid character
            return false;
        }
    }

    // Only count non-boss tiles for expected_tile_count
    // Boss tiles are handled at the end and not part of the initial count
    if (actual_tile_count < expected_tile_count)
        return false;

    return true;
}

std::string WiiMixRogueSettings::RogueTilesToSeed(std::vector<WiiMixRogueSettings::RogueTile> tiles, WiiMixEnums::Difficulty difficulty, int num_players, WiiMixEnums::RogueLength length) {
    std::string seed;
    // First two digits: tile count, third digit: num_players
    if (tiles.size() < 10) {
        seed = "0" + std::to_string(tiles.size());
    } else {
        seed = std::to_string(tiles.size());
    }
    seed += std::to_string(num_players);

    std::vector<WiiMixRogueSettings::RogueTile> boss_tiles;
    for (auto tile : tiles) {
        if (tile.type == '%') {
            // If the tile is an event
            // Append an objective id (6 digits) and event id (2 digits)
            seed += "%";
            char temp_str[9];
            snprintf(temp_str, 9, "%06d%02d", tile.objectiveId, static_cast<int>(tile.event));
            seed += temp_str;
        } else if (tile.type == '$') {
            // If the tile is an item
            // Append item id (2 digits)
            seed += "$";
            char temp_str[3];
            snprintf(temp_str, 3, "%02d", static_cast<int>(tile.item));
            seed += temp_str;
        } else if (tile.type == '@') {
            boss_tiles.push_back(tile);
        }
    }
    // Handle boss tiles at the end
    if (!boss_tiles.empty()) {
        seed += "@";
        for (auto tile : boss_tiles) {
            // If the tile is a boss
            // Append objective id (6 digits)
            // On normal, there's just a single boss with an event
            // On hard, there's 3 consecutive bosses with events
            // On wiisanity, there's 3 consecutive bosses with events PLUS an extra custom final boss against the emulator itself
            if (difficulty == WiiMixEnums::Difficulty::NORMAL) {
                // Single boss
                char temp_str[7];
                snprintf(temp_str, 7, "%06d", tile.objectiveId);
                seed += temp_str;
            } else if ((difficulty == WiiMixEnums::Difficulty::HARD || difficulty == WiiMixEnums::Difficulty::WIISANITY) && length == WiiMixEnums::RogueLength::MARATHON) {
                // Multiple bosses
                // Handled outside this function by adding multiple boss tiles
                // WiiSanity final boss is not seeded; it's always the same
                char temp_str[25];
                snprintf(temp_str, 25, "%06d%02d", tile.objectiveId, static_cast<int>(tile.event));
                seed += temp_str;
            }
        }
    }
    return seed;
}

// Return a new instance of rogue settings from the seed
// The seed should determine everything about the rogue run
WiiMixRogueSettings* WiiMixRogueSettings::SettingsFromSeed(QString seed) {
    std::string str_seed = seed.toStdString();
    if (!VerifyRogueSeed(str_seed)) {
        return nullptr; // Invalid seed
    }

    // Parse length and num_players from seed
    int expected_tile_count = std::stoi(str_seed.substr(0, 2));
    int num_players = str_seed[2] - '0';

    std::vector<RogueTile> tiles;
    size_t i = 3;

    // Parse tiles
    while (i < str_seed.size()) {
        if (str_seed[i] == '%') {
            // Event tile: '%' + 6 digits (objectiveId) + 2 digits (event)
            int objectiveId = std::stoi(str_seed.substr(i + 1, 6));
            WiiMixEnums::RogueEvent event = static_cast<WiiMixEnums::RogueEvent>(std::stoi(str_seed.substr(i + 7, 2)));
            tiles.push_back({WiiMixEnums::RogueItem::END, '%', event, objectiveId});
            i += 9;
        } else if (str_seed[i] == '$') {
            // Item tile: '$' + 2 digits (item id)
            WiiMixEnums::RogueItem item = static_cast<WiiMixEnums::RogueItem>(std::stoi(str_seed.substr(i + 1, 2)));
            tiles.push_back({item, '$', WiiMixEnums::RogueEvent::END, -1});
            i += 3;
        } else if (str_seed[i] == '@') {
            // Boss tiles: '@' + (6 or more digits, but always at end)
            i++;
            while (i + 5 < str_seed.size()) {
                int objectiveId = std::stoi(str_seed.substr(i, 6));
                WiiMixEnums::RogueEvent event = WiiMixEnums::RogueEvent::END;
                i += 6;
                // Optionally, check for 2 more digits (event id)
                if (i + 1 < str_seed.size() && isdigit(str_seed[i]) && isdigit(str_seed[i + 1])) {
                    event = static_cast<WiiMixEnums::RogueEvent>(std::stoi(str_seed.substr(i, 2)));
                    i += 2;
                }
                tiles.push_back({WiiMixEnums::RogueItem::END, '@', event, objectiveId});
            }
            break; // Boss tiles are always at the end
        } else {
            // Invalid character (should not happen due to prior verification)
            break;
        }
    }

    // Determine length based on expected_tile_count
    WiiMixEnums::RogueLength length;
    if (expected_tile_count == 3) {
        length = WiiMixEnums::RogueLength::SHORT;
    } else if (expected_tile_count == 6) {
        length = WiiMixEnums::RogueLength::MEDIUM;
    } else if (expected_tile_count == 9) {
        length = WiiMixEnums::RogueLength::LONGLENGTH;
    }else if (expected_tile_count == 18) {
        length = WiiMixEnums::RogueLength::MARATHON;
    } else {
        length = WiiMixEnums::RogueLength::END;
    }

    // Extract objectives and events from tiles
    std::vector<WiiMixEnums::ObjectiveType> objective_types;
    std::vector<WiiMixEnums::GameGenre> genres;
    QList<WiiMixEnums::RogueEvent> events;
    QList<WiiMixEnums::RogueItem> items;
    WiiMixEnums::Difficulty difficulty = WiiMixEnums::Difficulty::NORMAL; // Default, could be encoded in seed in future
    WiiMixEnums::SaveStateBank save_state_bank = WiiMixEnums::SaveStateBank::END; // Default

    // Create the settings object
    WiiMixRogueSettings* settings = new WiiMixRogueSettings(
        difficulty,
        save_state_bank,
        {},
        objective_types,
        genres,
        length,
        num_players,
        WiiMixEnums::MultiplayerMode::END // Default, could be encoded in seed
    );
    settings->SetSeed(seed);
    settings->SetEvents(events);
    settings->SetItems(items);
    settings->SetNumPlayers(num_players);
    settings->SetLength(length);

    return settings;
}

std::vector<int> WiiMixRogueSettings::SeedToObjectives(QString seed) {
    std::string str_seed = seed.toStdString();
    std::vector<int> objectives;
    size_t i = 2; // skip length prefix
    while (i < str_seed.size()) {
        if (str_seed[i] == '%') {
            // Event tile: '%' + 6 digits (objectiveId) + 2 digits (event)
            if (i + 8 < str_seed.size()) {
                std::string obj_id = str_seed.substr(i + 1, 6);
                objectives.push_back(std::stoi(obj_id));
                i += 9;
            } else {
                break;
            }
        } else if (str_seed[i] == '$') {
            // Item tile: skip
            i += 3;
        } else if (str_seed[i] == '@') {
            // Boss tiles start here, break
            break;
        } else {
            // Invalid character, skip
            i++;
        }
    }
    return objectives;
}

// reads final tile, as it's always boss tile
std::vector<int> WiiMixRogueSettings::SeedToBossObjectives(QString seed) {
    std::string str_seed = seed.toStdString();
    std::vector<int> bossObjectives;
    size_t at_pos = str_seed.find('@');
    if (at_pos == std::string::npos)
        return bossObjectives;
    size_t i = at_pos + 1;
    while (i + 5 < str_seed.size()) {
        std::string obj_id = str_seed.substr(i, 6);
        bossObjectives.push_back(std::stoi(obj_id));
        i += 6;
        // Optionally skip event id if present
        if (i + 1 < str_seed.size() && isdigit(str_seed[i]) && isdigit(str_seed[i + 1])) {
            i += 2;
        }
    }
    return bossObjectives;
}

int WiiMixRogueSettings::LengthToNumObjectives(WiiMixEnums::RogueLength length) {
    switch (length) {
        case WiiMixEnums::RogueLength::SHORT:
            return 3;
        case WiiMixEnums::RogueLength::MEDIUM:
            return 6;
        case WiiMixEnums::RogueLength::LONGLENGTH:
            return 9;
        case WiiMixEnums::RogueLength::MARATHON:
            return 18;
        default:
            return -1;
    }
}