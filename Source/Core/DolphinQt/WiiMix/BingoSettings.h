// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Settings.h"

#pragma once

class WiiMixBingoSettings : public WiiMixSettings 
{
public:
  explicit WiiMixBingoSettings(const WiiMixSettings& settings, WiiMixEnums::BingoType bingo_type = DEFAULT_BINGO_TYPE, int card_size = DEFAULT_CARD_SIZE);
  WiiMixEnums::BingoType GetBingoType() const;
  void SetBingoType(WiiMixEnums::BingoType value);
  // Options are 3x3, 5x5, 7x7, but they correspond to 9, 25, 49
  int GetCardSize() const;
  void SetCardSize(int value);
  bool GetTeams();
  void SetTeams(bool value);
  // Bingo only really works over the internet, so when a settings file is shared
  // others can load it, and if they have a network connection plus the corresponding versions
  // of games satisfied by the objectives, they will be eligible to join. Otherwise, 
  // they will get notified and asked to update a game, connect to the internet, etc
  // This info is not stored in bingo settings, but rather managed via BingoNetplay

private:
  WiiMixEnums::BingoType m_bingo_type;
  int m_card_size;
  bool m_teams;
};
