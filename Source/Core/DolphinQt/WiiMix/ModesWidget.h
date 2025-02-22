// Copyright 2021 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>
#include <QIcon>
#include <QString>
#include <QFrame>
#include <QResizeEvent>
#include <QMainWindow>

#include "Core/Boot/Boot.h"

#include <array>
#include "WiiMixButton.h"
#include <iostream>

#include "DolphinQt/WiiMix/Enums.h"

class QIcon;
class QString;
class QHBoxLayout;
class QGroupBox;

struct BootParameters;

class WiiMixModesWidget final : public QMainWindow
{
  Q_OBJECT
public:
  explicit WiiMixModesWidget(std::unique_ptr<BootParameters> boot_parameters);

signals:
  void ShuffleSelected(WiiMixEnums::Mode mode);

private:
  bool eventFilter(QObject* obj, QEvent* event) override;
  void CreateLayout();
  void keyPressEvent(QKeyEvent* keyEvent) override;

  QGroupBox* m_mode_box;
  QHBoxLayout* m_mode_layout;
  std::array<QFrame*, 3> m_mode_selectors;
  int selected_mode_n;
};