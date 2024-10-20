// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/BingoNetplay.h"

#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

WiiMixBingoNetplay::WiiMixBingoNetplay() {}

// Client connection
// void WiiMixBingoNetplay::Connect() {
//     // Create a socket
//     m_client_socket = socket(AF_INET, SOCK_STREAM, 0);

//     // Specify the address
//     sockaddr_in serverAddress;
//     serverAddress.sin_family = AF_INET;
//     serverAddress.sin_port = htons(0); // Let the OS choose the port
//     serverAddress.sin_addr.s_addr = INADDR_ANY;

//     // Make the connection
//     connect(m_client_socket, (struct sockaddr*)&serverAddress,
//         sizeof(serverAddress));
// }

void WiiMixBingoNetplay::SendBingoCard() {

}

void WiiMixBingoNetplay::ReceiveBingoCard() {

}