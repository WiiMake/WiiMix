// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>
#include <Core/DolphinQt/Config/WiiMixModesWidget.h>

#include <array>

class QComboBox;
class QHBoxLayout;
class QGroupBox;

class WiiMixConfigWidget final : public QWidget
{
  Q_OBJECT
public:
  explicit WiiMixConfigWidget(QWidget* parent);
  enum Difficulty {
    EASY,
    NORMAL,
    HARD,
    WIISANITY
  };

  void CreateLayout(WiiMixModesWidget::Mode mode);
  void ConnectWidgets();

  QGroupBox* m_config_box;
  // num objectives (shuffle) - size of bingo card (bingo) - num stages in rogue (rogue)
  QComboBox* m_objectives;
  // shuffle time TODO: need a custom slider to show values
  QSlider* m_shuffle_time;
  // Difficulty
  QLabel* m_difficulty;
  // Time (maybe I make this an estimate at the bottom rather than a parameter)
  QLabel* m_time;
  // unverified save states (checkbox for all rounds)
  QCheckBox* m_unverified;
};