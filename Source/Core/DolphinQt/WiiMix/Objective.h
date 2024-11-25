// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include <map>
#include <vector>

class WiiMixObjective
{
public:
  explicit WiiMixObjective(
    uint16_t achievement_id,
    uint16_t game_id,
    std::string title,
    std::string description,
    std::string savestate_file,
    std::string iso_file,
    WiiMixEnums::Player completed
  ) : m_achievement_id(achievement_id), m_game_id(game_id), m_title(title), m_description(description), m_savestate_file(savestate_file), m_iso_file(iso_file), m_completed(completed){};

  uint16_t GetAchievementID();
  uint16_t GetGameID();
  std::string GetTitle();
  std::string GetDescription();
  std::string GetSavestateFile();
  void SetSavestateFile(std::string savestate_file);
  std::string GetISOFile();
  void SetISOFile(std::string iso_file);
  WiiMixEnums::Player GetCompleted();
  void SetCompleted(WiiMixEnums::Player player);

  std::string GetBadgeURL();

  void LaunchObjective();

  static void RALoginCallback(std::string username, std::string token);
  static std::map<uint16_t, std::string> getGameAchievements(uint16_t game_id);
  static std::map<std::string, uint16_t> getGameList(uint16_t console_id); // 16 for gamecube

  static std::vector<WiiMixObjective> GetObjectives();
  static void CacheGames();


  std::map<uint16_t, std::string> AchievementIDToSaveStateFile = {

    // WarioWare
    {394715, "GZWE01.s01"},
    {394718, "GZWE01.s02"},

    // F-Zero GX

    // Monkey Ball
    {459112, "GMBE8P.s01"},
    {472874, "GMBE8P.s02"},

    // Pokemon Coliseum
    {404280, "GC6E01.s01"},
    {404282, "GC6E01.s02"},
    {404285, "GC6E01.s03"},
    {404287, "GC6E01.s04"},

    // Smash Melee
    {427418, "GALE01.s01"},
    {434618, "GALE01.s02"},
    {427477, "GALE01.s03"},
    {437003, "GALE01.s04"},
    {436331, "GALE01.s05"},

    // Sunshine
    {395001, "GMSE01.s01"},
    {395002, "GMSE01.s02"},
    {395003, "GMSE01.s03"},
    {395004, "GMSE01.s04"},
    {395098, "GMSE01.s05"},

  };

  std::map<uint16_t, uint16_t> AchievementIDToGameID = {

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

  std::map<uint16_t, std::string> GameIDToFileName = {
    {6605, "WarioWare, Inc. - Mega Party Game$! (USA).iso"},
    {9699, "F-Zero GX (USA).ciso"},
    {9510, "Super Monkey Ball (USA).ciso"},
    {7423, "Pokemon Colosseum (USA).iso"},
    {9602, "Super Smash Bros. Melee (USA) (En,Ja) (v1.02).iso"},
    {6049, "Super Mario Sunshine (USA).iso"},
    {2827, "Tony Hawk's Pro Skater 4 (USA).ciso"}
  };

  static std::map<uint16_t, std::string> m_games_cache;
private:
  uint16_t m_achievement_id;
  uint16_t m_game_id;
  WiiMixEnums::Player m_completed;
  std::string m_title;
  std::string m_description;
  std::string m_savestate_file;
  std::string m_iso_file;
  static std::string m_username;
  static std::string m_token;
};
