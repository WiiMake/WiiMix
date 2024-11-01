// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Objective.h"

// A server for all your WiiMix-related needs

class WiiMixServer : public QObject
{
  Q_OBJECT

public:
  explicit WiiMixServer(QObject *parent = nullptr, WiiMixEnums::BingoType bingo_type = WiiMixEnums::BingoType::BINGO, std::list<WiiMixEnums::Player> players = {}, std::list<WiiMixObjective> bingo_card = {});

  // void CreateLobby(QString lobby_name, QString lobby_password);
  
  // // Bingo card objectives are read top to bottom left to right and are 0 indexed
  // void SetBingoType(WiiMixEnums::BingoType bingo_type);
  // WiiMixEnums::BingoType GetBingoType() const;

  // void SetPlayers(const std::list<WiiMixEnums::Player>& players);
  // void AddPlayer(WiiMixEnums::Player player);
  // void RemovePlayer(WiiMixEnums::Player player);
  // std::list<WiiMixEnums::Player> GetPlayers() const;

  // void SetBingoCard(const std::list<WiiMixObjective>& bingo_card);
  // void UpdateBingoCard(int objective, WiiMixEnums::Player player);
  // std::list<WiiMixObjective> GetBingoCard() const;

signals:
  void DataReceived();

private slots:
    void NewConnection();
    void Disconnected();
    void ReadyRead();

private:
    QTcpServer *m_server;
    WiiMixEnums::BingoType m_bingo_type;
    std::list<WiiMixEnums::Player> m_players;
    std::list<WiiMixObjective> m_bingo_card;
};