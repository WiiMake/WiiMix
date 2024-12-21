#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>
#include "DolphinQt/GameList/GameTracker.h"

#include "DolphinQt/WiiMix/Settings.h"

#include "DolphinQt/WiiMix/Enums.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

#include "DolphinQt/Settings.h"
#include "DolphinQt/Resources.h"
#include "Core/Core.h"
#include "Core/System.h"
#include "Common/FileUtil.h"
#include "Common/IniFile.h"
#include "Core/ConfigManager.h"

WiiMixSettings::WiiMixSettings(WiiMixEnums::Difficulty difficulty, WiiMixEnums::Mode mode, WiiMixEnums::SaveStateBank bank,
    std::vector<WiiMixObjective> objectives, std::vector<std::shared_ptr<const UICommon::GameFile>> games) : m_difficulty(difficulty), m_mode(mode), m_save_state_bank(bank), m_objectives(objectives), m_games(games) {
    if (difficulty != DEFAULT_DIFFICULTY) {
        m_difficulty = difficulty;
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
    // std::vector<WiiMixObjective> config_objectives = WiiMixSettings::ObjectiveIdsToObjectives(Config::Get(Config::WIIMIX_OBJECTIVE_IDS));
    // if (objectives.size() != 0) {
    //     m_objectives = objectives;
    // }
    // else if (config_objectives.size() != 0) {
    //     m_objectives = objectives;
    // }
    // else {
    m_objectives = DEFAULT_OBJECTIVES;
    // }
    // Games is set whenever it needs to be set. That means:
    // - Before loading objectives for WiiMix
    // - Pressing save
    // - Loading someone else's games (errors out if you don't have the compatible games)
    // std::vector<UICommon::GameFile> config_games = WiiMixSettings::GameIdsToGameFiles(Config::Get(Config::WIIMIX_GAME_IDS));
    // if (games.size() != 0) {
    //     m_games = games;
    // }
    // else if (config_games.size() != 0) {
    //     m_games = config_games;
    // }
    // else {
    m_games = DEFAULT_GAMES;
    // }

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

// This is only used for sending json back and forth
std::vector<std::shared_ptr<const UICommon::GameFile>> WiiMixSettings::GameIdsToGameFiles(std::string game_ids_list) {
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

std::string WiiMixSettings::GameFilesToGameIds(std::vector<std::shared_ptr<const UICommon::GameFile>> games) {
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
void WiiMixSettings::SetGamesList(std::vector<std::shared_ptr<const UICommon::GameFile>> game_list) {
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

void WiiMixSettings::AddGame(const std::shared_ptr<const UICommon::GameFile>& game) {
    // Only make changes if the system is not running or starting
    if (!Core::IsRunning(Core::System::GetInstance()))
        return;

    m_games.push_back(game);
    
    // NOTE: the GUI does not need to be rebuilt here because the game tracker in GameListModel emits a signal to build the GUI already
    // This is just also connected to that signal to have the data all in one place
}

void WiiMixSettings::UpdateGame(const std::shared_ptr<const UICommon::GameFile>& game)
{
    // Only make changes if the system is not running or starting
    if (!Core::IsRunning(Core::System::GetInstance()))
        return;

    int index = WiiMixSettings::FindGameIndex(game->GetFilePath());
    if (index < 0)
    {
        WiiMixSettings::AddGame(game);
    }
    else
    {
        m_games[index] = game;
    }

    // NOTE: the GUI does not need to be rebuilt here because the game tracker in GameListModel emits a signal to build the GUI already
    // This is just also connected to that signal to have the data all in one place
}

void WiiMixSettings::RemoveGame(const std::string& path)
{
    // Only make changes if the system is not running or starting
    if (!Core::IsRunning(Core::System::GetInstance()))
        return;
        
    int entry = WiiMixSettings::FindGameIndex(path);
    if (entry < 0)
        return;
    
    // TODOx: not sure if the code below works
    // Should just delete a single element
    m_games.erase(m_games.begin() + entry);

    // NOTE: the GUI does not need to be rebuilt here because the game tracker in GameListModel emits a signal to build the GUI already
    // This is just also connected to that signal to have the data all in one place
}

int WiiMixSettings::FindGameIndex(const std::string& path) const
{
    for (int i = 0; i < m_games.size(); i++)
    {
        if (m_games[i]->GetFilePath() == path)
        return i;
    }
    return -1;
}


const std::vector<std::shared_ptr<const UICommon::GameFile>> WiiMixSettings::GetGamesList() const {
    return m_games;
}

// These methods are used in GUI, hence the Paschal Case
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
    int num = 0;
    if (!size.isEmpty()) {
        num = size[0].digitValue();
    }
    return num * num;
}

void WiiMixSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank bank) {
    m_save_state_bank = bank;
    // switch (GetMode())
    // {
    //     case WiiMixEnums::Mode::BINGO:
    //         Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_SAVE_STATE_BANK, bank);
    //         break;
    //     case WiiMixEnums::Mode::SHUFFLE:
    //         Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_SAVE_STATE_BANK, bank);
    //         break;
    //     case WiiMixEnums::Mode::ROGUE:
    //         Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_SAVE_STATE_BANK, bank);
    //         break;
    //     default:
    //         break;
    // }
    // emit SettingsChanged(bank);
}

void WiiMixSettings::SetDifficulty(WiiMixEnums::Difficulty difficulty) {
    m_difficulty = difficulty;
    // switch (GetMode())
    // {
    //     case WiiMixEnums::Mode::BINGO:
    //         Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_DIFFICULTY, difficulty);
    //         break;
    //     case WiiMixEnums::Mode::SHUFFLE:
    //         Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_DIFFICULTY, difficulty);
    //         break;
    //     case WiiMixEnums::Mode::ROGUE:
    //         Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_DIFFICULTY, difficulty);
    //         break;
    //     default:
    //         break;
    // }
    // emit SettingsChanged(difficulty);
}

// void WiiMixSettings::SetTime(int time) {
//     m_time = time;
// }

void WiiMixSettings::SetMode(WiiMixEnums::Mode mode) {
    m_mode = mode;
    Config::Set(Config::LayerType::Base, Config::WIIMIX_MODE, mode);
    // emit SettingsChanged(mode);
}

void WiiMixSettings::SetObjectives(std::vector<WiiMixObjective> objectives) {
    m_objectives = objectives;
    // Note that objectives get populated once a WiiMix starts, not during the initial load
    // So nothing is stored in config
    // This also serves to minimize retroachievements and wiimix server requests
}

void WiiMixSettings::AddObjective(WiiMixObjective objective) {
    m_objectives.push_back(objective);
}

QJsonObject WiiMixSettings::ToJsonCommon() {
    QJsonObject json;
    json[QStringLiteral(COMMON_NETPLAY_SETTINGS_MODE)] = static_cast<int>(GetMode());
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
        selected_objectives.append(objectives[i].ToJson().object());
        }
        json[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)] = selected_objectives;
    }
    else {
        json[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)] = {};
    }
    json[QStringLiteral(COMMON_NETPLAY_SETTINGS_DIFFICULTY)] = static_cast<int>(GetDifficulty());
    #ifdef QT_GUI_LIB
        json[QStringLiteral(COMMON_NETPLAY_SETTINGS_GAMES_LIST)] = QString::fromStdString(GameFilesToGameIds(GetGamesList()));
    #else
        json[QStringLiteral(COMMON_NETPLAY_SETTINGS_GAMES_LIST)] = QString::fromStdString(GetGamesList());
    #endif
    return json;
}

