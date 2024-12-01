#pragma once

#include <QFrame>

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

