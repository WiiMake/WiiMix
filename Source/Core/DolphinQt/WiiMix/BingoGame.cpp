#pragma once

#include "BingoGame.h"
#include "Common/FileUtil.h"
#include <QToolTip>

void BingoGame::toolTip(QWidget* context) {
    char toolTipString[1000];
    std::string icon = File::GetSysDirectory() + "Resources/Flag_Russia.png";
    std::string name = "Fresh Member of the Greil Mercenaries";
    std::string description = "Clear 'The Battle Begins'";
    snprintf(toolTipString, 999, "<img src='%s'><h2>%s</h2>\n%s", icon.data(), name.data(), description.data());
    QToolTip::showText(QCursor::pos(), QString::fromStdString(toolTipString), context, QRect(), 10000);
}
