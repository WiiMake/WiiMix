// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include <map>

class WiiMixObjective
{
public:
  explicit WiiMixObjective(
    std::string objective_string,
    WiiMixEnums::Player completed
  );

  std::string GetObjectiveString();
  WiiMixEnums::Player GetCompleted();
  void SetCompleted(WiiMixEnums::Player player);

  static void RALoginCallback(std::string username, std::string token);
  static std::map<uint16_t, std::string> getGameAchievements(uint16_t game_id);
  static std::map<std::string, uint16_t> getGameList(uint16_t console_id); // 16 for gamecube

private:
  std::string m_objective_string;
  WiiMixEnums::Player m_completed;

  static std::string m_username;
  static std::string m_token;
};
