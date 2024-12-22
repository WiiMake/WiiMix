#include "DolphinQt/WiiMix/Server/BingoSettings.h"
#include "DolphinQt/WiiMix/CommonSettings.h"

#include <QJsonObject>

#include "DolphinQt/WiiMix/Enums.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

WiiMixBingoSettings::WiiMixBingoSettings(WiiMixCommonSettings& settings, WiiMixEnums::BingoType bingo_type, int card_size)
    : WiiMixCommonSettings(settings.GetDifficulty(), settings.GetSaveStateBank(), settings.GetObjectives()), m_bingo_type(bingo_type), m_card_size(card_size) 
{}


WiiMixBingoSettings::WiiMixBingoSettings(WiiMixEnums::BingoType bingo_type, int card_size) : WiiMixCommonSettings(), m_bingo_type(bingo_type), m_card_size(card_size) {}

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

// Copy constructor if on server
WiiMixBingoSettings WiiMixBingoSettings::FromJson(QJsonDocument json)
{
    // Take care of the common settings first
    WiiMixCommonSettings common_settings = WiiMixCommonSettings::FromJsonCommon(json);
    QJsonObject obj = json.object();
    WiiMixEnums::BingoType bingo_type = static_cast<WiiMixEnums::BingoType>(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_BINGO_TYPE)].toInt());
    bool teams = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_TEAMS)].toBool();
    int card_size = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_CARD_SIZE)].toInt();

    QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> players;
    QJsonObject players_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS)].toObject();
    for (auto it = players_variant.begin(); it != players_variant.end(); ++it)
    {
        QJsonObject player_info = it.value().toObject();
        WiiMixEnums::Color color = static_cast<WiiMixEnums::Color>(player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_COLOR)].toInt());
        QString name = player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_NAME)].toString();
        players[static_cast<WiiMixEnums::Player>(it.key().toInt())] = QPair<WiiMixEnums::Color, QString>(color, name);
    }

    QMap<WiiMixEnums::Player, int> current_objectives;
    QJsonObject current_objectives_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_CURRENT_OBJECTIVES)].toObject();
    for (auto it = current_objectives_variant.begin(); it != current_objectives_variant.end(); ++it) {
        current_objectives[static_cast<WiiMixEnums::Player>(it.key().toInt())] = it.value().toInt();
    }

    QMap<WiiMixEnums::Player, bool> players_ready;
    QJsonObject players_ready_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS_READY)].toObject();
    for (auto it = players_ready_variant.begin(); it != players_ready_variant.end(); ++it) {
        players_ready[static_cast<WiiMixEnums::Player>(it.key().toInt())] = it.value().toBool();
    }

    QString lobby_id = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)].toString();
    QString lobby_password = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD)].toString();
    QString seed = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_SEED)].toString();

    WiiMixBingoSettings new_settings = WiiMixBingoSettings(common_settings, bingo_type, card_size);
    new_settings.SetTeams(teams);
    new_settings.SetPlayers(players);
    new_settings.SetCurrentObjectives(current_objectives);
    new_settings.SetPlayersReady(players_ready);
    new_settings.SetLobbyID(lobby_id);
    new_settings.SetLobbyPassword(lobby_password);
    new_settings.SetSeed(seed);
    return new_settings;
}

void WiiMixBingoSettings::UpdateFromJson(QJsonDocument json) {
    // Instance method; update the current settings
    
    // Take care of the common settings first
    FromJsonCommon(json);
    QJsonObject obj = json.object();
    SetBingoType(static_cast<WiiMixEnums::BingoType>(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_BINGO_TYPE)].toInt()));
    SetTeams(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_TEAMS)].toBool());
    SetCardSize(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_CARD_SIZE)].toInt());

    QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> players;
    QJsonObject players_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS)].toObject();
    for (auto it = players_variant.begin(); it != players_variant.end(); ++it)
    {
        QJsonObject player_info = it.value().toObject();
        WiiMixEnums::Color color = static_cast<WiiMixEnums::Color>(player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_COLOR)].toInt());
        QString name = player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_NAME)].toString();
        players[static_cast<WiiMixEnums::Player>(it.key().toInt())] = QPair<WiiMixEnums::Color, QString>(color, name);
    }
    SetPlayers(players);

    QMap<WiiMixEnums::Player, int> current_objectives;
    QJsonObject current_objectives_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_CURRENT_OBJECTIVES)].toObject();
    for (auto it = current_objectives_variant.begin(); it != current_objectives_variant.end(); ++it) {
        current_objectives[static_cast<WiiMixEnums::Player>(it.key().toInt())] = it.value().toInt();
    }
    SetCurrentObjectives(current_objectives);

    QMap<WiiMixEnums::Player, bool> players_ready;
    QJsonObject players_ready_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS_READY)].toObject();
    for (auto it = players_ready_variant.begin(); it != players_ready_variant.end(); ++it) {
        players_ready[static_cast<WiiMixEnums::Player>(it.key().toInt())] = it.value().toBool();
    }
    SetPlayersReady(players_ready);

    SetLobbyID(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)].toString());
    SetLobbyPassword(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD)].toString());
    SetSeed(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_SEED)].toString());
    
    return;
}

QString WiiMixBingoSettings::GetSeed() {
    return m_seed;
}

void WiiMixBingoSettings::SetSeed(QString value) {
    m_seed = value;
}

void WiiMixBingoSettings::SetDifficulty(WiiMixEnums::Difficulty difficulty) {
    m_difficulty = difficulty;
}

void WiiMixBingoSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) {
    m_save_state_bank = save_state_bank;
}