#include "DolphinQt/WiiMix/BingoSettings.h"

#include <QJsonObject>

#include "DolphinQt/WiiMix/Enums.h"

WiiMixBingoSettings::WiiMixBingoSettings(const WiiMixSettings& settings, WiiMixEnums::BingoType bingo_type, int card_size)
    : WiiMixSettings(settings), m_bingo_type(bingo_type), m_card_size(card_size) 
{}

WiiMixBingoSettings::WiiMixBingoSettings(WiiMixEnums::BingoType bingo_type, int card_size) : WiiMixSettings(WiiMixSettings()), m_bingo_type(WiiMixEnums::BingoType::END), m_card_size(9), m_teams(false)
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

QJsonDocument WiiMixBingoSettings::toJson()
{
    QJsonObject json;
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_BINGO_TYPE)] = static_cast<int>(m_bingo_type);
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_TEAMS)] = m_teams;
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_CARD_SIZE)] = m_card_size;
    QVariantMap players_variant;
    for (auto it = m_players.begin(); it != m_players.end(); ++it)
    {
        QVariantMap player_info;
        player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_COLOR)] = static_cast<int>(std::get<0>(it.value()));
        player_info[QStringLiteral(BINGO_NETPLAY_SETTINGS_NAME)] = std::get<1>(it.value());
        players_variant[QString::number(static_cast<int>(it.key()))] = player_info;
    }
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS)] = QJsonDocument::fromVariant(players_variant).object();
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)] = m_lobby_id;
    json[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD)] = m_lobby_password;
    
    json[QStringLiteral(COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK)] = static_cast<int>(GetSaveStateBank());
    json[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)] = QString::fromStdString(WiiMixSettings::ObjectivesToObjectiveIds(GetObjectives()));
    json[QStringLiteral(COMMON_NETPLAY_SETTINGS_DIFFICULTY)] = static_cast<int>(GetDifficulty());
    json[QStringLiteral(COMMON_NETPLAY_SETTINGS_GAMES_LIST)] = QString::fromStdString(WiiMixSettings::GameFilesToGameIds(GetGamesList()));
    return QJsonDocument(json);
}

WiiMixBingoSettings WiiMixBingoSettings::fromJson(QJsonDocument json)
{
    WiiMixBingoSettings settings;
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
        settings.m_players[static_cast<WiiMixEnums::Player>(it.key().toInt())] = std::make_tuple(color, name);
    }
    
    settings.m_lobby_id = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)].toString();
    settings.m_lobby_password = obj[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_PASSWORD)].toString();
    settings.SetSaveStateBank(static_cast<WiiMixEnums::SaveStateBank>(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_SAVE_STATE_BANK)].toInt()));
    settings.SetObjectives(WiiMixSettings::ObjectiveIdsToObjectives(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)].toString().toStdString()));
    settings.SetDifficulty(static_cast<WiiMixEnums::Difficulty>(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_DIFFICULTY)].toInt()));
    settings.SetGamesList(WiiMixSettings::GameIdsToGameFiles(obj[QStringLiteral(COMMON_NETPLAY_SETTINGS_GAMES_LIST)].toString().toStdString()));
    
    return settings;
}