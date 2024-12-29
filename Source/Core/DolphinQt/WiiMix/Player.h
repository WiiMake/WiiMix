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
    std::string password_hash
  );

  uint16_t GetId();
  std::string GetUsername();
  std::string GetPasswordHash();

  QJsonObject ToJson();
  static WiiMixPlayer FromJson(const QJsonObject& obj);

private:
  uint16_t m_id;
  std::string m_username;
  std::string m_password_hash;
};