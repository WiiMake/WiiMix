// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonDocument>

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Objective.h"
#include "DolphinQt/WiiMix/BingoSettings.h"

// This is a file for syncing bingo cards between players
class WiiMixBingoClient : public QObject
{
  Q_OBJECT

public:
  // explicit WiiMixBingoClient(QObject *parent = nullptr, QTcpSocket *socket = nullptr, WiiMixBingoSettings settings = WiiMixBingoSettings());
  explicit WiiMixBingoClient(QObject *parent = nullptr, QTcpSocket *socket = nullptr);
  // bool Connect(WiiMixBingoSettings settings);

  // bool CreateLobby(WiiMixBingoSettings settings);

  // // Bingo card objectives are read top to bottom left to right and are 0 indexed
  // bool Update(int objective, WiiMixEnums::Player player);

  // // Runs when the bingo card is completed
  // bool BingoEnd();
signals:
  void onSettingsChanged(WiiMixBingoSettings settings);

  // Simplified it to just sending and receiving settings and an action
  // The settings are encoded and decoded to and from json
  bool SendData(WiiMixBingoSettings settings, WiiMixEnums::Action action);
  bool ReceiveData(QJsonDocument doc);

  // Getters (from server)
  // WiiMixEnums::BingoType GetBingoType() const;
  // std::list<WiiMixEnums::Player> GetPlayers() const;
  // std::list<WiiMixObjective> GetBingoCard() const;
  // QString GetLobbyID() const;
  // bool GetTeams() const;

// If desired, this can be optimized by storing in members and only sending the updated data
// But for simplicity and to reduce desync issues, we're currently sending the entire card each time
private:
  QTcpSocket* m_socket;
  // WiiMixBingoSettings m_bingo_settings;
//     std::list<WiiMixEnums::Player> m_players;
//     std::list<WiiMixObjective> m_bingo_card;
//     WiiMixEnums::BingoType m_bingo_type;
};