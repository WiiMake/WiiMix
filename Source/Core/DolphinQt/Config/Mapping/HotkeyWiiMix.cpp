// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Config/Mapping/HotkeyWiiMix.h"

#include <QGroupBox>
#include <QHBoxLayout>

#include "Core/HotkeyManager.h"

HotkeyWiiMix::HotkeyWiiMix(MappingWindow* window) : MappingWidget(window)
{
  CreateMainLayout();
}

void HotkeyWiiMix::CreateMainLayout()
{
  m_main_layout = new QHBoxLayout();

  m_main_layout->addWidget(
      CreateGroupBox(tr("WiiMix"), HotkeyManagerEmu::GetHotkeyGroup(HKGP_WIIMIX)));

  setLayout(m_main_layout);
}

InputConfig* HotkeyWiiMix::GetConfig()
{
  return HotkeyManagerEmu::GetConfig();
}

void HotkeyWiiMix::LoadSettings()
{
  HotkeyManagerEmu::LoadConfig();
}

void HotkeyWiiMix::SaveSettings()
{
  HotkeyManagerEmu::GetConfig()->SaveConfig();
}
