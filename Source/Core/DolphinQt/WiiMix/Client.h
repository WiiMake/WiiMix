// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QDebug>
#include <QJsonDocument>
#include <QByteArray>
#include <QTimer>
#include <QList>

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
#include "DolphinQt/WiiMix/RogueSettings.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"
#include "DolphinQt/WiiMix/Player.h"

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
  // bool ReceiveData(QJsonDocument doc, std::vector<QByteArray> files);
  bool ConnectToServer();
  QTcpSocket* GetSocket();

signals:
  // Update the bingo settings as they are changed for all players
  void onUpdateBingoConfig(WiiMixBingoSettings* settings);
  void onUpdateBingoObjectives(WiiMixBingoSettings* settings);
  void onUpdateRogueObjectives(WiiMixRogueSettings* settings);
  void onUpdateShuffleObjectives(WiiMixShuffleSettings* settings);
  void onGetPlayers(std::vector<WiiMixPlayer> players);
  void onGetPlayer(WiiMixPlayer player);
  void onGetObjectiveHistory(std::vector<WiiMixObjective> objectives);
  void onGetObjectives(std::vector<WiiMixObjective> objectives);
  // void onCalculatedTotalSize(int totalSize);
  void onBytesWritten(int bytesWritten, int totalBytes);
  void onBytesRead(int bytesRead, int totalBytes);

  void onError(QString error);

public slots:
  void BytesWritten();

protected:
  explicit WiiMixClient(QObject *parent = nullptr, QTcpSocket *socket = nullptr);

  inline static WiiMixClient* s_instance = nullptr;

// If desired, this can be optimized by storing in members and only sending the updated data
// But for simplicity and to reduce desync issues, we're currently sending the entire card each time
private:
  QTcpSocket* m_socket;
  qint64 m_bytes_written = 0;
  // For writing
  QByteArray m_data = {};
  // For reading
  QJsonDocument m_json = {};
  int m_data_size = 0;
  int m_json_size = 0;
  int m_files_size = 0;
  int m_current_pos = 0;
  QList<int> m_file_sizes = QList<int>();
  QByteArray m_file = {};
  int m_current_file = 0; 
  std::vector<WiiMixObjective> m_objectives;
  //     std::list<WiiMixEnums::Player> m_players;
  //     std::list<WiiMixObjective> m_bingo_card;
  //     WiiMixEnums::BingoType m_bingo_type;
};
