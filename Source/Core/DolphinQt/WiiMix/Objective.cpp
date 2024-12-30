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
                                 std::vector<WiiMixEnums::ObjectiveType> objective_type,
                                 std::string objective_description,
                                 std::vector<WiiMixEnums::GameGenre> game_genres,
                                 WiiMixEnums::Difficulty difficulty, uint64_t time)
    : m_id(id), m_title(std::move(title)), m_retroachievements_game_id(retroachievements_game_id),
      m_game_id(std::move(game_id)), m_achievement_id(achievement_id),
      m_objective_types(objective_type), m_objective_description(std::move(objective_description)),
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

std::vector<WiiMixEnums::ObjectiveType> WiiMixObjective::GetObjectiveTypes()
{
  return m_objective_types;
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
  QJsonArray objective_types;
  for (const auto& type : m_objective_types)
  {
    objective_types.append(QString::fromStdString(WiiMixEnums::ObjectiveTypeToString(type)));
  }
  obj[QStringLiteral("objective_types")] = objective_types;
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

WiiMixObjective WiiMixObjective::FromJson(const QJsonObject& obj)
{
  uint16_t id = static_cast<uint16_t>(obj[QStringLiteral("id")].toInt());
  std::string title = obj[QStringLiteral("title")].toString().toStdString();
  uint16_t retro_id = static_cast<uint16_t>(obj[QStringLiteral("retroachievements_game_id")].toInt());
  std::string game_id = obj[QStringLiteral("game_id")].toString().toStdString();
  uint16_t achievement_id = static_cast<uint16_t>(obj[QStringLiteral("achievement_id")].toInt());
  std::vector<WiiMixEnums::ObjectiveType> objective_types;
  for (const auto& objective_type : obj[QStringLiteral("objective_types")].toArray())
    objective_types.push_back(WiiMixEnums::ObjectiveTypeFromString(objective_type.toString().toStdString()));
  std::string description = obj[QStringLiteral("objective_description")].toString().toStdString();
  std::vector<WiiMixEnums::GameGenre> game_genres;
  for (const auto& genre : obj[QStringLiteral("game_genres")].toArray())
    game_genres.push_back(WiiMixEnums::GameGenreFromString(genre.toString().toStdString()));
  auto difficulty = WiiMixEnums::DifficultyFromString(
    obj[QStringLiteral("difficulty")].toString().toStdString());
  uint64_t time = static_cast<uint64_t>(obj[QStringLiteral("time")].toInt());

  return WiiMixObjective(
    id, title, retro_id, game_id, achievement_id, objective_types, description,
    game_genres, difficulty, time);
}

WiiMixEnums::Player WiiMixObjective::GetCompleted()
{
  return m_player_completed;
}

void WiiMixObjective::SetCompleted(WiiMixEnums::Player player)
{
  m_player_completed = player;
}
