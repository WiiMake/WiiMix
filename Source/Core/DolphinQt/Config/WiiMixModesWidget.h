// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>

#include <array>

class QComboBox;
class QHBoxLayout;
class QGroupBox;

namespace Core
{
enum class State;
}

class WiiMixModesWidget final : public QWidget
{
  Q_OBJECT
public:
  explicit WiiMixModesWidget(QWidget* parent);
  enum Mode {
    BINGO, // Sub-modes: lockout, capture the flag
    SHUFFLE, // Sub-modes: race
    ROGUE
  };

private:
  void CreateLayout();
  void ConnectWidgets();

  QGroupBox* m_mode_box;
  QHBoxLayout* m_mode_layout;
  std::array<QLayout*, 3> m_mode_selectors;
};