// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QString>

// The server has no GUI support
// So the preprocessor is used to weed out GUI functionality
// and replace it with server functionality
// This feels really hacky; the number of preprocessing statements can also be reduced

#include <QIcon>

#include <QJsonObject>
#include <QJsonDocument>

#include <algorithm>
#include <random>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Objective.h"
#include "DolphinQt/WiiMix/Player.h"

#include "UICommon/GameFile.h"

// This is a singleton class that holds the global settings for WiiMix
class WiiMixGlobalSettings : public QObject {
  Q_OBJECT

public:
  // Singleton; delete the copy constructor to prevent copies
  // This is only on client side; the server doesn't need a singleton
  WiiMixGlobalSettings(const WiiMixGlobalSettings& settings) = delete;

  // Singleton accessor method
  static WiiMixGlobalSettings* instance() {
      if (!s_instance) {
          s_instance = new WiiMixGlobalSettings();  // Create the singleton if it doesn't exist
      }
      return s_instance;
  }

  // Time will be an optional parameter that the user can set in case they only have a certain amount of time
  // It will be taken into account when populating objectives
  // void SetTime(int time);
  // Note that both version and region are checked manually against the roms that the user has
  // and the supported game hash in the retroachievements database. If the hash is not the same,
  // then achievements for the game will not be used for the sake of stability.
  //   void SetVersion();
  //   void SetRegion();
  void SetMode(WiiMixEnums::Mode mode);
  // Note that SetGamesList will be called if the list of wiimix games needs to be changed for netplay reasons
  // The function will ALSO update the values per config file and therefore update the GUI
  void SetGamesList(std::vector<std::shared_ptr<const UICommon::GameFile>> game_list);
  void AddGame(const std::shared_ptr<const UICommon::GameFile>& game);
  void UpdateGame(const std::shared_ptr<const UICommon::GameFile>& game);
  void RemoveGame(const std::string& path);
  int FindGameIndex(const std::string& path) const;

  const WiiMixEnums::Mode GetMode() const;

  static QString ModeToTitle(WiiMixEnums::Mode mode);
  static QIcon ModeToIcon(WiiMixEnums::Mode mode);
  static QString ModeToDescription(WiiMixEnums::Mode mode);
  static WiiMixEnums::Mode StringToMode(QString mode);

  static std::vector<std::shared_ptr<const UICommon::GameFile>> GameIdsToGameFiles(std::string game_ids_list);
  static std::string GameFilesToGameIds(std::vector<std::shared_ptr<const UICommon::GameFile>> games);
  const std::vector<std::shared_ptr<const UICommon::GameFile>> GetGamesList() const;

  void SetCurrentObjective(int objective);
  int GetCurrentObjective();

  void SetPlayer(WiiMixPlayer *player);
  WiiMixPlayer* GetPlayer();

  // QJsonObject ToJsonCommon();
  // void FromJsonCommon(QJsonDocument settings_json);

// SettingsChanged signals update the UI
// but also send a request to the WiiMixClient to propagate the new values to the server

// NOTE: it would probably be easier to work with these signals
// rather than the current implementation in ConfigWidget where each change causes a rebuild and update
signals:
//   void SettingsChanged(WiiMixEnums::Difficulty difficulty);
//   void SettingsChanged(WiiMixEnums::Mode mode);
//   void SettingsChanged(WiiMixEnums::SaveStateBank bank);
//   void SettingsChanged(std::vector<WiiMixObjective> objectives);
//   // For if connecting to a lobby and accepting the prompt regarding changes to your game list
  void SettingsChanged(std::vector<std::shared_ptr<const UICommon::GameFile>> games);

protected:
  // The default constructor is protected, as there should only be one instance of WiiMixSettings
  // It should only be called within the classes that extend it

  explicit WiiMixGlobalSettings(WiiMixEnums::Mode mode = DEFAULT_MODE, std::vector<std::shared_ptr<const UICommon::GameFile>> games = DEFAULT_GAMES, int current_objective = 0, WiiMixPlayer *player = nullptr);

  inline static WiiMixGlobalSettings* s_instance = nullptr; // Singleton instance

private:
  WiiMixEnums::Mode m_mode;
  std::vector<std::shared_ptr<const UICommon::GameFile>> m_games;
  int m_current_objective;
  WiiMixPlayer *m_player;
};
