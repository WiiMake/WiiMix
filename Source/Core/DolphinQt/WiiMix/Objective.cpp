#include "DolphinQt/WiiMix/Objective.h"

#include "DolphinQt/WiiMix/Enums.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <curl/curl.h>
#include <fstream>
#include <string>
#include "Objective.h"

WiiMixObjective::WiiMixObjective(uint16_t id, std::string title, uint16_t retroachievements_game_id,
                                 std::string game_id, uint16_t achievement_id,
                                 WiiMixEnums::ObjectiveType objective_type,
                                 std::string objective_description,
                                 std::vector<WiiMixEnums::GameGenre> game_genres,
                                 WiiMixEnums::Difficulty difficulty, uint64_t time)
    : m_id(id), m_title(std::move(title)), m_retroachievements_game_id(retroachievements_game_id),
      m_game_id(std::move(game_id)), m_achievement_id(achievement_id),
      m_objective_type(objective_type), m_objective_description(std::move(objective_description)),
      m_game_genres(std::move(game_genres)), m_difficulty(difficulty), m_time(time)
{
}

uint16_t WiiMixObjective::GetId()
{
  return m_id;
}

std::string WiiMixObjective::GetTitle()
{
  return m_title;
}

uint16_t WiiMixObjective::GetRetroAchievementsGameId()
{
  return m_retroachievements_game_id;
}

std::string WiiMixObjective::GetGameId()
{
  return m_game_id;
}

uint16_t WiiMixObjective::GetAchievementId()
{
  return m_achievement_id;
}

WiiMixEnums::ObjectiveType WiiMixObjective::GetObjectiveType()
{
  return m_objective_type;
}

std::string WiiMixObjective::GetObjectiveDescription()
{
  return m_objective_description;
}

std::vector<WiiMixEnums::GameGenre> WiiMixObjective::GetGameGenres()
{
  return m_game_genres;
}

WiiMixEnums::Difficulty WiiMixObjective::GetDifficulty()
{
  return m_difficulty;
}

uint64_t WiiMixObjective::GetTime()
{
  return m_time;
}

WiiMixEnums::Player WiiMixObjective::GetCompleted()
{
  return m_player_completed;
}

void WiiMixObjective::SetCompleted(WiiMixEnums::Player player)
{
  m_player_completed = player;
}

QJsonDocument WiiMixObjective::ToJson() {
    QJsonObject json;
    json[QStringLiteral("AchievementID")] = static_cast<int>(m_achievement_id);
    json[QStringLiteral("GameID")] = QString::fromStdString(m_game_id);
    json[QStringLiteral("Title")] = QString::fromStdString(m_title);
    json[QStringLiteral("Description")] = QString::fromStdString(m_objective_description);
    json[QStringLiteral("ObjectiveType")] = static_cast<int>(m_objective_type);
    json[QStringLiteral("Difficulty")] = static_cast<int>(m_difficulty);
    json[QStringLiteral("Time")] = static_cast<qint64>(m_time);

    QJsonArray genres_array;
    for (const auto& genre : m_game_genres)
    {
        genres_array.append(static_cast<int>(genre));
    }
    json[QStringLiteral("GameGenres")] = genres_array;

    json[QStringLiteral("Completed")] = static_cast<int>(m_player_completed);
    return QJsonDocument(json);
}

WiiMixObjective WiiMixObjective::FromJson(QJsonDocument json) {
    QJsonObject jsonObj = json.object();
    uint16_t achievement_id = static_cast<uint16_t>(jsonObj[QStringLiteral("AchievementID")].toInt());
    std::string game_id = jsonObj[QStringLiteral("GameID")].toString().toStdString();
    std::string title = jsonObj[QStringLiteral("Title")].toString().toStdString();
    std::string description = jsonObj[QStringLiteral("Description")].toString().toStdString();
    WiiMixEnums::ObjectiveType objective_type = static_cast<WiiMixEnums::ObjectiveType>(jsonObj[QStringLiteral("ObjectiveType")].toInt());
    WiiMixEnums::Difficulty difficulty = static_cast<WiiMixEnums::Difficulty>(jsonObj[QStringLiteral("Difficulty")].toInt());
    uint64_t time = static_cast<uint64_t>(jsonObj[QStringLiteral("Time")].toVariant().toLongLong());

    QJsonArray genres_array = jsonObj[QStringLiteral("GameGenres")].toArray();
    std::vector<WiiMixEnums::GameGenre> game_genres;
    for (const auto& genre : genres_array)
    {
        game_genres.push_back(static_cast<WiiMixEnums::GameGenre>(genre.toInt()));
    }

    WiiMixEnums::Player completed = static_cast<WiiMixEnums::Player>(jsonObj[QStringLiteral("Completed")].toInt());

    return WiiMixObjective(0, title, 0, game_id, achievement_id, objective_type, description, game_genres, difficulty, time);
}