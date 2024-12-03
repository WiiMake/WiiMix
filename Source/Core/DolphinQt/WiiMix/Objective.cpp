#include "DolphinQt/WiiMix/Objective.h"

#include "DolphinQt/WiiMix/Enums.h"

#include <string>
#include <curl/curl.h>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>




// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later
WiiMixObjective::WiiMixObjective(
    std::string objective_string,
    WiiMixEnums::Player completed
) : m_objective_string(objective_string), m_completed(completed){};

// @gyoder ID?
std::string WiiMixObjective::GetObjectiveString() {
    return m_objective_string;
}

WiiMixEnums::Player WiiMixObjective::GetCompleted() {
    return m_completed;
}

void WiiMixObjective::SetCompleted(WiiMixEnums::Player player) {
    m_completed = player;
}

// @gyoder icon?





