// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include <map>
#include <chrono>
#include <QJsonObject>

class WiiMixObjective
{
public:
  explicit WiiMixObjective(
    uint16_t id,
    std::string title,
    uint16_t retroachievements_game_id,
    std::string game_id,
    uint16_t achievement_id,
    std::vector<WiiMixEnums::ObjectiveType> objective_type,
    std::string objective_description,
    std::vector<WiiMixEnums::GameGenre> game_genres,
    WiiMixEnums::Difficulty difficulty,
    uint64_t time,
    std::string creator_username = NULL,
    WiiMixEnums::ObjectiveStatus status = WiiMixEnums::ObjectiveStatus::UNCOMPLETED,
    int num_times_completed = 0,
    int num_times_attempted = 0,
    std::chrono::system_clock::time_point last_attempted = std::chrono::system_clock::now()
  );

  #define OBJECTIVE_ID "id"
  #define OBJECTIVE_TITLE "title"
  #define OBJECTIVE_RETROACHIEVEMENTS_GAME_ID "retroachievements_game_id"
  #define OBJECTIVE_GAME_ID "game_id"
  #define OBJECTIVE_ACHIEVEMENT_ID "achievement_id"
  #define OBJECTIVE_OBJECTIVE_TYPES "objective_types"
  #define OBJECTIVE_OBJECTIVE_DESCRIPTION "objective_description"
  #define OBJECTIVE_GAME_GENRES "game_genres"
  #define OBJECTIVE_DIFFICULTY "difficulty"
  #define OBJECTIVE_TIME "time"
  #define OBJECTIVE_CREATOR_USERNAME "creator_username"

  // Objective history extension; these have default values so aren't necessary to be set,
  // but offer extra data if you want it :)
  #define OBJECTIVE_HISTORY_ID "id"
  #define OBJECTIVE_HISTORY_NUM_TIMES_COMPLETED "num_times_completed"
  #define OBJECTIVE_HISTORY_NUM_TIMES_ATTEMPTED "num_times_attempted"
  #define OBJECTIVE_HISTORY_MOST_RECENT_STATUS "most_recent_status"
  #define OBJECTIVE_HISTORY_MOST_RECENT_ATTEMPT "most_recent_attempt"
  // Not used directly in json conversion, but used for db queries for objective_history
  #define OBJECTIVE_HISTORY_PLAYER_ID "player_id"
  #define OBJECTIVE_HISTORY_OBJECTIVE_ID "objective_id"

  // Extras; used for selecting individual objectives
  #define GAME_GENRE "game_genre"
  #define OBJECTIVE_TYPE "objective_type"
  #define STATE_SLOT "slot"

  uint16_t GetId();
  std::string GetTitle();
  uint16_t GetRetroAchievementsGameId();
  std::string GetGameId();
  uint16_t GetAchievementId();
  std::vector<WiiMixEnums::ObjectiveType> GetObjectiveTypes();
  std::string GetObjectiveDescription();
  std::vector<WiiMixEnums::GameGenre> GetGameGenres();
  WiiMixEnums::Difficulty GetDifficulty();
  uint64_t GetTime();
  std::string GetCreatorUsername();
  WiiMixEnums::ObjectiveStatus GetStatus();
  int GetNumTimesCompleted();
  int GetNumTimesAttempted();
  std::chrono::system_clock::time_point GetLastAttempted();

  QJsonObject ToJson();
  static WiiMixObjective FromJson(const QJsonObject& obj);

  WiiMixEnums::Player GetCompleted();
  void SetCompleted(WiiMixEnums::Player player);

  // DB stuff
  void SetStatus(WiiMixEnums::ObjectiveStatus status);
  void SetNumTimesCompleted(int num_times_completed);
  void SetNumTimesAttempted(int num_times_attempted);
  void SetLastAttempted(std::chrono::system_clock::time_point last_attempted);

private:
  uint16_t m_id;
  std::string m_title;
  uint16_t m_retroachievements_game_id;
  std::string m_game_id;
  uint16_t m_achievement_id;
  std::vector<WiiMixEnums::ObjectiveType> m_objective_types;
  std::string m_objective_description;
  std::vector<WiiMixEnums::GameGenre> m_game_genres;
  WiiMixEnums::Difficulty m_difficulty;
  uint64_t m_time;
  std::string m_creator_username;

  // For bingo -> might refactor
  WiiMixEnums::Player m_player_completed;

  // Join in from objective history table; oftentimes set to null or defaults in the constructor
  WiiMixEnums::ObjectiveStatus m_status;
  int m_num_times_completed;
  int m_num_times_attempted;
  std::chrono::system_clock::time_point m_last_attempted;
};