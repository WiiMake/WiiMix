// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPair>

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Objective.h"
#include "DolphinQt/WiiMix/BingoSettings.h"

// A server for all your WiiMix-related needs

class WiiMixServer : public QObject
{
  Q_OBJECT

public:
  // explicit WiiMixServer(QObject *parent = nullptr, WiiMixEnums::BingoType bingo_type = WiiMixEnums::BingoType::BINGO, std::list<WiiMixEnums::Player> players = {}, std::list<WiiMixObjective> bingo_card = {});
  explicit WiiMixServer(QObject *parent = nullptr);

private slots:
    void NewConnection();
    void Disconnected();
    void Read();

private:
    void SendToLobby(const QString& lobby, const QJsonObject& message);
    void ProcessMessage(QTcpSocket* client_socket, const QJsonObject& json);
    
    QTcpServer *m_server;
    QMap<QString, QPair<QSet<QTcpSocket*>, WiiMixBingoSettings>> m_lobbies;  // lobbyName -> list of clients in lobby
    QMap<QTcpSocket*, QString> m_client_lobbies;  // client -> lobbyName
};