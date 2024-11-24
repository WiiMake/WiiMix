// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QDialog>
#include "DolphinQt/WiiMix/Settings.h"
#include "DolphinQt/WiiMix/BingoSettings.h"
#include "DolphinQt/WiiMix/RogueSettings.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"
#include "DolphinQt/WiiMix/BingoClient.h"


class WiiMixModesWidget;
class WiiMixConfigWidget;
class QDialogButtonBox;
class QShowEvent;

class WiiMixSettingsWindow final : public QDialog
{
  Q_OBJECT
public:
  explicit WiiMixSettingsWindow(QWidget* parent);
  void CreateLayout(WiiMixEnums::Mode mode);
  void ClearLayout(QLayout *layout);

  std::vector<UICommon::GameFile> GetGamesList();

signals:
  void StartWiiMixBingo(WiiMixBingoSettings settings, WiiMixBingoClient* client);
  void StartWiiMixRogue(WiiMixRogueSettings settings);
  void StartWiiMixShuffle(WiiMixShuffleSettings settings);
  void ErrorLoadingSettings(QString error_message);

// protected:
//   void showEvent(QShowEvent* event) override;

private:
  void LoadSettings();
  void SaveSettings();

  void CreateMainLayout();
  void ConnectWidgets();

  // QDialogButtonBox* m_load_button_box;
  // QDialogButtonBox* m_save_button_box;
  WiiMixSettings m_settings;
  QPushButton* m_load_button_box;
  QPushButton* m_save_button_box;
  QPushButton* m_wii_mix_button;

  WiiMixModesWidget *m_modes;
  WiiMixConfigWidget* m_config;
};
