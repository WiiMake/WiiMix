// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <iostream>
#ifdef _WIN32
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <unistd.h>
#endif


// This is a file for syncing bingo cards between players
class WiiMixNetworkUtils
{
public:
  WiiMixNetworkUtils();

  static void* GetInAddr(struct sockaddr *sa);

};
