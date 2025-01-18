//
// Created by Владислав Павловский on 12.11.2024.
//

#include "WiiMixButton.h"
#include <QEvent>
#include <utility>
#include <QtGui/qpainter.h>

void WiiMixLogoButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    printf("new WiiMixLogoButton paintEvent is called\n");
    painter.setBrush(*(new QBrush(QColor(245, 195, 203))));
    painter.drawRect(size().height() * 0.1, 0, progress_width, size().height() * 0.8);
    QToolButton::paintEvent(event);
}
void WiiMixLogoButton::trackStateReadProgress(qint64 bytesWritten, qint64 totalBytes) {
    printf("trackStateReadProgress is called\n");
    progress_width = (int) (((double) bytesWritten / totalBytes) * size().width());
    repaint();
}

void WiiMixButton::drawButton() {
    char styleSheet[500];
    if (hasBackgroundImage) {
        std::snprintf(styleSheet, 500,
                      ".QFrame {background-image: url(\"%s\"); border-radius: %dpx; border-width: %dpx; border-color: %s; border-style: solid}",
                      backgroundImage.data(), border_radius, border_width, border_color.data());
    } else {
        std::snprintf(styleSheet, 500,
                      ".QFrame {border-radius: %dpx; border-width: %dpx; border-color: %s; border-style: solid}",
                      border_radius, border_width, border_color.data());
    }
    this->setStyleSheet(QString::fromStdString(styleSheet));
}

void WiiMixButton::setBackgroundImage(std::string path) {
    backgroundImage = std::move(path);
    drawButton();
}

void WiiMixButton::setBorderWidth(int width) {
    border_width = width;
    drawButton();
}

void WiiMixButton::setBorderColor(std::string color) {
    border_color = std::move(color);
    drawButton();
}

void WiiMixButton::setBorderRadius(int radius) {
    border_radius = radius;
    drawButton();
}

void WiiMixButton::setHasBackgroundImage(bool value) {
    hasBackgroundImage = value;
}