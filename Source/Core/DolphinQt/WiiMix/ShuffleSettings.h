// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/CommonSettings.h"

#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

// CLIENT SHUFFLE SETTINGS -> singleton
class WiiMixShuffleSettings : public WiiMixCommonSettings 
{
public:
  static WiiMixShuffleSettings* instance(
      WiiMixEnums::Difficulty difficulty = DEFAULT_SHUFFLE_DIFFICULTY,
      WiiMixEnums::SaveStateBank save_state_bank = DEFAULT_SHUFFLE_SAVE_STATE_BANK,
      std::vector<WiiMixObjective> objectives = DEFAULT_OBJECTIVES,
      std::vector<WiiMixEnums::ObjectiveType> types = DEFAULT_OBJECTIVE_TYPES,
      std::vector<WiiMixEnums::GameGenre> genres = DEFAULT_GAME_GENRES,
      int number_of_switches = DEFAULT_NUMBER_OF_SWITCHES, 
      int min_time_between_switch = DEFAULT_MIN_SWITCH_TIME, 
      int max_time_between_switch = DEFAULT_MAX_SWITCH_TIME,
      int num_players = DEFAULT_NUM_PLAYERS_SHUFFLE,
      bool endless = DEFAULT_IS_ENDLESS) {
    // Check if difficulty, save state bank, objective types, or game genres are different from the default
    WiiMixEnums::Difficulty config_difficulty = Config::Get(Config::WIIMIX_SHUFFLE_DIFFICULTY);
    if (difficulty == DEFAULT_SHUFFLE_DIFFICULTY && config_difficulty != DEFAULT_SHUFFLE_DIFFICULTY) {
        difficulty = config_difficulty;
    }

    WiiMixEnums::SaveStateBank config_save_state_bank = Config::Get(Config::WIIMIX_SHUFFLE_SAVE_STATE_BANK);
    if (save_state_bank == DEFAULT_SHUFFLE_SAVE_STATE_BANK && config_save_state_bank != DEFAULT_SHUFFLE_SAVE_STATE_BANK) {
        save_state_bank = config_save_state_bank;
    }

    std::vector<WiiMixEnums::ObjectiveType> config_objective_types = WiiMixEnums::ObjectiveTypesFromString(Config::Get(Config::WIIMIX_SHUFFLE_OBJECTIVE_TYPES));
    if (types.empty() && config_objective_types.size() > 0) {
      types = config_objective_types;
    }

    std::vector<WiiMixEnums::GameGenre> config_game_genres = WiiMixEnums::GameGenresFromString(Config::Get(Config::WIIMIX_SHUFFLE_GAME_GENRES));
    if (genres.empty() && config_game_genres.size() > 0) {
      genres = config_game_genres;
    }

    if (!s_instance) {
        s_instance = new WiiMixShuffleSettings(difficulty, save_state_bank, objectives, types, genres, number_of_switches, min_time_between_switch, max_time_between_switch, num_players, endless);
    }
    return s_instance;
  }

  #define SHUFFLE_SETTINGS "SHUFFLE_SETTINGS"
  #define SHUFFLE_SETTINGS_NUMBER_OF_SWITCHES "NUMBER_OF_SWITCHES"
  #define SHUFFLE_SETTINGS_MIN_TIME_BETWEEN_SWITCH "MIN_TIME_BETWEEN_SWITCH"
  #define SHUFFLE_SETTINGS_MAX_TIME_BETWEEN_SWITCH "MAX_TIME_BETWEEN_SWITCH"
  #define SHUFFLE_SETTINGS_IS_ENDLESS "IS_ENDLESS"
  #define SHUFFLE_SETTINGS_NUM_PLAYERS "NUM_PLAYERS"
  #define SHUFFLE_SETTINGS_MULTIPLAYER_MODE "MULTIPLAYER_MODE"

  explicit WiiMixShuffleSettings(WiiMixEnums::Difficulty difficulty = DEFAULT_SHUFFLE_DIFFICULTY,
      WiiMixEnums::SaveStateBank save_state_bank = DEFAULT_SHUFFLE_SAVE_STATE_BANK,
      std::vector<WiiMixObjective> objectives = DEFAULT_OBJECTIVES, std::vector<WiiMixEnums::ObjectiveType> objective_types = DEFAULT_OBJECTIVE_TYPES, std::vector<WiiMixEnums::GameGenre> game_genres = DEFAULT_GAME_GENRES, int number_of_switches = DEFAULT_NUMBER_OF_SWITCHES, 
    int min_time_between_switch = DEFAULT_MIN_SWITCH_TIME, 
    int max_time_between_switch = DEFAULT_MAX_SWITCH_TIME, 
    int num_players = DEFAULT_NUM_PLAYERS_SHUFFLE,
    bool endless = DEFAULT_IS_ENDLESS,
    WiiMixEnums::MultiplayerMode multiplayer_mode = WiiMixEnums::MultiplayerMode::VERSUS);

  // Note that EITHER the number of switches OR the number of objectives gets set
  // It doesn't make sense for both to be set
  int GetNumberOfSwitches() const;
  void SetNumberOfSwitches(int value);
  int GetMinTimeBetweenSwitch() const;
  void SetMinTimeBetweenSwitch(int value);
  int GetMaxTimeBetweenSwitch() const;
  void SetMaxTimeBetweenSwitch(int value);
  int GetNumPlayers() const;
  void SetNumPlayers(int value);
  WiiMixEnums::MultiplayerMode GetMultiplayerMode() const;
  void SetMultiplayerMode(WiiMixEnums::MultiplayerMode multiplayer_mode);
  bool GetEndless() const; // The user(s) can play as long as they want
  void SetEndless(bool value);
  void SetDifficulty(WiiMixEnums::Difficulty difficulty) override;
  void SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) override;
  void SetObjectiveTypes(std::vector<WiiMixEnums::ObjectiveType> types) override;
  void SetGameGenres(std::vector<WiiMixEnums::GameGenre> genres) override;

  QJsonDocument ToJson();
  void FromJson(QJsonDocument json);


private:
  inline static WiiMixShuffleSettings* s_instance = nullptr; // Singleton instance
  int m_number_of_switches;
  int m_min_time_between_switch;
  int m_max_time_between_switch;
  int m_num_players;
  bool m_endless;
  WiiMixEnums::MultiplayerMode m_multiplayer_mode = WiiMixEnums::MultiplayerMode::VERSUS;
};
