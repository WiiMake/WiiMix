#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

#include "DolphinQt/WiiMix/CommonSettings.h"

#include "DolphinQt/WiiMix/Enums.h"

WiiMixCommonSettings::WiiMixCommonSettings(WiiMixEnums::Difficulty difficulty, WiiMixEnums::SaveStateBank bank,
    std::vector<WiiMixObjective> objectives, std::vector<WiiMixEnums::ObjectiveType> types, std::vector<WiiMixEnums::GameGenre> genres) : m_difficulty(difficulty), m_save_state_bank(bank), m_objectives(objectives), m_objective_types(types), m_game_genres(genres) {
    if (difficulty != DEFAULT_DIFFICULTY) {
        m_difficulty = difficulty;
    }
    else {
        m_difficulty = DEFAULT_DIFFICULTY;
    }

    if (bank != DEFAULT_SAVE_STATE_BANK) {
        m_save_state_bank = bank;
    }
    else {
        m_save_state_bank = DEFAULT_SAVE_STATE_BANK;
    }
    // Config::Get(Config::WIIMIX_TIME);
    // m_time = time;
    // Objectives should always be an empty list on the very first load; it gets populated by:
    // - Starting the WiiMix
    // - Pressing save
    // - Loading someone else's objectives
    // Note that objectives get cleared at the end of a WiiMix (if the Quit or WiiMix buttons are pressed instead of replay),
    // meaning if you want try the same set of objectives you'll need to reload the config file
    // std::vector<WiiMixObjective> config_objectives = WiiMixCommonSettings::ObjectiveIdsToObjectives(Config::Get(Config::WIIMIX_OBJECTIVE_IDS));
    // if (objectives.size() != 0) {
    //     m_objectives = objectives;
    // }
    // else if (config_objectives.size() != 0) {
    //     m_objectives = objectives;
    // }
    // else {
    m_objectives = DEFAULT_OBJECTIVES;
    // }
}

// These methods are used in GUI, hence the Paschal Case
QString WiiMixCommonSettings::DifficultyToString(WiiMixEnums::Difficulty difficulty) {
    switch (difficulty)
    {
        case WiiMixEnums::Difficulty::EASY:
            return QStringLiteral(DIFFICULTY_EASY);
        case WiiMixEnums::Difficulty::NORMAL:
            return QStringLiteral(DIFFICULTY_NORMAL);
        case WiiMixEnums::Difficulty::HARD:
            return QStringLiteral(DIFFICULTY_HARD);
        case WiiMixEnums::Difficulty::WIISANITY:
            return QStringLiteral(DIFFICULTY_WIISANITY);
        default:
            return QStringLiteral("");
    }
}

WiiMixEnums::Difficulty WiiMixCommonSettings::StringToDifficulty(QString difficulty) {
    if (difficulty == QStringLiteral(DIFFICULTY_EASY))
        return WiiMixEnums::Difficulty::EASY;
    else if (difficulty == QStringLiteral(DIFFICULTY_NORMAL))
        return WiiMixEnums::Difficulty::NORMAL;
    else if (difficulty == QStringLiteral(DIFFICULTY_HARD))
        return WiiMixEnums::Difficulty::HARD;
    else if (difficulty == QStringLiteral(DIFFICULTY_WIISANITY))
        return WiiMixEnums::Difficulty::WIISANITY;
    else
        return WiiMixEnums::Difficulty::NORMAL; // Default case
}

QString WiiMixCommonSettings::SaveStateBankToString(WiiMixEnums::SaveStateBank bank) {
    switch (bank)
    {
        // case WiiMixEnums::SaveStateBank::USER:
        //     return QStringLiteral("User");
        case WiiMixEnums::SaveStateBank::VERIFIED:
            return QStringLiteral(SAVE_STATE_BANK_VERIFIED);
        case WiiMixEnums::SaveStateBank::UNVERIFIED:
            return QStringLiteral(SAVE_STATE_BANK_UNVERIFIED);
        default:
            return QStringLiteral("");
    }
}

WiiMixEnums::SaveStateBank WiiMixCommonSettings::StringToSaveStateBank(QString bank) {
    // if (bank == QStringLiteral("User"))
    //     return WiiMixEnums::SaveStateBank::USER;
    if (bank == QStringLiteral(SAVE_STATE_BANK_VERIFIED))
        return WiiMixEnums::SaveStateBank::VERIFIED;
    else if (bank == QStringLiteral(SAVE_STATE_BANK_UNVERIFIED))
        return WiiMixEnums::SaveStateBank::UNVERIFIED;
    else
        return WiiMixEnums::SaveStateBank::UNVERIFIED;
        // return WiiMixEnums::SaveStateBank::USER; // Default case
}

void WiiMixCommonSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank bank) {
    m_save_state_bank = bank;
}

void WiiMixCommonSettings::SetDifficulty(WiiMixEnums::Difficulty difficulty) {
    m_difficulty = difficulty;
}

// void WiiMixCommonSettings::SetTime(int time) {
//     m_time = time;
// }

void WiiMixCommonSettings::SetObjectives(std::vector<WiiMixObjective> objectives) {
    m_objectives = objectives;
    // Note that objectives get populated once a WiiMix starts, not during the initial load
    // So nothing is stored in config
    // This also serves to minimize retroachievements and wiimix server requests
}

void WiiMixCommonSettings::AddObjective(WiiMixObjective objective) {
    m_objectives.push_back(objective);
}

QJsonObject WiiMixCommonSettings::ToJsonCommon() {
    QJsonObject json;
    json[QStringLiteral(COMMON_SETTINGS_SAVE_STATE_BANK)] = static_cast<int>(GetSaveStateBank());
    std::vector<WiiMixObjective> objectives = GetObjectives();
    if (objectives.size() > 0) {
        QJsonArray selected_objectives;
         
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(objectives.begin(), objectives.end(), g);
        
        qDebug() << "append objectives";
        for (int i = 0; i < 9; i++) {
            selected_objectives.append(objectives[i].ToJson());
        }
        json[QStringLiteral(COMMON_SETTINGS_OBJECTIVES)] = selected_objectives;
    }
    else {
        json[QStringLiteral(COMMON_SETTINGS_OBJECTIVES)] = {};
    }
    json[QStringLiteral(COMMON_SETTINGS_DIFFICULTY)] = static_cast<int>(GetDifficulty());
    json[QStringLiteral(COMMON_SETTINGS_OBJECTIVE_TYPES)] = QString::fromStdString(WiiMixEnums::ObjectiveTypesToString(GetObjectiveTypes()));
    json[QStringLiteral(COMMON_SETTINGS_GAME_GENRES)] = QString::fromStdString(WiiMixEnums::GameGenresToString(GetGameGenres()));
    return json;
}

WiiMixCommonSettings WiiMixCommonSettings::FromJsonCommon(QJsonDocument settings_json) {
    QJsonObject obj = settings_json.object();
    WiiMixEnums::SaveStateBank bank = static_cast<WiiMixEnums::SaveStateBank>(obj[QStringLiteral(COMMON_SETTINGS_SAVE_STATE_BANK)].toInt());
    std::vector<WiiMixObjective> objectives;
    QJsonArray objectives_array = obj[QStringLiteral(COMMON_SETTINGS_OBJECTIVES)].toArray();
    for (const QJsonValue& value : objectives_array) {
        objectives.push_back(WiiMixObjective::FromJson(value.toObject()));
    }
    WiiMixEnums::Difficulty difficulty = static_cast<WiiMixEnums::Difficulty>(obj[QStringLiteral(COMMON_SETTINGS_DIFFICULTY)].toInt());
    std::vector<WiiMixEnums::ObjectiveType> objective_types = WiiMixEnums::ObjectiveTypesFromString(obj[QStringLiteral(COMMON_SETTINGS_OBJECTIVE_TYPES)].toString().toStdString());
    std::vector<WiiMixEnums::GameGenre> game_genres = WiiMixEnums::GameGenresFromString(obj[QStringLiteral(COMMON_SETTINGS_GAME_GENRES)].toString().toStdString());
    return WiiMixCommonSettings(difficulty, bank, objectives, objective_types, game_genres);
}

WiiMixEnums::Difficulty WiiMixCommonSettings::GetDifficulty() const {
    return m_difficulty;
}

// int WiiMixCommonSettings::GetTime() {
//     return m_time;
// }

WiiMixEnums::SaveStateBank WiiMixCommonSettings::GetSaveStateBank() const {
    return m_save_state_bank;
}

std::vector<WiiMixObjective> WiiMixCommonSettings::GetObjectives() const {
    return m_objectives;
}

std::vector<WiiMixEnums::ObjectiveType> WiiMixCommonSettings::GetObjectiveTypes() const {
    // Assuming m_objective_types is a member variable of type WiiMixEnums::ObjectiveType
    return m_objective_types;
}

std::vector<WiiMixEnums::GameGenre> WiiMixCommonSettings::GetGameGenres() const {
    // Assuming m_game_genres is a member variable of type WiiMixEnums::GameGenre
    return m_game_genres;
}

void WiiMixCommonSettings::SetObjectiveTypes(std::vector<WiiMixEnums::ObjectiveType> types) {
    m_objective_types = types;
}

void WiiMixCommonSettings::SetGameGenres(std::vector<WiiMixEnums::GameGenre> genres) {
    m_game_genres = genres;
}