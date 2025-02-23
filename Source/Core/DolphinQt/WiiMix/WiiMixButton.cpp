//
// Created by Владислав Павловский on 12.11.2024.
//

#include "WiiMixButton.h"
#include <QEvent>
#include <QString>
#include <utility>
#include <QPainterPath>
#include <QtGui/qpainter.h>
#include "ConfigWidget.h"

void WiiMixLogoButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    int radius;
    if (progress_width < size().width() * 0.01) radius = 0;
    else radius = 2;
    path.addRoundedRect(QRectF(1, size().height() * 0.8, progress_width - 2, size().height() * 0.05), radius, radius);
    QPen pen(Qt::black, (radius == 0) ? 0 : 1);
    painter.setPen(pen);
    if (radius != 0) {
        QColor color;
        if (Config::Get(Config::MAIN_THEME_NAME) == "Clean") {
            color = QColor(127, 127, 127);
        } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Blue") {
            color = Qt::blue;
        } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Emerald") {
            color = Qt::green;
        } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Lite") {
            color = Qt::transparent;
        } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Pink") {
            color = QColor(255, 192, 203);
        } else {
            qDebug() << "Wrong theme name";
        }
        painter.fillPath(path, QBrush(color));
        painter.drawPath(path);
    }
    QToolButton::paintEvent(event);
}
void WiiMixLogoButton::trackStateReadProgress(qint64 bytesWritten, qint64 totalBytes) {
    totalBytesWritten = bytesWritten;
    // printf("totalBytesWritten: %lld, totalBytes: %lld\n", totalBytesWritten, totalBytes);
    progress_width = totalBytesWritten * size().width() / totalBytes;
    repaint();
}

void WiiMixButton::drawButton() {
    char styleSheet[500];
    if (hasBackgroundImage) {
        std::snprintf(styleSheet, 500,
                      ".QFrame {border-image: url(\"%s\") %d stretch stretch; border-radius: %dpx; border-width: %dpx; border-color: %s; border-style: solid}",
                      backgroundImage.data(), border_radius, border_radius, border_width, border_color.data());
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
