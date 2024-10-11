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