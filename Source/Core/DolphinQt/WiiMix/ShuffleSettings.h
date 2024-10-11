// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/Settings.h"

#pragma once

class WiiMixShuffleSettings : public WiiMixSettings 
{
public:
  explicit WiiMixShuffleSettings();
  // Note that EITHER the number of switches OR the number of objectives gets set
  // It doesn't make sense for both to be set
  bool GetNumberOfSwitches();
  void SetNumberOfSwitches(int value);
  bool GetTimeBetweenSwitch();
  void SetTimeBetweenSwitch(int value);
  bool GetEndless(); // The user(s) can play as long as they want
  void SetEndless(bool value);

private:
  int m_number_of_switches;
  int m_time_between_switch;
  bool m_endless;
};
