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
  // Bingo lobby ids encode game information of all the selected game ids
  // as well as have a unique random identifier at the end
  // For example: 5000010000200003000042030234039204
  // 5: # of selected games
  // 00001: game id 1
  // 00002: game id 2
  // 00003: game id 3
  // 00004: game id 4
  // 20302: game id 5
  // 34039204: random 8 digit identifier
  // The LineEdit fields for connecting to bingo 
  // has a validator that checks against the current games you have enabled for wiimix,
  // preventing connections to lobbies where the same games aren't enabled
  // Also, the LineEdit field for hosting prevents hosting if no games are enabled
  // or if the sum total of achievements of all enabled games < size of bingo board 
  void SetLobbyID(QString value);
  QString GetLobbyPassword();
  void SetLobbyPassword(QString value);
  QString GetSeed();
  // Example seed: 50349569036230058245023057008257096345043578069584059874 
  // 9: size of the bingo board
  // 349569: achievement 1
  // 036230: 2
  // 058245: 3
  // 023057: 4
  // 008257: 5
  // 096345: 6
  // 043578: 7
  // 069584: 8
  // 059874: 9
  void SetSeed(QString value);
  
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
  QString m_seed;
};
