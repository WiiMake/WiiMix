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
  void ConnectWidgets();

  int GetMinTimeBetweenSwitch() const;
  int GetMaxTimeBetweenSwitch() const;
  QString GetDifficulty() const;
  QString GetSaveStateBank() const;
  int GetNumSwitches() const;
  bool GetEndless() const;
  bool GetIsLockout() const;
  QString GetCardSize() const;

private:
  QGroupBox* m_config_box;
  // num objectives (shuffle) - size of bingo card (bingo) - num stages in rogue (rogue)
  // QComboBox* m_objectives;
  // shuffle time
  QSlider* m_min_time_between_switch;
  QSlider* m_max_time_between_switch;
  // WiiMixEnums::Difficulty
  QComboBox* m_difficulty;
  // Time (maybe I make this an estimate at the bottom rather than a parameter)
  // QLabel* m_time;
  QComboBox* m_save_state_bank;
  QLineEdit* m_num_switches;
  QCheckBox* m_endless_mode;
  QRadioButton* m_bingo_button;
  QRadioButton* m_lockout_button;
  QComboBox* m_card_size;
};