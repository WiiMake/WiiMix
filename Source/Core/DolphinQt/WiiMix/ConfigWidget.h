// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QRadioButton>
#include <QPushButton>
#include <QMenuBar>

#include "DolphinQt/WiiMix/BingoClient.h"
#include "DolphinQt/WiiMix/BingoSettings.h"

#include <array>

class QComboBox;
class QHBoxLayout;
class QGroupBox;

class WiiMixConfigWidget final : public QDialog
{
  Q_OBJECT
public:
  explicit WiiMixConfigWidget(QWidget* parent);

  void CreateLayout(WiiMixEnums::Mode mode = WiiMixEnums::Mode::END);
  void CreateBingoLayout(QString menu=QStringLiteral("Host"));
  void CreateShuffleLayout();
  void CreateRogueLayout();
  void CreateCommonLayout();
  void ConnectWidgets();

  // These values should ALWAYS be in sync with the server

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
  void SetCardSize(QString value);
  QString GetLobbyID() const;
  QString GenerateLobbyID() const; 
  QString GetPlayerName() const;
  void SetPlayerName(QString value);
  QString GetLobbyPassword() const;
  void SetLobbyPassword(QString value);
  bool GetTeams() const;
  void SetTeams(bool value);
  QMap<WiiMixEnums::Player, std::tuple<WiiMixEnums::Color, QString>> GetPlayers() const;
  void SetPlayer(WiiMixEnums::Player player, std::tuple<WiiMixEnums::Color, QString> value);
  void SetPlayers(QMap<WiiMixEnums::Player, std::tuple<WiiMixEnums::Color, QString>> value);

  // std::array<bool, MAX_PLAYERS> GetTeamSelectors() const;
  // void SetTeamSelectors(std::array<bool, MAX_PLAYERS> value);
  // std::array<std::string, MAX_PLAYERS> GetTeamSelectorStates() const;
  // void SetTeamSelectorStates(std::array<std::string, MAX_PLAYERS> value);
  // QList<std::string> GetBingoPlayerName() const;
  // void SetBingoPlayerNames(QList<std::string> value);

  
  WiiMixBingoSettings GetBingoSettings();

public slots:
  void onSettingsChanged();

private:
  QVBoxLayout* m_config_layout;

  // Note when designing interfaces only make the elements that contain data members

  // Bingo Config Options
  QMenuBar* m_menu_bar;
  QLineEdit* m_bingo_lobby_id;
  QLineEdit* m_bingo_player_name;
  QLineEdit* m_bingo_lobby_password;
  QRadioButton* m_bingo_button;
  QRadioButton* m_lockout_button;
  QRadioButton* m_time_attack_button;
  QCheckBox* m_teams_enabled;
  QPushButton* m_connect_button;
  QList<QCheckBox*> m_team_selectors;
  // QList<std::string> m_team_selector_states;
  // QList<std::string> m_bingo_player_names;
  QMap<WiiMixEnums::Player, std::tuple<WiiMixEnums::Color, QString>> m_players;
  QComboBox* m_card_size;

  // Rogue Config Options

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

  WiiMixBingoClient* m_bingo_client;
  // Time (maybe I make this an estimate at the bottom rather than a parameter)
  // QLabel* m_time;
};