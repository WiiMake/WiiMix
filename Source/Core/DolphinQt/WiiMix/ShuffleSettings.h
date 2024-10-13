// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Settings.h"

#pragma once

class WiiMixShuffleSettings : public WiiMixSettings 
{
public:
  explicit WiiMixShuffleSettings(const WiiMixSettings& settings, 
    int number_of_switches = DEFAULT_NUMBER_OF_SWITCHES, 
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

private:
  int m_number_of_switches;
  int m_min_time_between_switch;
  int m_max_time_between_switch;
  bool m_endless;
};
