#include <QString>

#include "DolphinQt/WiiMix/Settings.h"

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/Settings.h"
#include "DolphinQt/Resources.h"
#include "Settings.h"

QString WiiMixSettings::DifficultyToString(WiiMixEnums::Difficulty difficulty) {
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

WiiMixEnums::Difficulty WiiMixSettings::StringToDifficulty(QString difficulty) {
    if (difficulty == QStringLiteral("Normal"))
        return WiiMixEnums::Difficulty::NORMAL;
    else if (difficulty == QStringLiteral("Hard"))
        return WiiMixEnums::Difficulty::HARD;
    else if (difficulty == QStringLiteral("Wiisanity"))
        return WiiMixEnums::Difficulty::WIISANITY;
    else
        return WiiMixEnums::Difficulty::NORMAL; // Default case
}

QString WiiMixSettings::SaveStateBankToString(WiiMixEnums::SaveStateBank bank) {
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

WiiMixEnums::SaveStateBank WiiMixSettings::StringToSaveStateBank(QString bank) {
    if (bank == QStringLiteral("User"))
        return WiiMixEnums::SaveStateBank::USER;
    else if (bank == QStringLiteral("Verified"))
        return WiiMixEnums::SaveStateBank::VERIFIED;
    else if (bank == QStringLiteral("Unverified"))
        return WiiMixEnums::SaveStateBank::UNVERIFIED;
    else
        return WiiMixEnums::SaveStateBank::USER; // Default case
}

QString WiiMixSettings::BingoTypeToString(WiiMixEnums::BingoType type) {
    switch (type)
    {
        case WiiMixEnums::BingoType::BINGO:
            return QStringLiteral("Bingo");
        case WiiMixEnums::BingoType::LOCKOUT:
            return QStringLiteral("Lockout");
        case WiiMixEnums::BingoType::TIME_ATTACK:
            return QStringLiteral("Time Attack");
        default:
            return QStringLiteral("");
    }
}

WiiMixEnums::BingoType WiiMixSettings::StringToBingoType(QString type) {
    if (type == QStringLiteral("Bingo"))
        return WiiMixEnums::BingoType::BINGO;
    else if (type == QStringLiteral("Lockout"))
        return WiiMixEnums::BingoType::LOCKOUT;
    else if (type == QStringLiteral("Time Attack"))
        return WiiMixEnums::BingoType::TIME_ATTACK;
    else
        return WiiMixEnums::BingoType::END; // Default case
}

QString WiiMixSettings::ModeToTitle(WiiMixEnums::Mode mode) {
    switch (mode)
    {
        case WiiMixEnums::Mode::BINGO:
            return QStringLiteral("Bingo");
        case WiiMixEnums::Mode::SHUFFLE:
            return QStringLiteral("Shuffle");
        case WiiMixEnums::Mode::ROGUE:
            return QStringLiteral("Rogue");
        default:
            return QStringLiteral("");
    }
}

QIcon WiiMixSettings::ModeToIcon(WiiMixEnums::Mode mode) {
    switch (mode)
    {
        case WiiMixEnums::Mode::BINGO:
            return Resources::GetResourceIcon("wiimix_bingo");
        case WiiMixEnums::Mode::SHUFFLE:
            return Resources::GetResourceIcon("wiimix_shuffle");
        case WiiMixEnums::Mode::ROGUE:
            return Resources::GetResourceIcon("wiimix_rogue");
        default:
            return QIcon();
    }
}

QString WiiMixSettings::ModeToDescription(WiiMixEnums::Mode mode) {
    switch (mode)
    {
        case WiiMixEnums::Mode::BINGO:
            return QStringLiteral("Compete over netplay to be the first to complete objectives on a bingo board!");
        case WiiMixEnums::Mode::SHUFFLE:
            return QStringLiteral("Shuffle between different states in different games!");
        case WiiMixEnums::Mode::ROGUE:
            return QStringLiteral("The Wii is going rogue! Attempt to complete a series of increasingly difficult objectives with only one life!");
        default:
            return QStringLiteral("");
    }
}

WiiMixEnums::Mode WiiMixSettings::StringToMode(QString mode) {
    if (mode == QStringLiteral("Bingo"))
        return WiiMixEnums::Mode::BINGO;
    else if (mode == QStringLiteral("Shuffle"))
        return WiiMixEnums::Mode::SHUFFLE;
    else if (mode == QStringLiteral("Rogue"))
        return WiiMixEnums::Mode::ROGUE;
    else
        return WiiMixEnums::Mode::BINGO; // Default case
}

int WiiMixSettings::StringToCardSize(QString size) {
    bool ok;
    int num = size.toInt(&ok);
    // if (!ok)
        // return 0; // Default case if conversion fails
    return num * num;
}

std::vector<UICommon::GameFile> WiiMixSettings::GameIdsToGameFiles(std::string game_ids_list) {
    std::vector<UICommon::GameFile> game_files;
    std::istringstream stream(game_ids_list);
    std::string game_id;
    while (std::getline(stream, game_id, ',')) {
        // TODOx: might be able to rework this by checking the game_id against the game config files
        // It's unlikely that anyone would manually edit their config files to include a game that they don't have
        UICommon::GameFile *game = new UICommon::GameFile();
        UICommon::GameFile::GetGameFileById(game_id, game);
        if (game->IsValid()) {
            game_files.push_back(*game);
        }
        else {
            // Error out if the game is not found
            return {};
        }
    }
    return game_files;
}

// TODO
std::vector<WiiMixObjective> WiiMixSettings::ObjectiveIdsToObjectives(std::string objective_ids_list) {
    // std::vector<WiiMixObjective> objectives;
    // std::istringstream stream(objective_ids_list);
    // std::string objective_id;
    // while (std::getline(stream, objective_id, ',')) {
    //     WiiMixObjective objective = WiiMixObjective::GetObjectiveById(objective_id);
    //     if (objective.IsValid()) {
    //         objectives.push_back(objective);
    //     }
    // }
    // return objectives;
    return {};
}

std::string WiiMixSettings::GameFilesToGameIds(std::vector<UICommon::GameFile> games) {
    std::string game_ids_list = "";
    for (size_t i = 0; i < games.size(); ++i) {
        game_ids_list += games[i].GetGameID();
        if (i + 1 != games.size()) {
            game_ids_list += ",";
        }
    }
    return game_ids_list;
}

std::string WiiMixSettings::ObjectivesToObjectiveIds(std::vector<WiiMixObjective> objectives) {
    std::string objective_ids_list = "";
    for (size_t i = 0; i < objectives.size(); ++i) {
        objective_ids_list += objectives[i].GetObjectiveString();
        if (i + 1 != objectives.size()) {
            objective_ids_list += ",";
        }
    }
    return objective_ids_list;
}

WiiMixSettings::WiiMixSettings(WiiMixEnums::Difficulty difficulty, WiiMixEnums::Mode mode, WiiMixEnums::SaveStateBank bank,
    std::vector<WiiMixObjective> objectives, std::vector<UICommon::GameFile> games) {
    WiiMixEnums::Difficulty config_difficulty = Config::Get(Config::WIIMIX_DIFFICULTY);
    if (difficulty != DEFAULT_DIFFICULTY) {
        m_difficulty = difficulty;
    }
    else if (config_difficulty != DEFAULT_DIFFICULTY) {
        m_difficulty = config_difficulty;
    }
    else {
        m_difficulty = DEFAULT_DIFFICULTY;
    }
    WiiMixEnums::Mode config_mode = Config::Get(Config::WIIMIX_MODE);
    if (mode != DEFAULT_MODE) {
        m_mode = mode;
    }
    else if (config_mode != DEFAULT_MODE) {
        m_mode = config_mode;
    }
    else {
        m_mode = DEFAULT_MODE;
    }

    WiiMixEnums::SaveStateBank config_bank = Config::Get(Config::WIIMIX_SAVE_STATE_BANK);
    if (bank != DEFAULT_SAVE_STATE_BANK) {
        m_save_state_bank = bank;
    }
    else if (config_bank != DEFAULT_SAVE_STATE_BANK) {
        m_save_state_bank = config_bank;
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
    std::vector<WiiMixObjective> config_objectives = WiiMixSettings::ObjectiveIdsToObjectives(Config::Get(Config::WIIMIX_OBJECTIVE_IDS));
    if (objectives.size() != 0) {
        m_objectives = objectives;
    }
    else if (config_objectives.size() != 0) {
        m_objectives = objectives;
    }
    else {
        m_objectives = DEFAULT_OBJECTIVES;
    }
    // Games is set whenever it needs to be set. That means:
    // - Before loading objectives for WiiMix
    // - Pressing save
    // - Loading someone else's games (errors out if you don't have the compatible games)
    std::vector<UICommon::GameFile> config_games = WiiMixSettings::GameIdsToGameFiles(Config::Get(Config::WIIMIX_GAME_IDS));
    if (games.size() != 0) {
        m_games = games;
    }
    else if (config_games.size() != 0) {
        m_games = config_games;
    }
    else {
        m_games = DEFAULT_GAMES;
    }
}

void WiiMixSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank bank) {
    m_save_state_bank = bank;
    Settings::GetQSettings().setValue(QStringLiteral("WiiMix/SaveStateBank"), static_cast<int>(bank));
}

void WiiMixSettings::SetDifficulty(WiiMixEnums::Difficulty difficulty) {
    m_difficulty = difficulty;
    Settings::GetQSettings().setValue(QStringLiteral("WiiMix/Difficulty"), static_cast<int>(difficulty));
}

// void WiiMixSettings::SetTime(int time) {
//     m_time = time;
// }

void WiiMixSettings::SetMode(WiiMixEnums::Mode mode) {
    m_mode = mode;
    Settings::GetQSettings().setValue(QStringLiteral("WiiMix/Mode"), static_cast<int>(mode));
}

void WiiMixSettings::SetGamesList(std::vector<UICommon::GameFile> game_list) {
    m_games = game_list;
    Settings::GetQSettings().setValue(QStringLiteral("WiiMix/Games"), QString::fromStdString(GameFilesToGameIds(m_games)));
}

void WiiMixSettings::AddGame(UICommon::GameFile game) {
    m_games.push_back(game);
    std::string game_ids_list = Config::Get(Config::WIIMIX_GAME_IDS);
    if (!game_ids_list.empty())
        game_ids_list += ", ";
    game_ids_list += game.GetGameID();
    Settings::GetQSettings().setValue(QStringLiteral("WiiMix/Games"), QString::fromStdString(game_ids_list));
}

void WiiMixSettings::SetObjectives(std::vector<WiiMixObjective> objectives) {
    m_objectives = objectives;
    Settings::GetQSettings().setValue(QStringLiteral("WiiMix/Objectives"), QString::fromStdString(ObjectivesToObjectiveIds(m_objectives)));
}

void WiiMixSettings::AddObjective(WiiMixObjective objective) {
    m_objectives.push_back(objective);
    std::string objective_ids_list = Config::Get(Config::WIIMIX_OBJECTIVE_IDS);
    if (!objective_ids_list.empty())
        objective_ids_list += ", ";
    objective_ids_list += objective.GetObjectiveString();
    Settings::GetQSettings().setValue(QStringLiteral("WiiMix/Objectives"), QString::fromStdString(objective_ids_list));
}

WiiMixEnums::Mode WiiMixSettings::GetMode() {
    return m_mode;
}

WiiMixEnums::Difficulty WiiMixSettings::GetDifficulty() {
    return m_difficulty;
}

// int WiiMixSettings::GetTime() {
//     return m_time;
// }

WiiMixEnums::SaveStateBank WiiMixSettings::GetSaveStateBank() {
    return m_save_state_bank;
}

std::vector<WiiMixObjective> WiiMixSettings::GetObjectives() {
    return m_objectives;
}

std::vector<UICommon::GameFile> WiiMixSettings::GetGamesList() {
    return m_games;
}
