// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QJsonDocument>
#include <QString>
#include <QMap>
#include <QPair>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/CommonSettings.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

// CLIENT BINGO SETTINGS -> singleton
class WiiMixBingoSettings : public WiiMixCommonSettings 
{
public:
  explicit WiiMixBingoSettings(WiiMixEnums::Difficulty difficulty = DEFAULT_BINGO_DIFFICULTY, WiiMixEnums::SaveStateBank save_state_bank = DEFAULT_BINGO_SAVE_STATE_BANK, std::vector<WiiMixObjective> objectives = DEFAULT_OBJECTIVES, WiiMixEnums::BingoType bingo_type = DEFAULT_BINGO_TYPE, int card_size = DEFAULT_CARD_SIZE, bool teams = DEFAULT_TEAMS);
  static WiiMixBingoSettings* instance(WiiMixEnums::Difficulty difficulty = DEFAULT_BINGO_DIFFICULTY, WiiMixEnums::SaveStateBank save_state_bank = DEFAULT_BINGO_SAVE_STATE_BANK, std::vector<WiiMixObjective> objectives = DEFAULT_OBJECTIVES, WiiMixEnums::BingoType bingo_type = DEFAULT_BINGO_TYPE, int card_size = DEFAULT_CARD_SIZE, bool teams = DEFAULT_TEAMS) {
    WiiMixEnums::Difficulty config_difficulty = Config::Get(Config::WIIMIX_BINGO_DIFFICULTY);
    if (difficulty == DEFAULT_BINGO_DIFFICULTY && config_difficulty != DEFAULT_BINGO_DIFFICULTY) {
        difficulty = difficulty;
    }

    WiiMixEnums::SaveStateBank config_save_state_bank = Config::Get(Config::WIIMIX_BINGO_SAVE_STATE_BANK);
    if (save_state_bank == DEFAULT_BINGO_SAVE_STATE_BANK && config_save_state_bank != DEFAULT_BINGO_SAVE_STATE_BANK) {
        save_state_bank = config_save_state_bank;
    }
    if (!s_instance) {
        s_instance = new WiiMixBingoSettings(difficulty, save_state_bank, objectives, bingo_type, card_size, teams);
    }
    return s_instance;
  }

  #define BINGO_SETTINGS_BINGO_TYPE "BINGO_TYPE"
  #define BINGO_SETTINGS_TEAMS "TEAMS"
  #define BINGO_SETTINGS_CARD_SIZE "CARD_SIZE"
  #define BINGO_SETTINGS_PLAYERS "PLAYERS"
  #define BINGO_SETTINGS_LOBBY_ID "LOBBY_ID"
  #define BINGO_SETTINGS_LOBBY_PASSWORD "LOBBY_PASSWORD"
  #define BINGO_SETTINGS_SEED "SEED"
  #define BINGO_SETTINGS_COLOR "COLOR"
  #define BINGO_SETTINGS_NAME "NAME"
  #define BINGO_SETTINGS_CURRENT_OBJECTIVES "CURRENT_OBJECTIVES"
  #define BINGO_SETTINGS_PLAYERS_READY "PLAYERS_READY"

  #define BINGO_SETTINGS_PLAYER_NUM "PLAYER_NUM"

  WiiMixEnums::BingoType GetBingoType() const;
  void SetBingoType(WiiMixEnums::BingoType value);
  // Options are 3x3, 5x5, 7x7, but they correspond to 9, 25, 49
  int GetCardSize() const;
  void SetCardSize(int value);
  bool GetTeams();
  void SetTeams(bool value);
  QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> GetPlayers();
  void AddPlayer(WiiMixEnums::Player player, QPair<WiiMixEnums::Color, QString> value);
  void RemovePlayer(WiiMixEnums::Player player);
  void SetPlayers(QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> value);
  QString GetLobbyID();
    // Bingo lobby ids encode game information of all the selected game ids
    // as well as have a unique random identifier at the end
    // For example: 5000010000200003000042030234039204
    // 5: # of selected games
    // 00001: game id 1
    // 00002: game id 2
    // 00003: game id 3
    // 00004: game id 4
    // 20302: game id 5
    // 34039204: random 8 digit identifier
    // The LineEdit fields for connecting to bingo
    // has a validator that checks against the current games you have enabled for wiimix,
    // preventing connections to lobbies where the same games aren't enabled
    // Also, the LineEdit field for hosting prevents hosting if no games are enabled
    // or if the sum total of achievements of all enabled games < size of bingo board
  void SetLobbyID(QString value);
  QString GetLobbyPassword();
  void SetLobbyPassword(QString value);
  QString GetSeed();
  // Example seed: 000001000002000003000004000005000006000007000000000000000000
  // The size of the bingo board is implicitly 9 here
  // 000001: objective 1
  // 000002: objective 2
  // 000003: objective 3
  // 000004: objective 4
  // 000005: objective 5
  // 000006: objective 6
  // 000007: objective 7
  // 000000: 000000 is a saved id; it means select a random objective
  // 000000: select a random objective
  void SetSeed(QString value);
  QMap<WiiMixEnums::Player, int> GetCurrentObjectives();
  void SetCurrentObjectives(QMap<WiiMixEnums::Player, int> value);
  void UpdateCurrentObjectives(WiiMixEnums::Player player, int value);
  QMap<WiiMixEnums::Player, bool> GetPlayersReady();
  void SetPlayersReady(QMap<WiiMixEnums::Player, bool> value);
  void UpdatePlayerReady(WiiMixEnums::Player player, bool value);

  static std::vector<int> SeedToObjectives(QString seed);
  static std::string ObjectivesToSeed(std::vector<WiiMixObjective*> objectives);
  static bool VerifySeed(std::string seed);
  
  QJsonDocument ToJson();
  void FromJson(QJsonDocument json);

  static QString BingoTypeToString(WiiMixEnums::BingoType type);
  static WiiMixEnums::BingoType StringToBingoType(QString type);

  void SetDifficulty(WiiMixEnums::Difficulty difficulty);
  void SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank);

  static int StringToCardSize(QString size);

  void SetPlayerNum(WiiMixEnums::Player player);
  WiiMixEnums::Player GetPlayerNum();

  // Bingo only really works over the internet, so when a settings file is shared
  // others can load it, and if they have a network connection plus the corresponding versions
  // of games satisfied by the objectives, they will be eligible to join. Otherwise, 
  // they will get notified and asked to update a game, connect to the internet, etc
  // This info is not stored in bingo settings, but rather managed via BingoNetplay

private:
  inline static WiiMixBingoSettings* s_instance = nullptr; // Singleton instance
  WiiMixEnums::BingoType m_bingo_type;
  int m_card_size;
  // TODOx: teams hasn't been tested at all
  bool m_teams;
  QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> m_players = {};
  WiiMixEnums::Player m_player_num = WiiMixEnums::Player::ONE;
  QMap<WiiMixEnums::Player, int> m_current_objectives = {};
  QMap<WiiMixEnums::Player, bool> m_players_ready = {};
  QString m_lobby_id = QStringLiteral("");
  QString m_lobby_password = QStringLiteral("");
  QString m_seed = QStringLiteral("");
};
