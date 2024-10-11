// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QString>
#include <QIcon>

#include "DolphinQt/WiiMix/Objective.h"
#include "UICommon/GameFile.h"

class WiiMixSettings 
{
public:

  enum class Difficulty {
    NORMAL,
    HARD,
    WIISANITY,
    END // Default/size value
  };

  static QString DifficultyToString(Difficulty difficulty);

  enum class Mode {
    BINGO, // Sub-modes: lockout, capture the flag
    SHUFFLE, // Sub-modes: race
    ROGUE,
    END, // Default/size value
  };

  static QString ModeToTitle(Mode mode);
  static QIcon ModeToIcon(Mode mode);
  static QString ModeToDescription(Mode mode);

  explicit WiiMixSettings(Difficulty difficulty = Difficulty::NORMAL, Mode mode = Mode::BINGO, int time = 0, 
    std::vector<WiiMixObjective> objectives = {}, std::vector<UICommon::GameFile> games = {});

  void SetSaveStateBank();
  void SetDifficulty();
  // Time will be an optional parameter that the user can set in case they only have a certain amount of time
  // It will be taken into account when populating objectives
  void SetTime();
  // Note that both version and region are checked manually against the roms that the user has
  // and the supported game hash in the retroachievements database. If the hash is not the same,
  // then achievements for the game will not be used for the sake of stability.
  //   void SetVersion();
  //   void SetRegion();
  void SetMode();
  void SetGamesList();
  void SetObjectives(); // A list of objectives; bingo objectives are read from left to right on the bingo board

private:
  // A NSMBW save state is 28.4MB; that's too big to use GitHub as a storage solution
  // For now since this is a free project, Google Drive API (https://developers.google.com/drive/api/reference/rest/v2)
  // will be used as a Save State Storage Solution (15GB free)
  // but if it ever needs to be scaled up, we can look into self-hosting or alternatives.
  // We'll probably upload two files; the save state, and associated json with each save state that stores parsable info
  // This info includes control scheme, so you don't have to swap between vertical and horizontal wii remotes or 
  // plug in a nunchuck during the middle of the match
  Difficulty m_difficulty;
  Mode m_mode;
  std::vector<WiiMixObjective> m_objectives;
  std::vector<UICommon::GameFile> m_games;
  int m_time;
};
