// // Copyright 2017 Dolphin Emulator Project
// // SPDX-License-Identifier: GPL-2.0-or-later

// #include "DolphinQt/WiiMix/BingoLobbyWidget.h"

// WiiMixBingoLobbyWidget::WiiMixBingoLobbyWidget(QWidget* parent) : QWidget(parent)
// {
//   CreateLayout();
//   ConnectWidgets();
//   return;
// }

// void WiiMixBingoLobbyWidget::CreateLayout()
// {
//   m_mode_box = new QGroupBox(tr("Mode"));
//   m_mode_layout = new QHBoxLayout();
//   m_mode_selectors[0] = new QFrame();
//   m_mode_selectors[1] = new QFrame();
//   m_mode_selectors[2] = new QFrame();
//   m_mode_selectors[0]->setFrameStyle(QFrame::Panel | QFrame::Raised);
//   m_mode_selectors[1]->setFrameStyle(QFrame::Panel | QFrame::Raised);
//   m_mode_selectors[2]->setFrameStyle(QFrame::Panel | QFrame::Raised);
//   m_mode_selectors[0]->setFixedWidth(100);
//   m_mode_selectors[1]->setFixedWidth(100);
//   m_mode_selectors[2]->setFixedWidth(100);
//   m_mode_layout->addWidget(m_mode_selectors[0]);
//   m_mode_layout->addWidget(m_mode_selectors[1]);
//   m_mode_layout->addWidget(m_mode_selectors[2]);
//   m_mode_box->setLayout(m_mode_layout);

//   QVBoxLayout* layout = new QVBoxLayout();
//   layout->addWidget(m_mode_box);
//   setLayout(layout);
//   return;
// }

// void WiiMixBingoLobbyWidget::ConnectWidgets()
// {
//   connect(m_mode_selectors[0], &QFrame::clicked, this, [this] {
//     emit ModeChanged(WiiMixEnums::Mode::BINGO);
//   });
//   connect(m_mode_selectors[1], &QFrame::clicked, this, [this] {
//     emit ModeChanged(WiiMixEnums::Mode::SHUFFLE);
//   });
//   connect(m_mode_selectors[2], &QFrame::clicked, this, [this] {
//     emit ModeChanged(WiiMixEnums::Mode::ROGUE);
//   });
//   return;
// }