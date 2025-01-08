// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QSpinBox>
#include <QListWidget>
#include <QComboBox>
#include <QTimeEdit>
#include <QPushButton>

#include "DolphinQt/WiiMix/Objective.h"

class WiiMixStateSendMenu : public QDialog
{
    Q_OBJECT

public:
    explicit WiiMixStateSendMenu();

    void ConnectWidgets();
    void CreateLayout();

signals:
    void SendObjective(WiiMixObjective objective);

private:
    QLineEdit* m_title;
    QTextEdit* m_description;
    QSpinBox* m_achievement_id;
    QListWidget* m_objective_types;
    QListWidget* m_game_genres;
    QComboBox* m_difficulty;
    QSpinBox* m_time_length;
    QPushButton* m_send_button;
};