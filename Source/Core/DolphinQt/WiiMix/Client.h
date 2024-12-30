// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonDocument>

#include <iostream>
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <netinet/in.h>
  #include <unistd.h>
#endif

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Objective.h"
#include "DolphinQt/WiiMix/BingoSettings.h"

// This is a file for syncing bingo cards between players
// This is also a singleton; you can only be connected to ONE lobby at a time
class WiiMixClient : public QObject
{
  Q_OBJECT

public:
  WiiMixClient(const WiiMixClient& client) = delete;

  static WiiMixClient* instance() {
    if (!s_instance) {
      s_instance = new WiiMixClient();
    }
    return s_instance;
  }

  // explicit WiiMixClient(QObject *parent = nullptr, QTcpSocket *socket = nullptr);
  
  // Simplified it to just sending and receiving settings and an action
  // The settings are encoded and decoded to and from json

  // Note: these methods and signals could be overloaded to handle netplay for other modes
  // Bingo is the only one that has netplay currently implemented
  // bool HasNetworkConnection();
  bool IsConnected() const;
  bool SendData(QJsonObject obj, WiiMixEnums::Action action);
  bool ReceiveData(QJsonDocument doc);
  bool ReceiveData(QJsonDocument doc, QByteArray file);
  bool ConnectToServer();

signals:
  void onSettingsChanged(WiiMixBingoSettings* settings);
  void onGetObjectiveDBReceived(QJsonDocument json);
  void onGetPlayersReceived(QJsonDocument json);
  void onGetObjectiveHistoryReceived(QJsonDocument json);
  
  void onError(QString error);

protected:
  explicit WiiMixClient(QObject *parent = nullptr, QTcpSocket *socket = nullptr);

  inline static WiiMixClient* s_instance = nullptr;

// If desired, this can be optimized by storing in members and only sending the updated data
// But for simplicity and to reduce desync issues, we're currently sending the entire card each time
private:
  QTcpSocket* m_socket;
  //     std::list<WiiMixEnums::Player> m_players;
  //     std::list<WiiMixObjective> m_bingo_card;
  //     WiiMixEnums::BingoType m_bingo_type;
};
