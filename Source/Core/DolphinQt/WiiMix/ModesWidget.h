// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>
#include <QIcon>
#include <QString>
#include <QFrame>
#include <QResizeEvent>

#include <array>
#include "WiiMixButton.h"
#include <iostream>

#include "DolphinQt/WiiMix/Enums.h"

class QIcon;
class QString;
class QHBoxLayout;
class QGroupBox;

class WiiMixModesWidget final : public QWidget
{
  Q_OBJECT
public:
  explicit WiiMixModesWidget(QWidget* parent);

signals:
  void ModeChanged(WiiMixEnums::Mode mode);

private:
  bool eventFilter(QObject* obj, QEvent* event) override;
  void CreateLayout();
  void ConnectWidgets();
  void keyPressEvent(QKeyEvent* keyEvent) override;

  int selected_mode_n;
  QGroupBox* m_mode_box;
  QHBoxLayout* m_mode_layout;
  std::array<QFrame*, 3> m_mode_selectors;
};