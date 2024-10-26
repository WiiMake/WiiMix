// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// This is a file for syncing bingo cards between players
class WiiMixNetworkUtils
{
public:
  WiiMixNetworkUtils();

  static void* GetInAddr(struct sockaddr *sa);

};