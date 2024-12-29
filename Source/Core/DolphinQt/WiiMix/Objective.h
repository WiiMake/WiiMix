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
    uint64_t creator_id = NULL,
    WiiMixEnums::ObjectiveStatus status = WiiMixEnums::ObjectiveStatus::UNCOMPLETED,
    int num_times_completed = 0,
    int num_times_attempted = 0,
    std::chrono::system_clock::time_point last_attempted = std::chrono::system_clock::now()
  );

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
  uint64_t GetCreatorId();
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
  uint64_t m_creator_id;

  // For bingo -> might refactor
  WiiMixEnums::Player m_player_completed;

  // Join in from objective history table; oftentimes set to null or defaults in the constructor
  WiiMixEnums::ObjectiveStatus m_status;
  int m_num_times_completed;
  int m_num_times_attempted;
  std::chrono::system_clock::time_point m_last_attempted;
};