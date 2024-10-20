// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// This is a file for syncing bingo cards between players
class WiiMixBingoNetplay
{
public:
  WiiMixBingoNetplay();

  void Connect();

  void SendBingoCard();

  void ReceiveBingoCard();

private:
    int m_client_socket;
    sockaddr_in m_server_address;
};