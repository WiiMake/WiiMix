#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>
#include "DolphinQt/GameList/GameTracker.h"

#include "DolphinQt/WiiMix/GlobalSettings.h"

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Player.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

#include "DolphinQt/Settings.h"
#include "DolphinQt/Resources.h"
#include "Core/Core.h"
#include "Core/System.h"
#include "Common/FileUtil.h"
#include "Common/IniFile.h"
#include "Core/ConfigManager.h"

WiiMixGlobalSettings::WiiMixGlobalSettings(WiiMixEnums::Mode mode, std::vector<std::shared_ptr<const UICommon::GameFile>> games, int current_objective, WiiMixPlayer *player, std::vector<int> scores) : m_mode(mode), m_games(games), m_current_objective(current_objective), m_player(player), m_objective_scores(scores) {
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
    m_games = DEFAULT_GAMES;
    m_current_objective = 0;
    std::string player_username = Config::Get(Config::WIIMIX_PLAYER_USERNAME);
    if (player_username != "") {
        // Password is not stored in the config file
        // It is only required for the initial login
        // Id also does not need to be stored in the config file, as usernames are unique
        int num_objectives_completed = Config::Get(Config::WIIMIX_PLAYER_NUM_OBJECTIVES_COMPLETED);
        int num_unique_objectives_completed = Config::Get(Config::WIIMIX_PLAYER_NUM_UNIQUE_OBJECTIVES_COMPLETED);
        int num_objectives_attempted = Config::Get(Config::WIIMIX_PLAYER_NUM_OBJECTIVES_ATTEMPTED);
        m_player = new WiiMixPlayer(-1, player_username, "", WiiMixEnums::Role::USER, num_objectives_completed, num_unique_objectives_completed, num_objectives_attempted);
    }
    else {
        m_player = nullptr;
    }
}

QIcon WiiMixGlobalSettings::ModeToIcon(WiiMixEnums::Mode mode) {
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

// This is only used for sending json back and forth
std::vector<std::shared_ptr<const UICommon::GameFile>> WiiMixGlobalSettings::GameIdsToGameFiles(std::string game_ids_list) {
    std::vector<std::shared_ptr<const UICommon::GameFile>> game_files;
    std::istringstream stream(game_ids_list);
    std::string game_id;
    while (std::getline(stream, game_id, ',')) {
        UICommon::GameFile *game = new UICommon::GameFile();
        UICommon::GameFile::GetGameFileById(game_id, game);
        if (game->IsValid()) {
            game_files.push_back(std::shared_ptr<const UICommon::GameFile>(game));
        }
        else {
            // Error out if the game is not found
            return {};
        }
    }
    return game_files;
}

std::string WiiMixGlobalSettings::GameFilesToGameIds(std::vector<std::shared_ptr<const UICommon::GameFile>> games) {
    std::string game_ids_list = "";
    for (size_t i = 0; i < games.size(); ++i) {
        game_ids_list += games[i]->GetGameID();
        if (i + 1 != games.size()) {
            game_ids_list += ",";
        }
    }
    return game_ids_list;
}

// This is only called when copying someone else's games configuration (i.e. for wiimix netplay)
void WiiMixGlobalSettings::SetGamesList(std::vector<std::shared_ptr<const UICommon::GameFile>> game_list) {
    m_games = game_list;

    // Iterate over the new games list and update the game config files
    for (std::shared_ptr<const UICommon::GameFile> game_ptr : m_games) {
        const UICommon::GameFile game = *(game_ptr.get());
        if (!File::Exists(File::GetUserPath(D_GAMESETTINGS_IDX) + game.GetGameID() + ".ini")) {
            // Create a new ini file and set the WiiMix state
            Common::IniFile ini = SConfig::LoadDefaultGameIni(game.GetGameID(), game.GetRevision());
            ini.GetOrCreateSection("WiiMix")->Set("WiiMix", true);
            ini.Save(File::GetUserPath(D_GAMESETTINGS_IDX) + game.GetGameID() + ".ini");
        }
        else {
            Common::IniFile ini = SConfig::LoadLocalGameIni(game.GetGameID(), game.GetRevision());
            ini.GetOrCreateSection("WiiMix")->Set("WiiMix", true);
            ini.Save(File::GetUserPath(D_GAMESETTINGS_IDX) + game.GetGameID() + ".ini");
        }
    }

    // Rebuild the game list GUI
    emit SettingsChanged(m_games);
}

void WiiMixGlobalSettings::AddGame(const std::shared_ptr<const UICommon::GameFile>& game) {
    // Only make changes if the system is not running or starting
    //if (Core::IsRunning(Core::System::GetInstance())) // TODOx: something is wrong with this. it should not be true. changed for testing. see #2
        // return;

    m_games.push_back(game);

    // NOTE: the GUI does not need to be rebuilt here because the game tracker in GameListModel emits a signal to build the GUI already
    // This is just also connected to that signal to have the data all in one place
}

void WiiMixGlobalSettings::UpdateGame(const std::shared_ptr<const UICommon::GameFile>& game)
{
    qDebug() << "Updating game: " << QString::fromStdString(game->GetGameID());
    // Only make changes if the system is not running or starting
    // if (!Core::IsRunning(Core::System::GetInstance()))
    //     return;

    int index = WiiMixGlobalSettings::FindGameIndex(game->GetFilePath());
    qDebug() << "index: " << index;
    if (index < 0)
    {
        WiiMixGlobalSettings::AddGame(game);
    }
    else
    {
        m_games[index] = game;
    }

    // NOTE: the GUI does not need to be rebuilt here because the game tracker in GameListModel emits a signal to build the GUI already
    // This is just also connected to that signal to have the data all in one place
}

void WiiMixGlobalSettings::RemoveGame(const std::string& path)
{
    // Only make changes if the system is not running or starting
    if (!Core::IsRunning(Core::System::GetInstance()))
        return;

    int entry = WiiMixGlobalSettings::FindGameIndex(path);
    if (entry < 0)
        return;

    // TODOx: not sure if the code below works
    // Should just delete a single element
    m_games.erase(m_games.begin() + entry);

    // NOTE: the GUI does not need to be rebuilt here because the game tracker in GameListModel emits a signal to build the GUI already
    // This is just also connected to that signal to have the data all in one place
}

int WiiMixGlobalSettings::FindGameIndex(const std::string& path) const
{
    for (int i = 0; i < m_games.size(); i++)
    {
        if (m_games[i]->GetFilePath() == path)
        return i;
    }
    return -1;
}

const std::vector<std::shared_ptr<const UICommon::GameFile>> WiiMixGlobalSettings::GetGamesList() const {
    return m_games;
}

QString WiiMixGlobalSettings::ModeToTitle(WiiMixEnums::Mode mode) {
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

QString WiiMixGlobalSettings::ModeToDescription(WiiMixEnums::Mode mode) {
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

WiiMixEnums::Mode WiiMixGlobalSettings::StringToMode(QString mode) {
    if (mode == QStringLiteral("Bingo"))
        return WiiMixEnums::Mode::BINGO;
    else if (mode == QStringLiteral("Shuffle"))
        return WiiMixEnums::Mode::SHUFFLE;
    else if (mode == QStringLiteral("Rogue"))
        return WiiMixEnums::Mode::ROGUE;
    else
        return WiiMixEnums::Mode::BINGO; // Default case
}

void WiiMixGlobalSettings::SetMode(WiiMixEnums::Mode mode) {
    m_mode = mode;
    Config::Set(Config::LayerType::Base, Config::WIIMIX_MODE, mode);
    // emit SettingsChanged(mode);
}

WiiMixEnums::Mode WiiMixGlobalSettings::GetMode() const {
    return m_mode;
}

void WiiMixGlobalSettings::SetCurrentObjective(int objective) {
    m_current_objective = objective;
}

int WiiMixGlobalSettings::GetCurrentObjective() {
    return m_current_objective;
}

void WiiMixGlobalSettings::SetPlayer(WiiMixPlayer *player) {
    m_player = player;
    if (m_player) {
        Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_USERNAME, player->GetUsername());
        Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_OBJECTIVES_COMPLETED, player->GetNumObjectivesCompleted());
        Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_UNIQUE_OBJECTIVES_COMPLETED, player->GetNumUniqueObjectivesCompleted());
        Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_OBJECTIVES_ATTEMPTED, player->GetNumObjectivesAttempted());
    }
}

