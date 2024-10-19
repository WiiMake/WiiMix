// // Copyright 2021 Dolphin Emulator Project
// // SPDX-License-Identifier: GPL-2.0-or-later

// #pragma once

// #include <QWidget>
// #include <QIcon>
// #include <QString>
// #include <QFrame>

// #include <array>
// #include "DolphinQt/WiiMix/Settings.h"


// class QIcon;
// class QString;
// class QHBoxLayout;
// class QGroupBox;

// class WiiMixBingoLobbyWidget final : public QWidget
// {
//   Q_OBJECT
// public:
//   explicit WiiMixBingoLobbyWidget(QWidget* parent);

// signals:
//   void ModeChanged(WiiMixEnums::Mode mode);

// private:
//   void CreateLayout();
//   void ConnectWidgets();

//   QGroupBox* m_mode_box;
//   QHBoxLayout* m_mode_layout;
//   std::array<QFrame*, 3> m_mode_selectors;
// };