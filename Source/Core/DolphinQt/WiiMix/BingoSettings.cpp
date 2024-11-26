#include "DolphinQt/WiiMix/BingoSettings.h"
#include "DolphinQt/WiiMix/Settings.h"

#include <QJsonObject>

#include "DolphinQt/WiiMix/Enums.h"

WiiMixBingoSettings::WiiMixBingoSettings(WiiMixSettings& settings, WiiMixEnums::BingoType bingo_type, int card_size)
    : WiiMixSettings(settings.GetDifficulty(), settings.GetMode(), settings.GetSaveStateBank(), settings.GetObjectives(), settings.GetGamesList()), m_bingo_type(bingo_type), m_card_size(card_size)
{}

WiiMixBingoSettings::WiiMixBingoSettings(WiiMixEnums::BingoType bingo_type, int card_size) : WiiMixSettings(), m_bingo_type(bingo_type), m_card_size(card_size)
{}

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
        players_variant[QString::number(static_cast<int>(it.key()))] = player_info;
        players_ready_variant[QString::number(static_cast<int>(it.key()))] = m_players_ready[it.key()];
        current_objectives_variant[QString::number(static_cast<int>(it.key()))] = m_current_objectives[it.key()];
    }
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS)] = QJsonDocument::fromVariant(players_variant).object();
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_CURRENT_OBJECTIVES)] = QJsonDocument::fromVariant(current_objectives_variant).object();
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS_READY)] = QJsonDocument::fromVariant(players_ready_variant).object();
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)] = m_lobby_id;
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD)] = m_lobby_password;

    return QJsonDocument(json);
}

WiiMixBingoSettings WiiMixBingoSettings::FromJson(QJsonDocument json)
{
    // Take care of the common settings first
    WiiMixSettings common_settings = FromJsonCommon(json);
    WiiMixBingoSettings settings = WiiMixBingoSettings(common_settings);
    QJsonObject obj = json.object();
    settings.SetBingoType(static_cast<WiiMixEnums::BingoType>(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_BINGO_TYPE)].toInt()));
    settings.SetTeams(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_TEAMS)].toBool());
    settings.SetCardSize(obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_CARD_SIZE)].toInt());

    QJsonObject players_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS)].toObject();
    for (auto it = players_variant.begin(); it != players_variant.end(); ++it)
    {
        QJsonObject player_info = it.value().toObject();
        WiiMixEnums::Color color = static_cast<WiiMixEnums::Color>(player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_COLOR)].toInt());
        QString name = player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_NAME)].toString();
        settings.m_players[static_cast<WiiMixEnums::Player>(it.key().toInt())] = QPair<WiiMixEnums::Color, QString>(color, name);
    }
    QJsonObject current_objectives_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_CURRENT_OBJECTIVES)].toObject();
    for (auto it = current_objectives_variant.begin(); it != players_variant.end(); ++it) {
        settings.m_current_objectives[static_cast<WiiMixEnums::Player>(it.key().toInt())] = it.value().toInt();
    }
    QJsonObject players_ready_variant = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS_READY)].toObject();
    for (auto it = players_ready_variant.begin(); it != players_variant.end(); ++it) {
        settings.m_players_ready[static_cast<WiiMixEnums::Player>(it.key().toInt())] = it.value().toBool();
    }
    
    settings.m_lobby_id = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)].toString();
    settings.m_lobby_password = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD)].toString();

    return settings;
}
