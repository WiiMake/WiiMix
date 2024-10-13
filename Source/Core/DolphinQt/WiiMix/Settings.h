// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QString>
#include <QIcon>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Objective.h"
#include "UICommon/GameFile.h"

class WiiMixSettings 
{
public:
  // cut int time = DEFAULT_TIME
  explicit WiiMixSettings(WiiMixEnums::Difficulty difficulty = DEFAULT_DIFFICULTY, WiiMixEnums::Mode mode = DEFAULT_MODE, WiiMixEnums::SaveStateBank bank = DEFAULT_SAVE_STATE_BANK, 
    std::vector<WiiMixObjective> objectives = DEFAULT_OBJECTIVES, std::vector<UICommon::GameFile> games = DEFAULT_GAMES);

  void SetSaveStateBank(WiiMixEnums::SaveStateBank bank);
  void SetDifficulty(WiiMixEnums::Difficulty difficulty);
  // Time will be an optional parameter that the user can set in case they only have a certain amount of time
  // It will be taken into account when populating objectives
  // void SetTime(int time);
  // Note that both version and region are checked manually against the roms that the user has
  // and the supported game hash in the retroachievements database. If the hash is not the same,
  // then achievements for the game will not be used for the sake of stability.
  //   void SetVersion();
  //   void SetRegion();
  void SetMode(WiiMixEnums::Mode mode);
  void SetGamesList(std::vector<UICommon::GameFile> game_list);
  void AddGame(UICommon::GameFile game);
  void SetObjectives(std::vector<WiiMixObjective> objectives); // A list of objectives; bingo objectives are read from left to right on the bingo board
  void AddObjective(WiiMixObjective objective);
  WiiMixEnums::Mode GetMode();
  WiiMixEnums::Difficulty GetDifficulty();
  // int GetTime();
  WiiMixEnums::SaveStateBank GetSaveStateBank();
  std::vector<WiiMixObjective> GetObjectives();
  std::vector<UICommon::GameFile> GetGamesList();

  static QString DifficultyToString(WiiMixEnums::Difficulty difficulty);
  static WiiMixEnums::Difficulty StringToDifficulty(QString difficulty);

  static QString ModeToTitle(WiiMixEnums::Mode mode);
  static QIcon ModeToIcon(WiiMixEnums::Mode mode);
  static QString ModeToDescription(WiiMixEnums::Mode mode);
  static WiiMixEnums::Mode StringToMode(QString mode);

  static QString SaveStateBankToString(WiiMixEnums::SaveStateBank bank);
  static WiiMixEnums::SaveStateBank StringToSaveStateBank(QString bank);
  
  static int StringToCardSize(QString size);

  static std::vector<UICommon::GameFile> GameIdsToGameFiles(std::string game_ids_list);
  static std::vector<WiiMixObjective> ObjectiveIdsToObjectives(std::string objective_ids_list);

private:
  // A NSMBW save state is 28.4MB; that's too big to use GitHub as a storage solution
  // For now since this is a free project, Google Drive API (https://developers.google.com/drive/api/reference/rest/v2)
  // will be used as a Save State Storage Solution (15GB free)
  // but if it ever needs to be scaled up, we can look into self-hosting or alternatives.
  // We'll probably upload two files; the save state, and associated json with each save state that stores parsable info
  // This info includes control scheme, so you don't have to swap between vertical and horizontal wii remotes or 
  // plug in a nunchuck during the middle of the match
  WiiMixEnums::SaveStateBank m_save_state_bank;
  WiiMixEnums::Difficulty m_difficulty;
  WiiMixEnums::Mode m_mode;
  std::vector<WiiMixObjective> m_objectives;
  std::vector<UICommon::GameFile> m_games;
  int m_time; // unused for now
};