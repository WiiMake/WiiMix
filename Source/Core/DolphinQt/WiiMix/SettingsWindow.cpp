// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/SettingsWindow.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/ConfigWidget.h"
#include "DolphinQt/QtUtils/WrapInScrollArea.h"
#include "DolphinQt/Resources.h"

WiiMixSettingsWindow::WiiMixSettingsWindow(QWidget *parent) : QDialog(parent)
{
  setWindowTitle(tr("WiiMix"));

  m_modes = new WiiMixModesWidget(this);
  m_config = new WiiMixConfigWidget(this);
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

// TODO
void WiiMixSettingsWindow::CreateMainLayout()
{
  auto* layout = new QVBoxLayout();

  layout->addWidget(m_modes);
  layout->addWidget(m_config);

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
    connect(m_wii_mix_button, &QPushButton::clicked, this, [=, this]() { emit StartWiiMix(); }); // Start WiiMix
    connect(m_load_button_box, &QPushButton::clicked, this, &WiiMixSettingsWindow::LoadSettings);
    connect(m_save_button_box, &QPushButton::clicked, this, &WiiMixSettingsWindow::SaveSettings);
    return;
}

void WiiMixSettingsWindow::LoadSettings() {
  
}

void WiiMixSettingsWindow::SaveSettings() {

}