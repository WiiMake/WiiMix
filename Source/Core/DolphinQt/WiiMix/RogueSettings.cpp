#include "DolphinQt/WiiMix/RogueSettings.h"
#include <QList>
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

#include <QJsonObject>
#include <QJsonArray>

WiiMixRogueSettings::WiiMixRogueSettings(WiiMixEnums::Difficulty difficulty, WiiMixEnums::SaveStateBank bank, std::vector<WiiMixObjective> objectives, WiiMixEnums::RogueLength length) : WiiMixCommonSettings(
    difficulty,
    bank,
    objectives
), m_length(length) {
    if (length != DEFAULT_ROGUE_LENGTH) {
        m_length = length;
    }
    else if (Config::Get(Config::WIIMIX_ROGUE_LENGTH) != DEFAULT_ROGUE_LENGTH) {
        m_length = Config::Get(Config::WIIMIX_ROGUE_LENGTH);
    }
    else {
        m_length = DEFAULT_ROGUE_LENGTH;
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
    Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_DIFFICULTY, difficulty);
    // emit SettingsChanged(difficulty);
}

void WiiMixRogueSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) {
    m_save_state_bank = save_state_bank;
    Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_SAVE_STATE_BANK, save_state_bank);
    // emit SettingsChanged(save_state_bank);
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

QJsonDocument WiiMixRogueSettings::ToJson()
{
    // Take care of the common settings first
    QJsonObject json = ToJsonCommon();
    json[QStringLiteral(ROGUE_SETTINGS_SEED)] = m_seed;
    json[QStringLiteral(ROGUE_SETTINGS_DIFFICULTY)] = static_cast<int>(m_difficulty);
    json[QStringLiteral(ROGUE_SETTINGS_SAVE_STATE_BANK)] = static_cast<int>(m_save_state_bank);

    return QJsonDocument(json);
}

void WiiMixRogueSettings::FromJson(QJsonDocument json)
{
    QJsonObject jsonObject = json.object();
    if (jsonObject.contains(QStringLiteral(ROGUE_SETTINGS_SEED))) {
        m_seed = jsonObject[QStringLiteral(ROGUE_SETTINGS_SEED)].toString();
    }
    if (jsonObject.contains(QStringLiteral(ROGUE_SETTINGS_DIFFICULTY))) {
        m_difficulty = static_cast<WiiMixEnums::Difficulty>(jsonObject[QStringLiteral(ROGUE_SETTINGS_DIFFICULTY)].toInt());
    }
    if (jsonObject.contains(QStringLiteral(ROGUE_SETTINGS_SAVE_STATE_BANK))) {
        m_save_state_bank = static_cast<WiiMixEnums::SaveStateBank>(jsonObject[QStringLiteral(ROGUE_SETTINGS_SAVE_STATE_BANK)].toInt());
    }
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
            snprintf(temp_str, 9, "%06d%02d", tile.objectiveId, tile.event);
            seed += temp_str;
        } else if (tile.type == '$') {
            seed += "$";
            char temp_str[3];
            snprintf(temp_str, 3, "%02d", tile.item);
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