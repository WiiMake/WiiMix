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
            return QStringLiteral("Short");
        case WiiMixEnums::RogueLength::MEDIUM:
            return QStringLiteral("Medium");
        case WiiMixEnums::RogueLength::MARATHON:
            return QStringLiteral("Marathon");
        default:
            return QStringLiteral("");
    }
}

WiiMixEnums::RogueLength WiiMixRogueSettings::StringToLength(QString length)
{
    if (length == QStringLiteral("Short")) {
        return WiiMixEnums::RogueLength::SHORT;
    }
    else if (length == QStringLiteral("Medium")) {
        return WiiMixEnums::RogueLength::MEDIUM;
    }
    else if (length == QStringLiteral("Long") || length == QStringLiteral("MARATHON")) {
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

// returns true when seed is correct
bool WiiMixRogueSettings::VerifyRogueSeed(std::string seed) {
    try {
        char current_type;
        int id_len = -1;
        int length = std::stoi(seed.substr(0, 2));
        int factual_length = 0;
        for (int i = 2; i < seed.length(); i++) {
            if (seed[i] == '%') {
                if (id_len == -1 ||
                ( (current_type == '%' && id_len == 8)
                || (current_type == '$' && id_len == 2)
                || (current_type == '@' && id_len == 6))) {
                    id_len = 0;
                } else {
                    return false;
                }
                current_type = '%';
                factual_length++;
            } else if (seed[i] == '$') {
                if (id_len == -1 ||
                ( (current_type == '%' && id_len == 8)
                || (current_type == '$' && id_len == 2)
                || (current_type == '@' && id_len == 6))) {
                    id_len = 0;
                } else {
                    return false;
                }
                current_type = '$';
                factual_length++;
            } else if (seed[i] == '@') {
                if (id_len == -1 ||
                ( (current_type == '%' && id_len == 8)
                || (current_type == '$' && id_len == 2)
                || (current_type == '@' && id_len == 6))) {
                    id_len = 0;
                } else {
                    return false;
                }
                current_type = '@';
                factual_length++;
            } else if (seed[i] < 48 || seed[i] > 57) {
                return false;
            } else {
                if ((id_len > 8 && current_type == '%')
                || (id_len > 2 && current_type == '$')
                || (id_len > 6 && current_type == '@')) {
                    return false;
                }
                id_len += 1;
            }
        }
        if (length != factual_length) {
            return false;
        }
    } catch (std::out_of_range e) {
        return false;
    }
    return true;
}

std::string WiiMixRogueSettings::RogueTilesToSeed(std::vector<WiiMixRogueSettings::RogueTile> tiles) {
    std::string seed;
    if (tiles.size() < 10) {
        seed = "0" + std::to_string(tiles.size());
    } else {
        seed = std::to_string(tiles.size());
    }
    for (auto tile : tiles) {
        if (tile.type == '%') {
            seed += "%";
            char temp_str[9];
            snprintf(temp_str, 9, "%06d%02d", tile.objectiveId, static_cast<int>(tile.event));
            seed += temp_str;
        } else if (tile.type == '$') {
            seed += "$";
            char temp_str[3];
            snprintf(temp_str, 3, "%02d", static_cast<int>(tile.item));
            seed += temp_str;
        } else {
            seed += "@";
            char temp_str[7];
            snprintf(temp_str, 7, "%06d", tile.objectiveId);
            seed += temp_str;
        }
    }
    return seed;
}

std::vector<int> WiiMixRogueSettings::SeedToObjectives(QString seed) {
    std::string str_seed = seed.toStdString();
    std::vector<int> objectives;
    std::string objective;
    int k = -1;
    for (char c : str_seed) {
        if (c == '%' || k != -1) {
            if (k != -1) {
                objective += c;
            }
            k++;
        }
        if (k == 6) {
            objectives.push_back(std::stoi(objective));
            k = -1;
        }
    }
    return objectives;
}

// reads final tile, as it's always boss tile
std::vector<int> WiiMixRogueSettings::SeedToBossObjectives(QString seed) {
    std::string str_seed = seed.toStdString();
    std::vector<int> bossObjectives;
    std::string objective;
    int k = -1;
    for (char c : str_seed) {
        if (c == '@' || k != -1) {
            if (k != -1) {
                objective += c;
            }
            k++;
        }
        if (k == 6) {
            bossObjectives.push_back(std::stoi(objective));
            k = -1;
        }
    }
    return bossObjectives;
}

int WiiMixRogueSettings::LengthToNumObjectives(WiiMixEnums::RogueLength length) {
    switch (length) {
        case WiiMixEnums::RogueLength::SHORT:
            return 5;
        case WiiMixEnums::RogueLength::MEDIUM:
            return 7;
        case WiiMixEnums::RogueLength::MARATHON:
            return 10;
        default:
            return -1;
    }
}