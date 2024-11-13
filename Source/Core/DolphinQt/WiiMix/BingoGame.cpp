#pragma once

#include "BingoGame.h"
#include "Common/FileUtil.h"
#include <QToolTip>
#include <QGridLayout>
#include <QPushButton>
#include "DolphinQt/Resources.h"

void BingoGame::toolTip(QWidget* context) {
    char toolTipString[1000];
    std::string icon = File::GetSysDirectory() + "Resources/Flag_Russia.png";
    std::string name = "Fresh Member of the Greil Mercenaries";
    std::string description = "Clear 'The Battle Begins'";
    snprintf(toolTipString, 999, "<img src='%s'><h2>%s</h2>\n%s", icon.data(), name.data(), description.data());
    QToolTip::showText(QCursor::pos(), QString::fromStdString(toolTipString), context, QRect(), 10000);
}
QFrame* BingoGame::createObjectiveWindow(std::vector<BingoItem> items) {
    double sqrtSize = round(sqrt(items.size()));
    if (sqrtSize * sqrtSize != items.size()) {
        printf("incorrect items size for bingo window, aborting drawing");
    }
    QFrame* frame = new QFrame();
    QGridLayout* gridLayout = new QGridLayout();
    for (int i = 0; i < sqrtSize; i++) {
        for (int j = 0; j < sqrtSize; j++) {
            QPushButton *button = new QPushButton();
            button->setIcon(Resources::GetResourceIcon(items[i * sqrtSize + j].icon));
            toolTip(button); // TODO: set this to happen on hover
            gridLayout->addWidget(button, i, j);
        }
    }
    gridLayout->setSpacing(5);
    frame->setLayout(gridLayout);
    frame->show();
}
