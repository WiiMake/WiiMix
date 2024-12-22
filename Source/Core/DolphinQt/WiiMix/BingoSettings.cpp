#include "DolphinQt/WiiMix/BingoSettings.h"
#include "DolphinQt/WiiMix/CommonSettings.h"

#include <QJsonObject>

#include "DolphinQt/WiiMix/Enums.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

WiiMixBingoSettings::WiiMixBingoSettings(WiiMixEnums::Difficulty difficulty, WiiMixEnums::SaveStateBank save_state_bank, std::vector<WiiMixObjective> objectives, WiiMixEnums::BingoType bingo_type, int card_size, bool teams) : WiiMixCommonSettings(
    difficulty,
    save_state_bank,
    objectives
), m_bingo_type(bingo_type), m_card_size(card_size), m_teams(teams) {
    // Check if config overrides defaults
    if (bingo_type != DEFAULT_BINGO_TYPE) {
        m_bingo_type = bingo_type;
    } else if (Config::Get(Config::WIIMIX_BINGO_TYPE) != DEFAULT_BINGO_TYPE) {
        m_bingo_type = Config::Get(Config::WIIMIX_BINGO_TYPE);
    } else {
        m_bingo_type = DEFAULT_BINGO_TYPE;
    }

    if (card_size != DEFAULT_CARD_SIZE) {
        m_card_size = card_size;
    } else if (Config::Get(Config::WIIMIX_CARD_SIZE) != DEFAULT_CARD_SIZE) {
        m_card_size = Config::Get(Config::WIIMIX_CARD_SIZE);
    } else {
        m_card_size = DEFAULT_CARD_SIZE;
    }

    if (teams != DEFAULT_TEAMS) {
        m_teams = teams;
    } else if (Config::Get(Config::WIIMIX_TEAMS) != DEFAULT_TEAMS) {
        m_teams = Config::Get(Config::WIIMIX_TEAMS);
    } else {
        m_teams = DEFAULT_TEAMS;
    }
}

WiiMixEnums::BingoType WiiMixBingoSettings::GetBingoType() const
{
    return m_bingo_type;
}

void WiiMixBingoSettings::SetBingoType(WiiMixEnums::BingoType value)
{
    m_bingo_type = value;
}

int WiiMixBingoSettings::GetCardSize() const
{
    return m_card_size;
}

void WiiMixBingoSettings::SetCardSize(int value)
{
    // Check if the square root of the value is an integer
    // and if the value is odd
    int sqrt_value = static_cast<int>(sqrt(value));
    if (sqrt_value * sqrt_value == value && value % 2 != 0)
    {
        m_card_size = value;
    }
}

bool WiiMixBingoSettings::GetTeams()
{
    return m_teams;
}

void WiiMixBingoSettings::SetTeams(bool value)
{
    m_teams = value;
}

QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> WiiMixBingoSettings::GetPlayers()
{
    return m_players;
}

void WiiMixBingoSettings::AddPlayer(WiiMixEnums::Player player, QPair<WiiMixEnums::Color, QString> value)
{
    m_players[player] = value;
}

void WiiMixBingoSettings::RemovePlayer(WiiMixEnums::Player player)
{
    m_players.remove(player);
}

void WiiMixBingoSettings::SetPlayers(QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> value)
{
    m_players = value;
}

QString WiiMixBingoSettings::GetLobbyID()
{
    return m_lobby_id;
}

void WiiMixBingoSettings::SetLobbyID(QString value)
{
    m_lobby_id = value;
}

QString WiiMixBingoSettings::GetLobbyPassword()
{
    return m_lobby_password;
}

void WiiMixBingoSettings::SetLobbyPassword(QString value)
{
    m_lobby_password = value;
}

QMap<WiiMixEnums::Player, int> WiiMixBingoSettings::GetCurrentObjectives() {
    return m_current_objectives;
}

void WiiMixBingoSettings::SetCurrentObjectives(QMap<WiiMixEnums::Player, int> objectives) {
    m_current_objectives = objectives;
}

void WiiMixBingoSettings::UpdateCurrentObjectives(WiiMixEnums::Player player, int objective) {
    m_current_objectives[player] = objective;
}

QMap<WiiMixEnums::Player, bool> WiiMixBingoSettings::GetPlayersReady() {
    return m_players_ready;
}

void WiiMixBingoSettings::SetPlayersReady(QMap<WiiMixEnums::Player, bool> players_ready) {
    m_players_ready = players_ready;
}

void WiiMixBingoSettings::UpdatePlayerReady(WiiMixEnums::Player player, bool ready) {
    m_players_ready[player] = ready;
}

