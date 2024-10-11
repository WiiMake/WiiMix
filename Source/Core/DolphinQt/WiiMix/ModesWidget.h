// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>
#include <QIcon>
#include <QString>

#include <array>

class QIcon;
class QString;
class QHBoxLayout;
class QGroupBox;

class WiiMixModesWidget final : public QWidget
{
  Q_OBJECT
public:
  explicit WiiMixModesWidget(QWidget* parent);

private:
  void CreateLayout();
  void ConnectWidgets();

  QGroupBox* m_mode_box;
  QHBoxLayout* m_mode_layout;
  std::array<QLayout*, 3> m_mode_selectors;
};