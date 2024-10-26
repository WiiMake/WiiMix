// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "DolphinQt/WiiMix/Enums.h"

// This is a file for syncing bingo cards between players
class WiiMixBingoClient
{
public:
  WiiMixBingoClient();

  void Connect(QString lobby_name, QString lobby_password);

  // Bingo card objectives are read top to bottom left to right and are 0 indexed
  void UpdateBingoCard(int objective, WiiMixEnums::Player player);

  void ReceiveBingoCard();

  void ConnectedToLobby(WiiMixEnums::Player player);

private:

    int m_client_socket;
    sockaddr_in m_server_address;
};