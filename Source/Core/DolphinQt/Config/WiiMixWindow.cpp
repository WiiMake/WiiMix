// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/Config/WiiMixWindow.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "DolphinQt/Config/WiiMixModesWidget.h"
#include "DolphinQt/Config/WiiMixConfigWidget.h"
#include "DolphinQt/QtUtils/WrapInScrollArea.h"

// TODO
WiiMixWindow::WiiMixWindow(QWidget* parent) : QDialog(parent)
{
//   setWindowTitle(tr("Controller Settings"));
//   setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

//   m_gamecube_controllers = new GamecubeControllersWidget(this);
//   m_wiimote_controllers = new WiimoteControllersWidget(this);
//   m_common = new CommonControllersWidget(this);
//   CreateMainLayout();
//   ConnectWidgets();
    return;
}

// TODO
void WiiMixWindow::showEvent(QShowEvent* event)
{
//   QDialog::showEvent(event);
//   m_wiimote_controllers->UpdateBluetoothAvailableStatus();
    return;
}

// TODO
void WiiMixWindow::CreateMainLayout()
{
//   auto* layout = new QVBoxLayout();
//   m_button_box = new QDialogButtonBox(QDialogButtonBox::Close);
// 
//   layout->addWidget(m_gamecube_controllers);
//   layout->addWidget(m_wiimote_controllers);
//   layout->addWidget(m_common);
//   layout->addStretch();
//   layout->addWidget(m_button_box);
// 
//   WrapInScrollArea(this, layout);
    return;
}

void WiiMixWindow::ConnectWidgets()
{
//   connect(m_button_box, &QDialogButtonBox::rejected, this, &QDialog::reject);
    return;
}
