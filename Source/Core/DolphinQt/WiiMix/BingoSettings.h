// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QJsonDocument>
#include <QString>
#include <QMap>
#include <QPair>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Settings.h"

class WiiMixBingoSettings : public WiiMixSettings 
{
public:
  explicit WiiMixBingoSettings(WiiMixSettings& settings, WiiMixEnums::BingoType bingo_type = DEFAULT_BINGO_TYPE, int card_size = DEFAULT_CARD_SIZE);
  explicit WiiMixBingoSettings(WiiMixEnums::BingoType bingo_type = DEFAULT_BINGO_TYPE, int card_size = DEFAULT_CARD_SIZE);
  
  WiiMixEnums::BingoType GetBingoType() const;
  void SetBingoType(WiiMixEnums::BingoType value);
  // Options are 3x3, 5x5, 7x7, but they correspond to 9, 25, 49
  int GetCardSize() const;
  void SetCardSize(int value);
  bool GetTeams();
  void SetTeams(bool value);
  QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> GetPlayers();
  void AddPlayer(WiiMixEnums::Player player, QPair<WiiMixEnums::Color, QString> value);
  void RemovePlayer(WiiMixEnums::Player player);
  void SetPlayers(QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> value);
  QString GetLobbyID();
  void SetLobbyID(QString value);
  QString GetLobbyPassword();
  void SetLobbyPassword(QString value);
  
  QJsonDocument ToJson();
  WiiMixBingoSettings FromJson(QJsonDocument json); 
  // Bingo only really works over the internet, so when a settings file is shared
  // others can load it, and if they have a network connection plus the corresponding versions
  // of games satisfied by the objectives, they will be eligible to join. Otherwise, 
  // they will get notified and asked to update a game, connect to the internet, etc
  // This info is not stored in bingo settings, but rather managed via BingoNetplay

private:
  WiiMixEnums::BingoType m_bingo_type;
  int m_card_size;
  bool m_teams;
  QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> m_players;
  QString m_lobby_id;
  QString m_lobby_password;
};