void WiiMixSettings::FromJsonCommon(QJsonDocument settings_json) {
    QJsonObject obj = settings_json.object();
    SetMode(static_cast<WiiMixEnums::Mode>(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK)].toInt()));
    SetSaveStateBank(static_cast<WiiMixEnums::SaveStateBank>(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK)].toInt()));
    // SetObjectives(WiiMixSettings::ObjectiveIdsToObjectives(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)].toString().toStdString()));
    std::vector<WiiMixObjective> objectives = {};
    QJsonArray objectives_array = obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)].toArray();
    for (const QJsonValue& value : objectives_array) {
        objectives.push_back(WiiMixObjective::FromJson(QJsonDocument(value.toObject())));
    }
    SetObjectives(objectives);
    SetDifficulty(static_cast<WiiMixEnums::Difficulty>(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_DIFFICULTY)].toInt()));
    SetGamesList(WiiMixSettings::GameIdsToGameFiles(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_GAMES_LIST)].toString().toStdString()));
    // #else
        // SetGamesList(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_GAMES_LIST)].toString().toStdString());
    return;
}

const WiiMixEnums::Mode WiiMixSettings::GetMode() const {
    return m_mode;
}

const WiiMixEnums::Difficulty WiiMixSettings::GetDifficulty() const {
    return m_difficulty;
}

// int WiiMixSettings::GetTime() {
//     return m_time;
// }

const WiiMixEnums::SaveStateBank WiiMixSettings::GetSaveStateBank() const {
    return m_save_state_bank;
}

const std::vector<WiiMixObjective> WiiMixSettings::GetObjectives() const {
    return m_objectives;
}
