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

// Operating on the assumption that GameFile.h is not reliant on Qt GUI

// This is just meant for class extension, and as such is not a singleton
class WiiMixCommonSettings : public QObject {
  Q_OBJECT

public:
  explicit WiiMixCommonSettings(WiiMixEnums::Difficulty difficulty = DEFAULT_DIFFICULTY, WiiMixEnums::SaveStateBank bank = DEFAULT_SAVE_STATE_BANK,
    std::vector<WiiMixObjective> objectives = DEFAULT_OBJECTIVES, std::vector<WiiMixEnums::ObjectiveType> types = DEFAULT_OBJECTIVE_TYPES, std::vector<WiiMixEnums::GameGenre> genres = DEFAULT_GAME_GENRES);
  WiiMixCommonSettings(const WiiMixCommonSettings& other) = default;

  // // Delete the copy constructor and copy assignment operator
  // WiiMixCommonSettings(const WiiMixCommonSettings&) = delete;
  // WiiMixCommonSettings& operator=(const WiiMixCommonSettings&) = delete;

  // // Optionally, you can provide a move constructor and move assignment operator if needed:
  // WiiMixCommonSettings(WiiMixCommonSettings&&) noexcept = default;
  // WiiMixCommonSettings& operator=(WiiMixCommonSettings&&) noexcept = default;

  #define COMMON_SETTINGS_SAVE_STATE_BANK "SAVE_STATE_BANK"
  #define COMMON_SETTINGS_OBJECTIVES "OBJECTIVES"
  #define COMMON_SETTINGS_DIFFICULTY "DIFFICULTY"
  #define COMMON_SETTINGS_OBJECTIVE_TYPES "OBJECTIVE_TYPES"
  #define COMMON_SETTINGS_GAME_GENRES "GAME_GENRES"
  #define COMMON_SETTINGS_GAMES_LIST "GAMES_LIST"
  #define COMMON_SETTINGS_LOCAL_OBJECTIVES "LOCAL_OBJECTIVES"

  virtual void SetSaveStateBank(WiiMixEnums::SaveStateBank bank);
  virtual void SetDifficulty(WiiMixEnums::Difficulty difficulty);
  virtual void SetObjectiveTypes(std::vector<WiiMixEnums::ObjectiveType> types);
  virtual void SetGameGenres(std::vector<WiiMixEnums::GameGenre> genres);
  // Time will be an optional parameter that the user can set in case they only have a certain amount of time
  // It will be taken into account when populating objectives
  // void SetTime(int time);
  // Note that both version and region are checked manually against the roms that the user has
  // and the supported game hash in the retroachievements database. If the hash is not the same,
  // then achievements for the game will not be used for the sake of stability.
  //   void SetVersion();
  //   void SetRegion();

  void SetObjectives(std::vector<WiiMixObjective> objectives); // A list of objectives; bingo objectives are read from left to right on the bingo board
  void AddObjective(WiiMixObjective objective);
  [[nodiscard]] WiiMixEnums::Difficulty GetDifficulty() const;
  // int GetTime();
  [[nodiscard]] WiiMixEnums::SaveStateBank GetSaveStateBank() const;
  [[nodiscard]] std::vector<WiiMixEnums::ObjectiveType> GetObjectiveTypes() const;
  [[nodiscard]] std::vector<WiiMixEnums::GameGenre> GetGameGenres() const;
  [[nodiscard]] std::vector<WiiMixObjective> GetObjectives() const;

  static QString DifficultyToString(WiiMixEnums::Difficulty difficulty);
  static WiiMixEnums::Difficulty StringToDifficulty(QString difficulty);

  static QString SaveStateBankToString(WiiMixEnums::SaveStateBank bank);
  static WiiMixEnums::SaveStateBank StringToSaveStateBank(QString bank);

  static std::vector<WiiMixObjective> ObjectiveIdsToObjectives(std::string objective_ids_list);
  // Using the built-in Objective::ToJson() function as opposed to the below function
  // static std::string ObjectivesToObjectiveIds(std::vector<WiiMixObjective> objectives);

  QJsonObject ToJsonCommon();
  static WiiMixCommonSettings FromJsonCommon(QJsonDocument settings_json);
  // WiiMixSettings FromJsonCommon(QJsonDocument settings_json);

  // NOTE: it would probably be easier to work with these signals
  // rather than the current implementation in ConfigWidget where each change causes a rebuild and update
  // signals:
  //   void SettingsChanged(WiiMixEnums::Difficulty difficulty);
  //   void SettingsChanged(WiiMixEnums::SaveStateBank bank);
  //   void SettingsChanged(std::vector<WiiMixObjective> objectives);

protected:
  WiiMixEnums::Difficulty m_difficulty;
  WiiMixEnums::SaveStateBank m_save_state_bank;
  std::vector<WiiMixEnums::ObjectiveType> m_objective_types;
  std::vector<WiiMixEnums::GameGenre> m_game_genres;

private:
  std::vector<WiiMixObjective> m_objectives;

  int m_time; // unused for now
};
