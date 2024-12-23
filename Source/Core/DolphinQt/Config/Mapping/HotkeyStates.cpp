// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Config/Mapping/HotkeyStates.h"

#include <QGroupBox>
#include <QHBoxLayout>

#include "Core/HotkeyManager.h"

HotkeyStates::HotkeyStates(MappingWindow* window) : MappingWidget(window)
{
  CreateMainLayout();
}

void HotkeyStates::CreateMainLayout()
{
  m_main_layout = new QHBoxLayout();

  m_main_layout->addWidget(
      CreateGroupBox(tr("Save"), HotkeyManagerEmu::GetHotkeyGroup(HKGP_SAVE_STATE)));
  m_main_layout->addWidget(
      CreateGroupBox(tr("Load"), HotkeyManagerEmu::GetHotkeyGroup(HKGP_LOAD_STATE)));
  // m_main_layout->addWidget(
  //     CreateGroupBox(tr("Swap Game"), HotkeyManagerEmu::GetHotkeyGroup(HKGP_SWAP_STATE)));
  // m_main_layout->addWidget(
  //     CreateGroupBox(tr("Load Objective"), HotkeyManagerEmu::GetHotkeyGroup(HKGP_LOAD_OBJECTIVE)));
  // m_main_layout->addWidget(
  //     CreateGroupBox(tr("Reset Objective"), HotkeyManagerEmu::GetHotkeyGroup(HKGP_RESET_OBJECTIVE)));
  // m_main_layout->addWidget(
  //     CreateGroupBox(tr("Bingo Ready"), HotkeyManagerEmu::GetHotkeyGroup(HKGP_BINGO_READY)));

  setLayout(m_main_layout);
}

InputConfig* HotkeyStates::GetConfig()
{
  return HotkeyManagerEmu::GetConfig();
}

void HotkeyStates::LoadSettings()
{
  HotkeyManagerEmu::LoadConfig();
}

void HotkeyStates::SaveSettings()
{
  HotkeyManagerEmu::GetConfig()->SaveConfig();
}
