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
#include "DolphinQt/WiiMix/Settings.h"

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

  int GetMinTimeBetweenSwitch() const;
  int GetMaxTimeBetweenSwitch() const;
  QString GetDifficulty() const;
  QString GetSaveStateBank() const;
  int GetNumSwitches() const;
  bool GetEndless() const;
  WiiMixEnums::BingoType GetBingoType() const;
  QString GetCardSize() const;
  QString GetLobbyName() const;
  QString GetLobbyPassword() const;
  bool GetTeamsEnabled() const;
  std::array<bool, MAX_PLAYERS> GetTeamSelectors() const;

private:
  QVBoxLayout* m_config_layout;

  // Note when designing interfaces only make the elements that contain data members

  // Bingo Config Options
  QMenuBar* m_menu_bar;
  QLineEdit* m_bingo_lobby_name;
  QLineEdit* m_bingo_lobby_password;
  QRadioButton* m_bingo_button;
  QRadioButton* m_lockout_button;
  QRadioButton* m_time_attack_button;
  QCheckBox* m_teams_enabled;
  QPushButton* m_connect_button;
  QList<QCheckBox*> m_team_selectors;
  QList<std::string> m_team_selector_states;  
  QComboBox* m_card_size;

  // Rogue Config Options

  // Shuffle Config Options
  QSlider* m_min_time_between_switch;
  QSlider* m_max_time_between_switch;
  QLineEdit* m_num_switches;
  QCheckBox* m_endless_mode;

  // Common Config Options

  QComboBox* m_difficulty;
  QComboBox* m_save_state_bank;
  // Time (maybe I make this an estimate at the bottom rather than a parameter)
  // QLabel* m_time;
};