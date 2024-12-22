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
      int number_of_switches = DEFAULT_NUMBER_OF_SWITCHES, 
      int min_time_between_switch = DEFAULT_MIN_SWITCH_TIME, 
      int max_time_between_switch = DEFAULT_MAX_SWITCH_TIME,
      bool endless = DEFAULT_IS_ENDLESS) {
    // Check if difficulty or save state bank are different from the default
    WiiMixEnums::Difficulty config_difficulty = Config::Get(Config::WIIMIX_SHUFFLE_DIFFICULTY);
    if (difficulty == DEFAULT_SHUFFLE_DIFFICULTY && config_difficulty != DEFAULT_SHUFFLE_DIFFICULTY) {
        difficulty = difficulty;
    }

    WiiMixEnums::SaveStateBank config_save_state_bank = Config::Get(Config::WIIMIX_SHUFFLE_SAVE_STATE_BANK);
    if (save_state_bank == DEFAULT_SHUFFLE_SAVE_STATE_BANK && config_save_state_bank != DEFAULT_SHUFFLE_SAVE_STATE_BANK) {
        save_state_bank = config_save_state_bank;
    }
    if (!s_instance) {
        s_instance = new WiiMixShuffleSettings(difficulty, save_state_bank, objectives, number_of_switches, min_time_between_switch, max_time_between_switch, endless);
    }
    return s_instance;
  }

  explicit WiiMixShuffleSettings(WiiMixEnums::Difficulty difficulty = DEFAULT_SHUFFLE_DIFFICULTY,
      WiiMixEnums::SaveStateBank save_state_bank = DEFAULT_SHUFFLE_SAVE_STATE_BANK,
      std::vector<WiiMixObjective> objectives = DEFAULT_OBJECTIVES, int number_of_switches = DEFAULT_NUMBER_OF_SWITCHES, 
    int min_time_between_switch = DEFAULT_MIN_SWITCH_TIME, 
    int max_time_between_switch = DEFAULT_MAX_SWITCH_TIME, 
    bool endless = DEFAULT_IS_ENDLESS);

  // Note that EITHER the number of switches OR the number of objectives gets set
  // It doesn't make sense for both to be set
  int GetNumberOfSwitches() const;
  void SetNumberOfSwitches(int value);
  int GetMinTimeBetweenSwitch() const;
  void SetMinTimeBetweenSwitch(int value);
  int GetMaxTimeBetweenSwitch() const;
  void SetMaxTimeBetweenSwitch(int value);
  bool GetEndless() const; // The user(s) can play as long as they want
  void SetEndless(bool value);
  void SetDifficulty(WiiMixEnums::Difficulty difficulty) override;

  void SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) override;

private:
  inline static WiiMixShuffleSettings* s_instance = nullptr; // Singleton instance
  int m_number_of_switches;
  int m_min_time_between_switch;
  int m_max_time_between_switch;
  bool m_endless;
};
