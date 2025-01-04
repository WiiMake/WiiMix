#include "DolphinQt/WiiMix/Player.h"

WiiMixPlayer::WiiMixPlayer(uint16_t id, std::string username, std::string password_hash, WiiMixEnums::Role role, int num_objectives_completed, int num_unique_objectives_completed, int num_objectives_attempted, int num_objectives_created)
    : m_id(id), m_username(std::move(username)), m_password_hash(std::move(password_hash)), m_role(role), m_num_objectives_completed(num_objectives_completed), m_num_unique_objectives_completed(num_unique_objectives_completed), m_num_objectives_attempted(num_objectives_attempted), m_num_objectives_created(num_objectives_created)
{}

uint16_t WiiMixPlayer::GetId()
{
    return m_id;
}

std::string WiiMixPlayer::GetUsername()
{
    return m_username;
}

std::string WiiMixPlayer::GetPasswordHash()
{
    return m_password_hash;
}

QJsonObject WiiMixPlayer::ToJson()
{
    QJsonObject obj;
    obj[QStringLiteral(PLAYER_ID)] = static_cast<int>(m_id);
    obj[QStringLiteral(PLAYER_USERNAME)] = QString::fromStdString(m_username);
    obj[QStringLiteral(PLAYER_PASSWORD_HASH)] = QString::fromStdString(m_password_hash);
    obj[QStringLiteral(PLAYER_ROLE)] = QStringLiteral("player");
    obj[QStringLiteral(PLAYER_NUM_OBJECTIVES_COMPLETED)] = m_num_objectives_completed;
    obj[QStringLiteral(PLAYER_NUM_UNIQUE_OBJECTIVES_COMPLETED)] = m_num_unique_objectives_completed;
    obj[QStringLiteral(PLAYER_NUM_OBJECTIVES_ATTEMPTED)] = m_num_objectives_attempted;
    obj[QStringLiteral(PLAYER_NUM_OBJECTIVES_CREATED)] = m_num_objectives_created;
    return obj;
}

WiiMixPlayer WiiMixPlayer::FromJson(const QJsonObject& obj)
{
    uint16_t id = static_cast<uint16_t>(obj[QStringLiteral(PLAYER_ID)].toInt());
    std::string username = obj[QStringLiteral(PLAYER_USERNAME)].toString().toStdString();
    std::string password_hash = obj[QStringLiteral(PLAYER_PASSWORD_HASH)].toString().toStdString();\
    WiiMixEnums::Role role = WiiMixEnums::RoleFromString(obj[QStringLiteral(PLAYER_ROLE)].toString().toStdString());
    int num_objectives_completed = obj[QStringLiteral(PLAYER_NUM_OBJECTIVES_COMPLETED)].toInt();
    int num_unique_objectives_completed = obj[QStringLiteral(PLAYER_NUM_UNIQUE_OBJECTIVES_COMPLETED)].toInt();
    int num_objectives_attempted = obj[QStringLiteral(PLAYER_NUM_OBJECTIVES_ATTEMPTED)].toInt();
    int num_objectives_created = obj[QStringLiteral(PLAYER_NUM_OBJECTIVES_CREATED)].toInt();
    return WiiMixPlayer(id, username, password_hash, role, num_objectives_completed, num_unique_objectives_completed, num_objectives_attempted, num_objectives_created);
}

void WiiMixPlayer::SetNumObjectivesCompleted(int num_objectives_completed)
{
    m_num_objectives_completed = num_objectives_completed;
}

int WiiMixPlayer::GetNumObjectivesCompleted()
{
    return m_num_objectives_completed;
}

void WiiMixPlayer::SetNumUniqueObjectivesCompleted(int num_unique_objectives_completed)
{
    m_num_unique_objectives_completed = num_unique_objectives_completed;
}

int WiiMixPlayer::GetNumUniqueObjectivesCompleted()
{
    return m_num_unique_objectives_completed;
}

void WiiMixPlayer::SetNumObjectivesAttempted(int num_objectives_attempted)
{
    m_num_objectives_attempted = num_objectives_attempted;
}

int WiiMixPlayer::GetNumObjectivesAttempted()
{
    return m_num_objectives_attempted;
}

void WiiMixPlayer::SetNumObjectivesCreated(int num_objectives_created)
{
    m_num_objectives_created = num_objectives_created;
}

int WiiMixPlayer::GetNumObjectivesCreated()
{
    return m_num_objectives_created;
}
