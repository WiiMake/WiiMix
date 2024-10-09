// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDialog>

class WiiMixModesWidget;
class WiiMixConfigWidget;
class QDialogButtonBox;
class QShowEvent;

class WiiMixWindow final : public QDialog
{
  Q_OBJECT
public:
  explicit WiiMixWindow(QWidget* parent);

protected:
  void showEvent(QShowEvent* event) override;

private:
  void LoadSettings(Core::State state);
  void SaveSettings();

  void CreateMainLayout();
  void ConnectWidgets();

  QDialogButtonBox* m_load_button_box;
  QDialogButtonBox* m_save_button_box;
  QPushButton* m_wii_mix_button;

  WiiMixModesWidget *m_modes;
  WiiMixConfigWidget* m_config;
};
