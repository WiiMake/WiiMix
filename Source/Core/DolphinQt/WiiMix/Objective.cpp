#include "DolphinQt/WiiMix/Objective.h"

#include "DolphinQt/WiiMix/Enums.h"

#include <string>

// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later
WiiMixObjective::WiiMixObjective(
    std::string objective_string, 
    WiiMixEnums::Player completed
) : m_objective_string(objective_string), m_completed(completed){};

std::string WiiMixObjective::GetObjectiveString() {
    return m_objective_string;
}

WiiMixEnums::Player WiiMixObjective::GetCompleted() {
    return m_completed;
}

void WiiMixObjective::SetCompleted(WiiMixEnums::Player player) {
    m_completed = player;
}