QJsonDocument WiiMixBingoSettings::ToJson()
{
    // Take care of the common settings first
    QJsonObject json = ToJsonCommon();
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_BINGO_TYPE)] = static_cast<int>(m_bingo_type);
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_TEAMS)] = m_teams;
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_CARD_SIZE)] = m_card_size;
    QVariantMap players_variant;
    QVariantMap current_objectives_variant;
    QVariantMap players_ready_variant;
    for (auto it = m_players.begin(); it != m_players.end(); ++it)
    {
        QVariantMap player_info;
        player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_COLOR)] = static_cast<int>(std::get<0>(it.value()));
        player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_NAME)] = std::get<1>(it.value());
        qDebug() << "players_variant";
        players_variant[QString::number(static_cast<int>(it.key()))] = player_info;
        qDebug() << "players_ready_variant";
        players_ready_variant[QString::number(static_cast<int>(it.key()))] = m_players_ready[it.key()];
        qDebug() << "curr_obj_variant";
        current_objectives_variant[QString::number(static_cast<int>(it.key()))] = m_current_objectives[it.key()];
    }
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS)] = QJsonDocument::fromVariant(players_variant).object();
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_CURRENT_OBJECTIVES)] = QJsonDocument::fromVariant(current_objectives_variant).object();
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS_READY)] = QJsonDocument::fromVariant(players_ready_variant).object();
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)] = m_lobby_id;
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD)] = m_lobby_password;

    return QJsonDocument(json);
}

void WiiMixBingoSettings::FromJson(QJsonDocument json)
{
    // Take care of the common settings first
    FromJsonCommon(json);
    QJsonObject obj = json.object();
    SetBingoType(static_cast<WiiMixEnums::BingoType>(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_BINGO_TYPE)].toInt()));
    SetTeams(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_TEAMS)].toBool());
    SetCardSize(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_CARD_SIZE)].toInt());

    QJsonObject players_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS)].toObject();
    for (auto it = players_variant.begin(); it != players_variant.end(); ++it)
    {
        QJsonObject player_info = it.value().toObject();
        WiiMixEnums::Color color = static_cast<WiiMixEnums::Color>(player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_COLOR)].toInt());
        QString name = player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_NAME)].toString();
        m_players[static_cast<WiiMixEnums::Player>(it.key().toInt())] = QPair<WiiMixEnums::Color, QString>(color, name);
    }
    qDebug() << "current_objectives_variant";
    QJsonObject current_objectives_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_CURRENT_OBJECTIVES)].toObject();
    for (auto it = current_objectives_variant.begin(); it != current_objectives_variant.end(); ++it) {
        m_current_objectives[static_cast<WiiMixEnums::Player>(it.key().toInt())] = it.value().toInt();
    }
    qDebug() << "players_ready_variant";
    QJsonObject players_ready_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS_READY)].toObject();
    for (auto it = players_ready_variant.begin(); it != players_ready_variant.end(); ++it) {
        m_players_ready[static_cast<WiiMixEnums::Player>(it.key().toInt())] = it.value().toBool();
    }

    m_lobby_id = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)].toString();
    m_lobby_password = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD)].toString();
}

QString WiiMixBingoSettings::GetSeed() {
    return m_seed;
}

void WiiMixBingoSettings::SetSeed(QString value) {
    m_seed = value;
}

void WiiMixBingoSettings::SetDifficulty(WiiMixEnums::Difficulty difficulty) {
    m_difficulty = difficulty;
    Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_DIFFICULTY, difficulty);
    // emit SettingsChanged(difficulty);
}

void WiiMixBingoSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) {
    m_save_state_bank = save_state_bank;
    Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_SAVE_STATE_BANK, save_state_bank);
    // emit SettingsChanged(save_state_bank);
}

QString WiiMixBingoSettings::BingoTypeToString(WiiMixEnums::BingoType type) {
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

WiiMixEnums::BingoType WiiMixBingoSettings::StringToBingoType(QString type) {
    if (type == QStringLiteral("Bingo"))
        return WiiMixEnums::BingoType::BINGO;
    else if (type == QStringLiteral("Lockout"))
        return WiiMixEnums::BingoType::LOCKOUT;
    else if (type == QStringLiteral("Time Attack"))
        return WiiMixEnums::BingoType::TIME_ATTACK;
    else
        return WiiMixEnums::BingoType::END; // Default case
}

int WiiMixBingoSettings::StringToCardSize(QString size) {
    int num = 0;
    if (!size.isEmpty()) {
        num = size[0].digitValue();
    }
    return num * num;
}