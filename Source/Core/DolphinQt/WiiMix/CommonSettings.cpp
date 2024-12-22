#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

#include "DolphinQt/WiiMix/CommonSettings.h"

#include "DolphinQt/WiiMix/Enums.h"

WiiMixCommonSettings::WiiMixCommonSettings(WiiMixEnums::Difficulty difficulty, WiiMixEnums::SaveStateBank bank,
    std::vector<WiiMixObjective> objectives) : m_difficulty(difficulty), m_save_state_bank(bank), m_objectives(objectives) {
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
        case WiiMixEnums::Difficulty::NORMAL:
            return QStringLiteral("Normal");
        case WiiMixEnums::Difficulty::HARD:
            return QStringLiteral("Hard");
        case WiiMixEnums::Difficulty::WIISANITY:
            return QStringLiteral("Wiisanity");
        default:
            return QStringLiteral("");
    }
}

WiiMixEnums::Difficulty WiiMixCommonSettings::StringToDifficulty(QString difficulty) {
    if (difficulty == QStringLiteral("Normal"))
        return WiiMixEnums::Difficulty::NORMAL;
    else if (difficulty == QStringLiteral("Hard"))
        return WiiMixEnums::Difficulty::HARD;
    else if (difficulty == QStringLiteral("Wiisanity"))
        return WiiMixEnums::Difficulty::WIISANITY;
    else
        return WiiMixEnums::Difficulty::NORMAL; // Default case
}

QString WiiMixCommonSettings::SaveStateBankToString(WiiMixEnums::SaveStateBank bank) {
    switch (bank)
    {
        case WiiMixEnums::SaveStateBank::USER:
            return QStringLiteral("User");
        case WiiMixEnums::SaveStateBank::VERIFIED:
            return QStringLiteral("Verified");
        case WiiMixEnums::SaveStateBank::UNVERIFIED:
            return QStringLiteral("Unverified");
        default:
            return QStringLiteral("");
    }
}

WiiMixEnums::SaveStateBank WiiMixCommonSettings::StringToSaveStateBank(QString bank) {
    if (bank == QStringLiteral("User"))
        return WiiMixEnums::SaveStateBank::USER;
    else if (bank == QStringLiteral("Verified"))
        return WiiMixEnums::SaveStateBank::VERIFIED;
    else if (bank == QStringLiteral("Unverified"))
        return WiiMixEnums::SaveStateBank::UNVERIFIED;
    else
        return WiiMixEnums::SaveStateBank::USER; // Default case
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
    json[QStringLiteral(COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK)] = static_cast<int>(GetSaveStateBank());
    std::vector<WiiMixObjective> objectives = GetObjectives();
    if (objectives.size() > 0) {
        QJsonArray selected_objectives;
        #ifndef WIN32   
            qDebug() << "Win32";
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(objectives.begin(), objectives.end(), g);
        #else
            qDebug() << "random_shuffle";
            std::random_shuffle(objectives.begin(), objectives.end());
        #endif
        qDebug() << "append objectives";
        for (int i = 0; i < 9; i++) {
            selected_objectives.append(objectives[i].ToJson());
        }
        json[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)] = selected_objectives;
    }
    else {
        json[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)] = {};
    }
    json[QStringLiteral(COMMON_NETPLAY_SETTINGS_DIFFICULTY)] = static_cast<int>(GetDifficulty());
    return json;
}

WiiMixCommonSettings WiiMixCommonSettings::FromJsonCommon(QJsonDocument settings_json) {
    QJsonObject obj = settings_json.object();
    WiiMixEnums::SaveStateBank bank = static_cast<WiiMixEnums::SaveStateBank>(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK)].toInt());
    std::vector<WiiMixObjective> objectives;
    QJsonArray objectives_array = obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)].toArray();
    for (const QJsonValue& value : objectives_array) {
        objectives.push_back(WiiMixObjective::FromJson(value.toObject()));
    }
    WiiMixEnums::Difficulty difficulty = static_cast<WiiMixEnums::Difficulty>(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_DIFFICULTY)].toInt());
    return WiiMixCommonSettings(difficulty, bank, objectives);
}

const WiiMixEnums::Difficulty WiiMixCommonSettings::GetDifficulty() const {
    return m_difficulty;
}

// int WiiMixCommonSettings::GetTime() {
//     return m_time;
// }

const WiiMixEnums::SaveStateBank WiiMixCommonSettings::GetSaveStateBank() const {
    return m_save_state_bank;
}

const std::vector<WiiMixObjective> WiiMixCommonSettings::GetObjectives() const {
    return m_objectives;
}