WiiMixPlayer *WiiMixGlobalSettings::GetPlayer() {
    return m_player;
}

void WiiMixGlobalSettings::SetNumObjectivesCompleted(int num_objectives_completed) {
    if (m_player) {
        m_player->SetNumObjectivesCompleted(num_objectives_completed);
        emit onSetNumObjectivesCompleted(num_objectives_completed);
    }
}

void WiiMixGlobalSettings::SetNumUniqueObjectivesCompleted(int num_unique_objectives_completed) {
    if (m_player) {
        m_player->SetNumUniqueObjectivesCompleted(num_unique_objectives_completed);
        emit onSetNumUniqueObjectivesCompleted(num_unique_objectives_completed);
    }
}

void WiiMixGlobalSettings::SetNumObjectivesAttempted(int num_objectives_attempted) {
    if (m_player) {
        m_player->SetNumObjectivesAttempted(num_objectives_attempted);
        emit onSetNumObjectivesAttempted(num_objectives_attempted);
    }
}

void WiiMixGlobalSettings::SetNumObjectivesCreated(int num_objectives_created) {
    if (m_player) {
        m_player->SetNumObjectivesCreated(num_objectives_created);
        emit onSetNumObjectivesCreated(num_objectives_created);
    }
}

std::string WiiMixGlobalSettings::GetSaveStatePath(WiiMixObjective objective) {
    return File::GetUserPath(D_WIIMIX_STATESAVES_IDX) + objective.GetGameId() + "_" + objective.GetFileHash() + ".sav";
}

std::string WiiMixGlobalSettings::GetLiveSaveStatePath(WiiMixObjective objective) {
    return File::GetUserPath(D_WIIMIX_LIVE_STATESAVES_IDX) + objective.GetGameId() + "_" + objective.GetFileHash() + ".sav";
}

void WiiMixGlobalSettings::SetObjectiveScores(std::vector<int>& scores) {
    m_objective_scores = scores;
}

void WiiMixGlobalSettings::IncrementPlayerScore(int player_num) {
    m_objective_scores.at(player_num - 1) = m_objective_scores.at(player_num - 1) + 1;
}

std::vector<int>& WiiMixGlobalSettings::GetObjectiveScores() {
    return m_objective_scores;
}