// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/RogueSettings.h"

#pragma once

class WiiMixRogueWindow : public WiiMixRogueWindow 
{
public:
  explicit WiiMixRogueWindow(const WiiMixRogueSettings& settings);

  static void GetSettings();

private:
  WiiMixRogueSettings m_settings;
};