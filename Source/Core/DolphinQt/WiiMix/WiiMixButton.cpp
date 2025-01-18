//
// Created by Владислав Павловский on 12.11.2024.
//

#include "WiiMixButton.h"
#include <QEvent>
#include <utility>
#include <QPainterPath>
#include <QtGui/qpainter.h>

void WiiMixLogoButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    int radius;
    if (progress_width < size().width() * 0.01) {
        radius = 0;
    } else {
        radius = 5;
    }
    path.addRoundedRect(QRectF(1, size().height() * 0.1, progress_width - 2, size().height() * 0.8), radius, radius);
    QPen pen(Qt::black, (radius == 0) ? 0 : 1);
    painter.setPen(pen);
    if (radius != 0) {
        painter.fillPath(path, QBrush(QColor(245, 195, 203)));
        painter.drawPath(path);
    }
    //painter.setBrush(*(new QBrush(QColor(245, 195, 203))));
    //painter.drawRect(0, size().height() * 0.1, progress_width, size().height() * 0.8);
    QToolButton::paintEvent(event);
}
void WiiMixLogoButton::trackStateReadProgress(qint64 bytesWritten, qint64 totalBytes) {
    totalBytesWritten = bytesWritten;
    printf("totalBytesWritten: %lld, totalBytes: %lld\n", totalBytesWritten, totalBytes);
    progress_width = totalBytesWritten * size().width() / totalBytes;
    repaint();
}

void WiiMixLogoButton::refreshTotalBytesWritten() {
    totalBytesWritten = 0;
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