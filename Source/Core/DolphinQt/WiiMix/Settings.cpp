#include <QString>
#include "DolphinQt/WiiMix/Settings.h"
#include "DolphinQt/Resources.h"

WiiMixSettings::WiiMixSettings(Difficulty difficulty, Mode mode, int time, 
    std::vector<WiiMixObjective> objectives, std::vector<UICommon::GameFile> games) {
    m_difficulty = difficulty;
    m_mode = mode;
    m_time = time;
    m_objectives = objectives;
    m_games = games;
}

QString WiiMixSettings::DifficultyToString(Difficulty difficulty) {
    switch (difficulty)
    {
        case Difficulty::NORMAL:
            return QStringLiteral("Normal");
        case Difficulty::HARD:
            return QStringLiteral("Hard");
        case Difficulty::WIISANITY:
            return QStringLiteral("Wiisanity");
        default:
            return QStringLiteral("");
    }
}

WiiMixSettings::Difficulty WiiMixSettings::StringToDifficulty(QString difficulty) {
    if (difficulty == QStringLiteral("Normal"))
        return WiiMixSettings::Difficulty::NORMAL;
    else if (difficulty == QStringLiteral("Hard"))
        return WiiMixSettings::Difficulty::HARD;
    else if (difficulty == QStringLiteral("Wiisanity"))
        return WiiMixSettings::Difficulty::WIISANITY;
    else
        return WiiMixSettings::Difficulty::NORMAL; // Default case
}

QString WiiMixSettings::SaveStateBankToString(SaveStateBank bank) {
    switch (bank)
    {
        case SaveStateBank::USER:
            return QStringLiteral("User");
        case SaveStateBank::VERIFIED:
            return QStringLiteral("Verified");
        case SaveStateBank::UNVERIFIED:
            return QStringLiteral("Unverified");
        default:
            return QStringLiteral("");
    }
}

WiiMixSettings::SaveStateBank WiiMixSettings::StringToSaveStateBank(QString bank) {
    if (bank == QStringLiteral("User"))
        return WiiMixSettings::SaveStateBank::USER;
    else if (bank == QStringLiteral("Verified"))
        return WiiMixSettings::SaveStateBank::VERIFIED;
    else if (bank == QStringLiteral("Unverified"))
        return WiiMixSettings::SaveStateBank::UNVERIFIED;
    else
        return WiiMixSettings::SaveStateBank::USER; // Default case
}

QString WiiMixSettings::ModeToTitle(Mode mode) {
    switch (mode)
    {
        case Mode::BINGO:
            return QStringLiteral("Bingo");
        case Mode::SHUFFLE:
            return QStringLiteral("Shuffle");
        case Mode::ROGUE:
            return QStringLiteral("Rogue");    
        default:
            return QStringLiteral("");
    }
}

QIcon WiiMixSettings::ModeToIcon(Mode mode) {
    switch (mode)
    {
        case Mode::BINGO:
            return Resources::GetResourceIcon("wiimix_bingo");
        case Mode::SHUFFLE:
            return Resources::GetResourceIcon("wiimix_shuffle");
        case Mode::ROGUE:
            return Resources::GetResourceIcon("wiimix_rogue");
        default:
            return QIcon();
    }
}

QString WiiMixSettings::ModeToDescription(Mode mode) {
    switch (mode)
    {
        case Mode::BINGO:
            return QStringLiteral("Compete over netplay to be the first to complete objectives on a bingo board!");
        case Mode::SHUFFLE:
            return QStringLiteral("Shuffle between different states in different games!");
        case Mode::ROGUE:
            return QStringLiteral("The Wii is going rogue! Attempt a series of increasingly difficult objectives with increasing difficulty, tough decisions, and helpful items. Can you make it to the end with only one life?");
        default:
            return QStringLiteral("");
    }
}

WiiMixSettings::Mode WiiMixSettings::StringToMode(QString mode) {
    if (mode == QStringLiteral("Bingo"))
        return Mode::BINGO;
    else if (mode == QStringLiteral("Shuffle"))
        return Mode::SHUFFLE;
    else if (mode == QStringLiteral("Rogue"))
        return Mode::ROGUE;
    else
        return Mode::BINGO; // Default case
}

int WiiMixSettings::StringToCardSize(QString size) {
    bool ok;
    int num = size.toInt(&ok);
    // if (!ok)
        // return 0; // Default case if conversion fails
    return num * num;
}

void WiiMixSettings::SetSaveStateBank(SaveStateBank bank) {
    m_save_state_bank = bank;
}

void WiiMixSettings::SetDifficulty(Difficulty difficulty) {
    m_difficulty = difficulty;
}

// void WiiMixSettings::SetTime(int time) {
//     m_time = time;
// }

void WiiMixSettings::SetMode(Mode mode) {
    m_mode = mode;
}

void WiiMixSettings::SetGamesList(std::vector<UICommon::GameFile> game_list) {
    m_games = game_list;
}

void WiiMixSettings::AddGame(UICommon::GameFile game) {
    m_games.push_back(game);
}

void WiiMixSettings::SetObjectives(std::vector<WiiMixObjective> objectives) {
    m_objectives = objectives;
}

void WiiMixSettings::AddObjective(WiiMixObjective objective) {
    m_objectives.push_back(objective);
}

WiiMixSettings::Mode WiiMixSettings::GetMode() {
    return m_mode;
}

WiiMixSettings::Difficulty WiiMixSettings::GetDifficulty() {
    return m_difficulty;
}

// int WiiMixSettings::GetTime() {
//     return m_time;
// }

WiiMixSettings::SaveStateBank WiiMixSettings::GetSaveStateBank() {
    return m_save_state_bank;
}

std::vector<WiiMixObjective> WiiMixSettings::GetObjectives() {
    return m_objectives;
}

std::vector<UICommon::GameFile> WiiMixSettings::GetGamesList() {
    return m_games;
}