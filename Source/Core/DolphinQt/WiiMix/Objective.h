// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"

class WiiMixObjective 
{
public:
  explicit WiiMixObjective(
    std::string objective_string, 
    WiiMixEnums::Player completed
  );

  std::string GetObjectiveString();
  WiiMixEnums::Player GetCompleted();
  void SetCompleted(WiiMixEnums::Player player);
  
private:
  std::string m_objective_string;
  WiiMixEnums::Player m_completed;
};
