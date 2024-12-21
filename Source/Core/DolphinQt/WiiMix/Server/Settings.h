// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QString>

// The server has no GUI support
// So the preprocessor is used to weed out GUI functionality
// and replace it with server functionality
// This feels really hacky; the number of preprocessing statements can also be reduced

#include <QJsonObject>
#include <QJsonDocument>

#include <algorithm>
#include <random>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Objective.h"

// SERVER SETTINGS; copyable
class WiiMixSettings {

public:
  explicit WiiMixSettings(WiiMixEnums::Difficulty difficulty = DEFAULT_DIFFICULTY, WiiMixEnums::Mode mode = DEFAULT_MODE, WiiMixEnums::SaveStateBank bank = DEFAULT_SAVE_STATE_BANK,
    std::vector<WiiMixObjective> objectives = DEFAULT_OBJECTIVES, std::string games = DEFAULT_GAMES);

  virtual void SetSaveStateBank(WiiMixEnums::SaveStateBank bank);
  virtual void SetDifficulty(WiiMixEnums::Difficulty difficulty);
  // Time will be an optional parameter that the user can set in case they only have a certain amount of time
  // It will be taken into account when populating objectives
  // void SetTime(int time);
  // Note that both version and region are checked manually against the roms that the user has
  // and the supported game hash in the retroachievements database. If the hash is not the same,
  // then achievements for the game will not be used for the sake of stability.
  //   void SetVersion();
  //   void SetRegion();
  void SetMode(WiiMixEnums::Mode mode);

  void SetObjectives(std::vector<WiiMixObjective> objectives); // A list of objectives; bingo objectives are read from left to right on the bingo board
  void AddObjective(WiiMixObjective objective);
  const WiiMixEnums::Mode GetMode() const;
  const WiiMixEnums::Difficulty GetDifficulty() const;
  // int GetTime();
  const WiiMixEnums::SaveStateBank GetSaveStateBank() const;
  const std::vector<WiiMixObjective> GetObjectives() const;

  static QString DifficultyToString(WiiMixEnums::Difficulty difficulty);
  static WiiMixEnums::Difficulty StringToDifficulty(QString difficulty);

  static QString ModeToTitle(WiiMixEnums::Mode mode);
  static QString ModeToDescription(WiiMixEnums::Mode mode);
  static WiiMixEnums::Mode StringToMode(QString mode);

  static QString SaveStateBankToString(WiiMixEnums::SaveStateBank bank);
  static WiiMixEnums::SaveStateBank StringToSaveStateBank(QString bank);

  static QString BingoTypeToString(WiiMixEnums::BingoType type);
  static WiiMixEnums::BingoType StringToBingoType(QString type);

  static int StringToCardSize(QString size);

  static std::vector<WiiMixObjective> ObjectiveIdsToObjectives(std::string objective_ids_list);
  // Using the built-in Objective::ToJson() function as opposed to the below function
  // static std::string ObjectivesToObjectiveIds(std::vector<WiiMixObjective> objectives);

  const std::string GetGamesList() const;

  QJsonObject ToJsonCommon();
  static WiiMixSettings FromJsonCommon(QJsonDocument settings_json);
  // WiiMixSettings FromJsonCommon(QJsonDocument settings_json);

protected:
  WiiMixEnums::Difficulty m_difficulty;
  WiiMixEnums::SaveStateBank m_save_state_bank;

private:
  WiiMixEnums::Mode m_mode;
  std::vector<WiiMixObjective> m_objectives;
  std::string m_games; // Use string IDs for server because UICommon::GameFile can't be passed via json
  
  int m_time; // unused for now
};
