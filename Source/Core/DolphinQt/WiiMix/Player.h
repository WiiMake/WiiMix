// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include <map>
#include <QJsonObject>
#include <string>

class WiiMixPlayer
{
public:
  explicit WiiMixPlayer(
    uint16_t id,
    std::string username,
    std::string password_hash,
    WiiMixEnums::Role role = WiiMixEnums::Role::USER,
    // These are internal player stats that are stored locally
    // to avoid having to request the server for them every time
    int num_objectives_completed = 0,
    int num_unique_objectives_completed = 0,
    int num_objectives_attempted = 0
  );

  #define PLAYER_ID "id"
  #define PLAYER_USERNAME "username"
  #define PLAYER_PASSWORD_HASH "password_hash"
  #define PLAYER_ROLE "role"
  #define PLAYER_NUM_OBJECTIVES_COMPLETED "num_objectives_completed"
  #define PLAYER_NUM_UNIQUE_OBJECTIVES_COMPLETED "num_unique_objectives_completed"
  #define PLAYER_NUM_OBJECTIVES_ATTEMPTED "num_objectives_attempted"

  uint16_t GetId();
  std::string GetUsername();
  std::string GetPasswordHash();

  void SetNumObjectivesCompleted(int num_objectives_completed);
  int GetNumObjectivesCompleted();

  void SetNumUniqueObjectivesCompleted(int num_unique_objectives_completed);
  int GetNumUniqueObjectivesCompleted();

  void SetNumObjectivesAttempted(int num_objectives_attempted);
  int GetNumObjectivesAttempted();

  QJsonObject ToJson();
  static WiiMixPlayer FromJson(const QJsonObject& obj);

signals:
  void onSetNumObjectivesCompleted(int num_objectives_completed);
  void onSetNumUniqueObjectivesCompleted(int num_unique_objectives_completed);
  void onSetNumObjectivesAttempted(int num_objectives_attempted);

private:
  uint16_t m_id;
  std::string m_username;
  std::string m_password_hash;
  WiiMixEnums::Role m_role;
  int m_num_objectives_completed;
  int m_num_unique_objectives_completed;
  int m_num_objectives_attempted;
};