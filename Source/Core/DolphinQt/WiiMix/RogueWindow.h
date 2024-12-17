// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/RogueSettings.h"
#include <QDialog>
#pragma once

class WiiMixRogueWindow : public QDialog, WiiMixRogueSettings
{
public:
  explicit WiiMixRogueWindow(const WiiMixRogueSettings &settings);

  static void GetSettings();
  static WiiMixRogueSettings::Event GetEvent();
    static WiiMixRogueSettings::Item GetItem();
private:
  WiiMixRogueSettings m_settings;
};