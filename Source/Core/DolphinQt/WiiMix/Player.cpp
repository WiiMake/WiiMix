#include "DolphinQt/WiiMix/Player.h"

WiiMixPlayer::WiiMixPlayer(uint16_t id, std::string username, std::string password_hash)
    : m_id(id), m_username(std::move(username)), m_password_hash(std::move(password_hash))
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
    obj["id"] = static_cast<int>(m_id);
    obj["username"] = QString::fromStdString(m_username);
    obj["password_hash"] = QString::fromStdString(m_password_hash);
    return obj;
}

WiiMixPlayer WiiMixPlayer::FromJson(const QJsonObject& obj)
{
    uint16_t id = static_cast<uint16_t>(obj["id"].toInt());
    std::string username = obj["username"].toString().toStdString();
    std::string password_hash = obj["password_hash"].toString().toStdString();
    return WiiMixPlayer(id, username, password_hash);
}