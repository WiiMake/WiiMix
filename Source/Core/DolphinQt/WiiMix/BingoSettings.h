// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/Settings.h"

#pragma once

class WiiMixBingoSettings : public WiiMixSettings 
{
public:
  explicit WiiMixBingoSettings(const WiiMixSettings& settings, bool is_lockout = false, int card_size = 25);
  bool GetLockout() const;
  void SetLockout(bool value);
  // Options are 3x3, 5x5, 7x7, but they correspond to 9, 25, 49
  int GetCardSize() const;
  void SetCardSize(int value);
  // Bingo only really works over the internet, so when a settings file is shared
  // others can load it, and if they have a network connection plus the corresponding versions
  // of games satisfied by the objectives, they will be eligible to join. Otherwise, 
  // they will get notified and asked to update a game, connect to the internet, etc
  // This info is not stored in bingo settings, but rather managed via BingoNetplay

private:
  bool m_is_lockout;
  int m_card_size;
};
