#pragma once

#include <QFrame>
#include <QToolButton>

class WiiMixLogoButton : public QToolButton {
    int progress_width;
    void paintEvent(QPaintEvent *event) override;
public:
    void trackStateReadProgress(qint64 bytesWritten, qint64 totalBytes);
};

class WiiMixButton : public QFrame {
    typedef QFrame inherited;
    std::string backgroundImage = "";
    int border_width = 0;
    std::string border_color = "gray";
    int border_radius = 0;
    bool hasBackgroundImage = false;
public:
    void drawButton();
    void setBackgroundImage(std::string path);
    void setBorderWidth(int width);
    void setBorderColor(std::string color);
    void setBorderRadius(int radius);
    void setHasBackgroundImage(bool value);
    virtual ~WiiMixButton() {
        backgroundImage = "";
        border_color = "";
        inherited::~QFrame();
    };
};