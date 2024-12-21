#include "DolphinQt/WiiMix/Objective.h"

#include "DolphinQt/WiiMix/Enums.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
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


QJsonObject WiiMixObjective::ToJson()
{
  QJsonObject obj;
  obj[QStringLiteral("id")] = m_id;
  obj[QStringLiteral("title")] = QString::fromStdString(m_title);
  obj[QStringLiteral("retroachievements_game_id")] = m_retroachievements_game_id;
  obj[QStringLiteral("game_id")] = QString::fromStdString(m_game_id);
  obj[QStringLiteral("achievement_id")] = m_achievement_id;
  obj[QStringLiteral("objective_type")] = QString::fromStdString(WiiMixEnums::ObjectiveTypeToString(m_objective_type));
  obj[QStringLiteral("objective_description")] = QString::fromStdString(m_objective_description);
  QJsonArray game_genres;
  for (const auto& genre : m_game_genres)
  {
    game_genres.append(QString::fromStdString(WiiMixEnums::GameGenreToString(genre)));
  }
  obj[QStringLiteral("game_genres")] = game_genres;
  obj[QStringLiteral("difficulty")] = QString::fromStdString(WiiMixEnums::DifficultyToString(m_difficulty));
  obj[QStringLiteral("time")] = static_cast<qint64>(m_time);
  return obj;
}

void WiiMixObjective::FromJson(const QJsonObject& obj)
{

  m_id = obj[QStringLiteral("id")].toInt();
  m_title = obj[QStringLiteral("title")].toString().toStdString();
  m_retroachievements_game_id = obj[QStringLiteral("retroachievements_game_id")].toInt();
  m_game_id = obj[QStringLiteral("game_id")].toString().toStdString();
  m_achievement_id = obj[QStringLiteral("achievement_id")].toInt();
  m_objective_type = WiiMixEnums::ObjectiveTypeFromString(obj[QStringLiteral("objective_type")].toString().toStdString());
  m_objective_description = obj[QStringLiteral("objective_description")].toString().toStdString();
  m_game_genres.clear();
  for (const auto& genre : obj[QStringLiteral("game_genres")].toArray())
  {
    m_game_genres.push_back(WiiMixEnums::GameGenreFromString(genre.toString().toStdString()));
  }
  m_difficulty = WiiMixEnums::DifficultyFromString(obj[QStringLiteral("difficulty")].toString().toStdString());
  m_time = obj[QStringLiteral("time")].toInt();

}




WiiMixEnums::Player WiiMixObjective::GetCompleted()
{
  return m_player_completed;
}

void WiiMixObjective::SetCompleted(WiiMixEnums::Player player)
{
  m_player_completed = player;
}

// @gyoder icon?
