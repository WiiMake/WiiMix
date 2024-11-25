// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include <map>

class WiiMixObjective
{
public:
  explicit WiiMixObjective(
    uint16_t achievement_id,
    uint16_t game_id,
    std::string title,
    std::string description,
    WiiMixEnums::Player completed
  ) : m_achievement_id(achievement_id), m_game_id(game_id), m_title(title), m_description(description), m_completed(completed){};

  uint16_t GetAchievementID();
  uint16_t GetGameID();
  std::string GetTitle();
  std::string GetDescription();
  WiiMixEnums::Player GetCompleted();
  void SetCompleted(WiiMixEnums::Player player);

  std::string GetBadgeURL();

  void LaunchObjective();

  static void RALoginCallback(std::string username, std::string token);
  static std::map<uint16_t, std::string> getGameAchievements(uint16_t game_id);
  static std::map<std::string, uint16_t> getGameList(uint16_t console_id); // 16 for gamecube


  const std::map<uint16_t, std::string> AchievementIDToSaveStateFile = {

    // WarioWare
    {394715, "GZWE01.so1"},
    {394718, "GZWE01.so2"},

    // F-Zero GX

    // Monkey Ball
    {459112, "GMBE8P.s01"},
    {472874, "GMBE8P.s02"},

    // Pokemon Coliseum
    {404280, "GC6E01.so1"},
    {404282, "GC6E01.so2"},
    {404285, "GC6E01.so3"},
    {404287, "GC6E01.so4"},

    // Smash Melee
    {427418, "GALEO1.so1"},
    {434618, "GALEO1.so2"},
    {427477, "GALEO1.so3"},
    {437003, "GALEO1.so4"},
    {436331, "GALEO1.so5"},

    // Sunshine
    {395001, "GMSE01.so1"},
    {395002, "GMSE01.so2"},
    {395003, "GMSE01.so3"},
    {395004, "GMSE01.so4"},
    {395098, "GMSE01.so5"},

  };

  const std::map<uint16_t, uint16_t> AchievementIDToGameID = {

    // WarioWare
    {394715, 6605},
    {394718, 6605},

    // F-Zero GX

    // Monkey Ball
    {459112, 9510},
    {472874, 9510},

    // Pokemon Coliseum
    {404280, 7423},
    {404282, 7423},
    {404285, 7423},
    {404287, 7423},

    // Smash Melee
    {427418, 9602},
    {434618, 9602},
    {427477, 9602},
    {437003, 9602},
    {436331, 9602},

    // Sunshine
    {395001, 6049},
    {395002, 6049},
    {395003, 6049},
    {395004, 6049},
    {395098, 6049},

  };

  const std::map<uint16_t, std::string> GameIDToFileName = {
    {6605, "WarioWare, Inc. - Mega Party Game$! (USA).iso"},
    {9699, "F-Zero GX (USA).ciso"},
    {9510, "Super Monkey Ball (USA).ciso"},
    {7423, "Pokemon Colosseum (USA).iso"},
    {9602, ""},
    {6049, "Super Mario Sunshine (USA).iso"},
    {2827, "Tony Hawk's Pro Skater 4 (USA).ciso"}
  };

private:
  uint16_t m_achievement_id;
  uint16_t m_game_id;
  WiiMixEnums::Player m_completed;
  std::string m_title;
  std::string m_description;
  static std::string m_username;
  static std::string m_token;
};
