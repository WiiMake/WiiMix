// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/SettingsWindow.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/ConfigWidget.h"
#include "DolphinQt/WiiMix/BingoSettings.h"
#include "DolphinQt/WiiMix/RogueSettings.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"
#include "DolphinQt/QtUtils/WrapInScrollArea.h"
#include "DolphinQt/Resources.h"

WiiMixSettingsWindow::WiiMixSettingsWindow(QWidget *parent) : QDialog(parent)
{
  setWindowTitle(tr("WiiMix"));

  m_modes = new WiiMixModesWidget(this);
  m_load_button_box = new QPushButton(tr("Load"));
  // m_load_button_box->button(QDialogButtonBox::Open)->setText(tr("Load"));
  // m_save_button_box = new QDialogButtonBox(QDialogButtonBox::Save);
  m_save_button_box = new QPushButton(tr("Save"));
  // m_load_button_box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  // m_save_button_box->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_wii_mix_button = new QPushButton();
  m_wii_mix_button->setIcon(Resources::GetResourceIcon("wiimix_text"));
  m_wii_mix_button->setIconSize(QSize(128, 128));  // Adjust this size as needed
  m_wii_mix_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  CreateMainLayout();
  ConnectWidgets();
  return;
}

// TODO
void WiiMixSettingsWindow::showEvent(QShowEvent* event)
{
//   QDialog::showEvent(event);
//   m_wiimote_controllers->UpdateBluetoothAvailableStatus();
    return;
}

void WiiMixSettingsWindow::CreateMainLayout()
{
  auto* layout = new QVBoxLayout();

  layout->addWidget(m_modes);

  QHBoxLayout* bottom_buttons = new QHBoxLayout();
  bottom_buttons->setSpacing(2);  // Set spacing between widgets
  bottom_buttons->setContentsMargins(0, 0, 0, 0);  // Remove any margins around the buttons
  bottom_buttons->addStretch(); // Add space before the buttons
  bottom_buttons->addWidget(m_load_button_box, 0);
  bottom_buttons->addWidget(m_wii_mix_button, 0);
  bottom_buttons->addWidget(m_save_button_box, 0);
  bottom_buttons->addStretch(); // Add space after the buttons

  layout->addLayout(bottom_buttons);

  WrapInScrollArea(this, layout);
  return;
}

void WiiMixSettingsWindow::ConnectWidgets()
{
    connect(m_modes, &WiiMixModesWidget::ModeChanged, this, &WiiMixSettingsWindow::CreateLayout);
    connect(m_load_button_box, &QPushButton::clicked, this, &WiiMixSettingsWindow::LoadSettings);
    connect(m_save_button_box, &QPushButton::clicked, this, &WiiMixSettingsWindow::SaveSettings);
    connect(m_wii_mix_button, &QPushButton::clicked, this, [this] {
      if (m_config) {
        m_settings.SetDifficulty(WiiMixSettings::StringToDifficulty(m_config->GetDifficulty()));
        m_settings.SetSaveStateBank(WiiMixSettings::StringToSaveStateBank(m_config->GetSaveStateBank()));
        if (m_settings.GetMode() == WiiMixSettings::Mode::BINGO) {
          WiiMixBingoSettings bingo_settings = WiiMixBingoSettings(m_settings);
          bingo_settings.SetCardSize(WiiMixSettings::StringToCardSize(m_config->GetCardSize()));
          bingo_settings.SetLockout(m_config->GetIsLockout());
          emit StartWiiMixBingo(bingo_settings);
        }
        else if (m_settings.GetMode() == WiiMixSettings::Mode::SHUFFLE) {
          WiiMixShuffleSettings shuffle_settings = WiiMixShuffleSettings(m_settings);
          shuffle_settings.SetNumberOfSwitches(m_config->GetNumSwitches());
          shuffle_settings.SetMinTimeBetweenSwitch(m_config->GetMinTimeBetweenSwitch());
          shuffle_settings.SetMaxTimeBetweenSwitch(m_config->GetMaxTimeBetweenSwitch());
          shuffle_settings.SetEndless(m_config->GetEndless());
          emit StartWiiMixShuffle(shuffle_settings);
        }
        else if (m_settings.GetMode() == WiiMixSettings::Mode::ROGUE) {
          WiiMixRogueSettings rogue_settings = WiiMixRogueSettings(m_settings);
          emit StartWiiMixRogue(rogue_settings);
        }
      }
    }); // Start WiiMix
    return;
}

void WiiMixSettingsWindow::CreateLayout(WiiMixSettings::Mode mode)
{
  m_config = new WiiMixConfigWidget(this);
  m_settings.SetMode(mode);
  m_config->CreateLayout(mode);

  // Create a new window with the configuration options
  auto* config_window = new QDialog(this);
  config_window->setWindowTitle(tr("Configuration Options"));
  auto* config_layout = new QVBoxLayout(config_window);
  config_layout->addWidget(m_config);
  config_window->setLayout(config_layout);
  config_window->exec();
  return;
}

// TODO
void WiiMixSettingsWindow::LoadSettings() {
  
}

void WiiMixSettingsWindow::SaveSettings() {

}