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
                                 WiiMixEnums::Difficulty difficulty, uint64_t time, std::string creator_username,
                                 WiiMixEnums::ObjectiveStatus status, int num_times_completed,
                                 int num_times_attempted, std::chrono::system_clock::time_point last_attempted)
    : m_id(id), m_title(std::move(title)), m_retroachievements_game_id(retroachievements_game_id),
      m_game_id(std::move(game_id)), m_achievement_id(achievement_id),
      m_objective_types(objective_type), m_objective_description(std::move(objective_description)),
      m_game_genres(std::move(game_genres)), m_difficulty(difficulty), m_time(time), m_creator_username(creator_username),
      m_status(status), m_num_times_completed(num_times_completed), m_num_times_attempted(num_times_attempted),
      m_last_attempted(last_attempted)
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

std::string WiiMixObjective::GetCreatorUsername()
{
  return m_creator_username;
}

QJsonObject WiiMixObjective::ToJson()
{
  QJsonObject obj;
  obj[QStringLiteral(OBJECTIVE_ID)] = m_id;
  obj[QStringLiteral(OBJECTIVE_TITLE)] = QString::fromStdString(m_title);
  obj[QStringLiteral(OBJECTIVE_RETROACHIEVEMENTS_GAME_ID)] = m_retroachievements_game_id;
  obj[QStringLiteral(OBJECTIVE_GAME_ID)] = QString::fromStdString(m_game_id);
  obj[QStringLiteral(OBJECTIVE_ACHIEVEMENT_ID)] = m_achievement_id;
  QJsonArray objective_types;
  for (const auto& type : m_objective_types)
  {
    objective_types.append(QString::fromStdString(WiiMixEnums::ObjectiveTypeToString(type)));
  }
  obj[QStringLiteral(OBJECTIVE_OBJECTIVE_TYPES)] = objective_types;
  obj[QStringLiteral(OBJECTIVE_OBJECTIVE_DESCRIPTION)] = QString::fromStdString(m_objective_description);
  QJsonArray game_genres;
  for (const auto& genre : m_game_genres)
  {
    game_genres.append(QString::fromStdString(WiiMixEnums::GameGenreToString(genre)));
  }
  obj[QStringLiteral(OBJECTIVE_GAME_GENRES)] = game_genres;
  obj[QStringLiteral(OBJECTIVE_DIFFICULTY)] = QString::fromStdString(WiiMixEnums::DifficultyToString(m_difficulty));
  obj[QStringLiteral(OBJECTIVE_TIME)] = static_cast<qint64>(m_time);
  if (m_creator_username.empty())
  {
    obj[QStringLiteral(OBJECTIVE_CREATOR_USERNAME)] = QJsonValue::Null;
  }
  else
  {
    obj[QStringLiteral(OBJECTIVE_CREATOR_USERNAME)] = QString::fromStdString(m_creator_username);
  }
  obj[QStringLiteral(OBJECTIVE_HISTORY_MOST_RECENT_STATUS)] = QString::fromStdString(WiiMixEnums::ObjectiveStatusToString(m_status));
  obj[QStringLiteral(OBJECTIVE_HISTORY_NUM_TIMES_COMPLETED)] = m_num_times_completed;
  obj[QStringLiteral(OBJECTIVE_HISTORY_NUM_TIMES_ATTEMPTED)] = m_num_times_attempted;
  obj[QStringLiteral(OBJECTIVE_HISTORY_MOST_RECENT_ATTEMPT)] = static_cast<qint64>(std::chrono::duration_cast<std::chrono::seconds>(
    m_last_attempted.time_since_epoch()).count());
  return obj;
}

WiiMixObjective WiiMixObjective::FromJson(const QJsonObject& obj)
{
  uint16_t id = static_cast<uint16_t>(obj[QStringLiteral(OBJECTIVE_ID)].toInt());
  std::string title = obj[QStringLiteral(OBJECTIVE_TITLE)].toString().toStdString();
  uint16_t retro_id = static_cast<uint16_t>(obj[QStringLiteral(OBJECTIVE_RETROACHIEVEMENTS_GAME_ID)].toInt());
  std::string game_id = obj[QStringLiteral(OBJECTIVE_GAME_ID)].toString().toStdString();
  uint16_t achievement_id = static_cast<uint16_t>(obj[QStringLiteral(OBJECTIVE_ACHIEVEMENT_ID)].toInt());
  std::vector<WiiMixEnums::ObjectiveType> objective_types;
  for (const auto& objective_type : obj[QStringLiteral(OBJECTIVE_OBJECTIVE_TYPES)].toArray())
    objective_types.push_back(WiiMixEnums::ObjectiveTypeFromString(objective_type.toString().toStdString()));
  std::string description = obj[QStringLiteral(OBJECTIVE_OBJECTIVE_DESCRIPTION)].toString().toStdString();
  std::vector<WiiMixEnums::GameGenre> game_genres;
  for (const auto& genre : obj[QStringLiteral(OBJECTIVE_GAME_GENRES)].toArray())
    game_genres.push_back(WiiMixEnums::GameGenreFromString(genre.toString().toStdString()));
  auto difficulty = WiiMixEnums::DifficultyFromString(
    obj[QStringLiteral(OBJECTIVE_DIFFICULTY)].toString().toStdString());
  uint64_t time = static_cast<uint64_t>(obj[QStringLiteral(OBJECTIVE_TIME)].toInt());
  std::string creator_username = obj[QStringLiteral(OBJECTIVE_CREATOR_USERNAME)].isNull() ? "" : obj[QStringLiteral(OBJECTIVE_CREATOR_USERNAME)].toString().toStdString();
  auto status = WiiMixEnums::ObjectiveStatusFromString(obj[QStringLiteral(OBJECTIVE_HISTORY_MOST_RECENT_STATUS)].toString().toStdString());
  int num_times_completed = obj[QStringLiteral(OBJECTIVE_HISTORY_NUM_TIMES_COMPLETED)].toInt();
  int num_times_attempted = obj[QStringLiteral(OBJECTIVE_HISTORY_NUM_TIMES_ATTEMPTED)].toInt();
  std::chrono::system_clock::time_point last_attempted = std::chrono::system_clock::from_time_t(
    obj[QStringLiteral(OBJECTIVE_HISTORY_MOST_RECENT_ATTEMPT)].toInt());

  return WiiMixObjective(
    id, title, retro_id, game_id, achievement_id, objective_types, description,
    game_genres, difficulty, time, creator_username);
}

WiiMixEnums::Player WiiMixObjective::GetCompleted()
{
  return m_player_completed;
}

void WiiMixObjective::SetCompleted(WiiMixEnums::Player player)
{
  m_player_completed = player;
}

WiiMixEnums::ObjectiveStatus WiiMixObjective::GetStatus()
{
  return m_status;
}

int WiiMixObjective::GetNumTimesCompleted()
{
  return m_num_times_completed;
}

int WiiMixObjective::GetNumTimesAttempted()
{
  return m_num_times_attempted;
}

std::chrono::system_clock::time_point WiiMixObjective::GetLastAttempted()
{
  return m_last_attempted;
}

void WiiMixObjective::SetStatus(WiiMixEnums::ObjectiveStatus status)
{
  m_status = status;
}

void WiiMixObjective::SetNumTimesCompleted(int num_times_completed)
{
  m_num_times_completed = num_times_completed;
}

void WiiMixObjective::SetNumTimesAttempted(int num_times_attempted)
{
  m_num_times_attempted = num_times_attempted;
}

void WiiMixObjective::SetLastAttempted(std::chrono::system_clock::time_point last_attempted)
{
  m_last_attempted = last_attempted;
}
