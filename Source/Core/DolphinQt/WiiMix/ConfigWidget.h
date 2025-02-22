// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>
#include <QDialog>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QMenuBar>
#include <QPair>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <QListWidget>

#include "DolphinQt/WiiMix/Client.h"
#include "DolphinQt/WiiMix/BingoSettings.h"
#include "DolphinQt/WiiMix/RogueSettings.h"

#include <array>

class QComboBox;
class QHBoxLayout;
class QGroupBox;

class WiiMixConfigWidget final : public QDialog
{
  Q_OBJECT
public:
  explicit WiiMixConfigWidget(QWidget* parent);

  void DeleteLayout();
  void CreateLayout(WiiMixEnums::Mode mode = WiiMixEnums::Mode::END);
  void CreateBingoLayout(QString menu=QStringLiteral("Host"));
  void CreateShuffleLayout();
  void CreateRogueLayout();
  void CreateCommonLayout();

  // These values should ALWAYS be in sync with the server

  // Rogue
  WiiMixEnums::RogueLength GetRogueLength() const;
  void SetRogueLength(QString) const;
  QString GetRogueSeed() const;
  void SetRogueSeed(QString seed);
  QRegularExpression RogueSeedValidator();

  // Shuffle
  int GetMinTimeBetweenSwitch() const;
  void SetMinTimeBetweenSwitch(int value);
  int GetMaxTimeBetweenSwitch() const;
  void SetMaxTimeBetweenSwitch(int value);
  bool GetEndless() const;
  void SetEndless(bool value);
  int GetNumSwitches() const;
  void SetNumSwitches(int value);
  
  // General
  QString GetDifficulty() const;
  void SetDifficulty(QString value);
  QString GetSaveStateBank() const;
  void SetSaveStateBank(QString value);
  
  // Bingo
  WiiMixEnums::BingoType GetBingoType() const;
  void SetBingoType(WiiMixEnums::BingoType value);
  QString GetCardSize() const;
  void SetCardSize(int index);
  QString GetLobbyID() const;
  QString GenerateLobbyID() const;
  void SetLobbyID(QString seed);
  QRegularExpression BingoLobbyIDValidator();
  QString GetBingoSeed() const;
  QString GenerateBingoSeed() const;
  void SetBingoSeed(QString seed);
  QRegularExpression BingoSeedValidator();
  QString GetPlayerName() const;
  void SetPlayerName(QString value);
  QString GetLobbyPassword() const;
  void SetLobbyPassword(QString value);
  bool GetTeams() const;
  void SetTeams(bool value);
  QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> GetPlayers() const;
  void SetPlayer(WiiMixEnums::Player player, QPair<WiiMixEnums::Color, QString> value);
  void SetPlayers(QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> value);

  std::array<bool, MAX_PLAYERS> GetTeamSelectors() const;
  void SetTeamSelectors(int index);
  // std::array<std::string, MAX_PLAYERS> GetTeamSelectorStates() const;
  // void SetTeamSelectorStates(std::array<std::string, MAX_PLAYERS> value);
  // QList<std::string> GetBingoPlayerName() const;
  // void SetBingoPlayerNames(QList<std::string> value);

  
  WiiMixBingoSettings* GetBingoSettings(WiiMixEnums::Action action = WiiMixEnums::Action::END);

signals:
  void onUpdateBingoConfig(WiiMixBingoSettings* settings); // This signal is for local one player
  // The network client has its own signal for when it receives data that it will emit
  // The corresponding signal is emitted based on whether bingo client exists or not

public slots:
  void OnSettingsChanged(WiiMixBingoSettings* settings);
  void DisplayClientError(QString error);

private:
  QVBoxLayout* m_config_layout;

  // Note when designing interfaces only make the elements that contain data members

  // Bingo Config Options
  QMenuBar* m_menu_bar;
  QLineEdit* m_bingo_lobby_id;
  QLineEdit* m_bingo_seed;
  QLineEdit* m_bingo_player_name;
  QLineEdit* m_bingo_lobby_password;
  QRadioButton* m_bingo_button;
  QRadioButton* m_lockout_button;
  // QRadioButton* m_time_attack_button;
  QCheckBox* m_teams_enabled;
  QPushButton* m_connect_button;
  QList<QCheckBox*> m_team_selectors;
  // QList<std::string> m_team_selector_states;
  // QList<std::string> m_bingo_player_names;
  // player: (color, name)
  QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> m_players;
  QComboBox* m_card_size;

  // Rogue Config Options
  QComboBox* m_rogue_length;
  QLineEdit* m_rogue_seed;

  // Shuffle Config Options
  QLabel* m_min_switch_time_label;
  QSlider* m_min_time_between_switch;
  QLabel* m_max_switch_time_label;
  QSlider* m_max_time_between_switch;
  QLineEdit* m_num_switches;
  QCheckBox* m_endless_mode;

  // Common Config Options

  QComboBox* m_difficulty;
  QComboBox* m_save_state_bank;
  QListWidget* m_objective_types;
  QListWidget* m_game_genres;

  QString m_player_name;
  int m_player_num;
  // Time (maybe I make this an estimate at the bottom rather than a parameter)
  // QLabel* m_time;
};
