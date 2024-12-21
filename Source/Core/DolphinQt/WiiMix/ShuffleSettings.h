// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Settings.h"

#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

// CLIENT SHUFFLE SETTINGS -> singleton
class WiiMixShuffleSettings : public WiiMixSettings 
{
public:
  static WiiMixShuffleSettings* instance(
      int number_of_switches = DEFAULT_NUMBER_OF_SWITCHES, 
      int min_time_between_switch = DEFAULT_MIN_SWITCH_TIME, 
      int max_time_between_switch = DEFAULT_MAX_SWITCH_TIME, 
      bool endless = DEFAULT_IS_ENDLESS) {
    if (!s_instance) {
        s_instance = new WiiMixShuffleSettings(number_of_switches, min_time_between_switch, max_time_between_switch, endless);
    }
    return static_cast<WiiMixShuffleSettings*>(s_instance);
  }

  explicit WiiMixShuffleSettings(int number_of_switches = DEFAULT_NUMBER_OF_SWITCHES, 
    int min_time_between_switch = DEFAULT_MIN_SWITCH_TIME, 
    int max_time_between_switch = DEFAULT_MAX_SWITCH_TIME, 
    bool endless = DEFAULT_IS_ENDLESS);

  // Note that EITHER the number of switches OR the number of objectives gets set
  // It doesn't make sense for both to be set
  bool GetNumberOfSwitches() const;
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
  int m_number_of_switches;
  int m_min_time_between_switch;
  int m_max_time_between_switch;
  bool m_endless;
};
