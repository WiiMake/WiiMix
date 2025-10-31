// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/CommonSettings.h"

#include <QList>
#include <QString>

#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

// CLIENT ROGUE SETTINGS -> singleton
class WiiMixRogueSettings : public WiiMixCommonSettings 
{
public:
  static WiiMixRogueSettings* instance(WiiMixEnums::Difficulty difficulty = DEFAULT_ROGUE_DIFFICULTY, WiiMixEnums::SaveStateBank save_state_bank = DEFAULT_ROGUE_SAVE_STATE_BANK, std::vector<WiiMixObjective> objectives = DEFAULT_OBJECTIVES, std::vector<WiiMixEnums::ObjectiveType> types = DEFAULT_OBJECTIVE_TYPES, std::vector<WiiMixEnums::GameGenre> genres = DEFAULT_GAME_GENRES, WiiMixEnums::RogueLength rogue_length = DEFAULT_ROGUE_LENGTH, int num_players = DEFAULT_NUM_PLAYERS_ROGUE, WiiMixEnums::MultiplayerMode multiplayer_mode = DEFAULT_MULTIPLAYER_MODE_ROGUE) {
    // Check if difficulty or save state bank are different from the default
    WiiMixEnums::Difficulty config_difficulty = Config::Get(Config::WIIMIX_ROGUE_DIFFICULTY);
    if (difficulty == DEFAULT_ROGUE_DIFFICULTY && config_difficulty != DEFAULT_ROGUE_DIFFICULTY) {
        difficulty = config_difficulty;
    }

    WiiMixEnums::SaveStateBank config_save_state_bank = Config::Get(Config::WIIMIX_ROGUE_SAVE_STATE_BANK);
    if (save_state_bank == DEFAULT_ROGUE_SAVE_STATE_BANK && config_save_state_bank != DEFAULT_ROGUE_SAVE_STATE_BANK) {
        save_state_bank = config_save_state_bank;
    }

    std::vector<WiiMixEnums::ObjectiveType> config_objective_types = WiiMixEnums::ObjectiveTypesFromString(Config::Get(Config::WIIMIX_ROGUE_OBJECTIVE_TYPES));
    if (types.empty() && !config_objective_types.empty()) {
      types = config_objective_types;
    }

    std::vector<WiiMixEnums::GameGenre> config_genres = WiiMixEnums::GameGenresFromString(Config::Get(Config::WIIMIX_ROGUE_GAME_GENRES));
    if (genres.empty() && !config_genres.empty()) {
      genres = config_genres;
    }

    if (!s_instance) {
        s_instance = new WiiMixRogueSettings(difficulty, save_state_bank, objectives, types, genres, rogue_length);
    }
    return s_instance;
  }

  explicit WiiMixRogueSettings(WiiMixEnums::Difficulty difficulty, WiiMixEnums::SaveStateBank bank, std::vector<WiiMixObjective> objectives, std::vector<WiiMixEnums::ObjectiveType> types, std::vector<WiiMixEnums::GameGenre> genres, WiiMixEnums::RogueLength length, int num_players = DEFAULT_NUM_PLAYERS_ROGUE, WiiMixEnums::MultiplayerMode multiplayer_mode = DEFAULT_MULTIPLAYER_MODE_ROGUE);

  #define ROGUE_SETTINGS "ROGUE_SETTINGS"
  #define ROGUE_SETTINGS_SEED "SEED"
  #define ROGUE_SETTINGS_DIFFICULTY "DIFFICULTY"
  #define ROGUE_SETTINGS_SAVE_STATE_BANK "SAVE_STATE_BANK"
  #define ROGUE_SETTINGS_LENGTH "LENGTH"
  #define ROGUE_SETTINGS_NUM_PLAYERS "NUM_PLAYERS"

  // Seeds will be encoded and decoded using Qt
  QString GetSeed();
  static int GetNumPlayersFromSeed(QString seed);
  // Example seed: 50402523039501042031012343722364155 (34 digits long)
  // 5: Length; the number of events before the finale
  // 040252: the id of achievement 1
  // 303950: the id of achievement 2
  // 104203: the id of achievement 3
  // 1: the number of boss achievements
  // 012345: the boss achievement
  // 3: the id of event 1
  // 7: the id of event 2
  // 2: the id of event 3
  // 2: the position of the first item selection
  // 2: the first item offered for item 1
  // 3: the 2nd item offered
  // 6: the 3rd item offered
  // 4: the position of the second item selection
  // 1: 1st item
  // 5: 2nd item
  // 5: 3rd item
  void SetSeed(QString string);
  struct RogueTile;

  static WiiMixRogueSettings* SettingsFromSeed(QString seed);
  static std::vector<int> SeedToObjectives(QString seed);
  static std::vector<int> SeedToBossObjectives(QString seed);
  static bool VerifyRogueSeed(std::string seed);
  static std::string RogueTilesToSeed(std::vector<RogueTile> tiles, WiiMixEnums::Difficulty difficulty, int num_players, WiiMixEnums::RogueLength length);

  static QString LengthToString(WiiMixEnums::RogueLength length);
  static WiiMixEnums::RogueLength StringToLength(QString length);
  static int LengthToNumObjectives(WiiMixEnums::RogueLength length);

  QList<WiiMixEnums::RogueEvent> GetEvents();
  WiiMixEnums::RogueEvent GetEvent();
  void SetEvents(QList<WiiMixEnums::RogueEvent> events);
  QList<WiiMixEnums::RogueItem> GetItemSet();
  void SetItems(QList<WiiMixEnums::RogueItem>);
  WiiMixEnums::RogueItem GetItem();
  QList<WiiMixEnums::RogueItem> GetPlayerItems();
  void SetPlayerItems(QList<WiiMixEnums::RogueItem> items);
  void AddPlayerItem(WiiMixEnums::RogueItem item);
  QList<WiiMixEnums::RogueItem> Get3Items();
  WiiMixEnums::RogueLength GetLength();
  void SetLength(WiiMixEnums::RogueLength length);
  void SetDifficulty(WiiMixEnums::Difficulty difficulty) override;
  void SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) override;
  void SetObjectiveTypes(std::vector<WiiMixEnums::ObjectiveType> types) override;
  void SetGameGenres(std::vector<WiiMixEnums::GameGenre> genres) override;

  int GetNumPlayers() const;
  void SetNumPlayers(int value);
  WiiMixEnums::MultiplayerMode GetMultiplayerMode() const;
  void SetMultiplayerMode(WiiMixEnums::MultiplayerMode multiplayer_mode);
  
  QJsonDocument ToJson();
  void FromJson(QJsonDocument json);

private:
  inline static WiiMixRogueSettings* s_instance = nullptr; // Singleton instance
  QList<WiiMixEnums::RogueEvent> m_events = QList<WiiMixEnums::RogueEvent>();
  QList<WiiMixEnums::RogueItem> m_items = QList<WiiMixEnums::RogueItem>();
  QList<WiiMixEnums::RogueItem> m_player_items = QList<WiiMixEnums::RogueItem>();
  WiiMixEnums::RogueLength m_length;
  // Endless can potentially come later
  // bool m_endless;
  int m_num_players;
  WiiMixEnums::MultiplayerMode m_multiplayer_mode = WiiMixEnums::MultiplayerMode::COOP;
  QString m_seed = QStringLiteral("");

};
