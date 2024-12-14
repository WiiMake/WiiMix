//
// Created by Владислав Павловский on 12.11.2024.
//

#include "WiiMixButton.h"
#include <QEvent>
#include <utility>

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