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

// @gyoder icon?
