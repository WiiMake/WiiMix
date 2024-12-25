// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include <map>
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
    uint64_t time
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

  QJsonObject ToJson();
  static WiiMixObjective FromJson(const QJsonObject& obj);

  WiiMixEnums::Player GetCompleted();
  void SetCompleted(WiiMixEnums::Player player);

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

  // not in the db
  WiiMixEnums::Player m_player_completed;
};