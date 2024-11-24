#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QResizeEvent>
#include <QGraphicsDropShadowEffect>
#include <QToolTip>

#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/Settings.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"
#include "VideoCommon/OnScreenDisplay.h"
#include "Core/Core.h"
#include "Overlay.h"
#include "Common/FileUtil.h"
#include "DolphinQt/Resources.h"
#include "WiiMixButton.h"
#include "BingoGame.h"


WiiMixModesWidget::WiiMixModesWidget(QWidget* parent) : QWidget(parent) {
    CreateLayout();
    ConnectWidgets();
}

void WiiMixModesWidget::CreateLayout() {
    m_mode_box = new QGroupBox(tr("Modes"));
    m_mode_layout = new QHBoxLayout();
    m_mode_box->setLayout(m_mode_layout);
    m_mode_selectors = {};
    for (int i = 0; i < static_cast<int>(WiiMixEnums::Mode::END); i++) {
        WiiMixButton* frame = new WiiMixButton();
        frame->setFixedSize(300, 169);
        // Set up QFrame
        char * backgroundImagePath = (char *) malloc(200);
        strncpy(backgroundImagePath, (File::GetSysDirectory() + "Resources" + "/temporary_gradient_bg.png").data(), 200);
        frame->setBorderRadius(18);
        frame->setBorderWidth(4);
        frame->setBorderColor("gray");
        frame->setHasBackgroundImage(true);
        frame->setBackgroundImage(backgroundImagePath);
        //frame->setToolTip(QString::fromStdString("Hello?"));
        frame->setCursor(Qt::PointingHandCursor);
        frame->installEventFilter(this); // To capture mouse events
        frame->setAttribute(Qt::WA_Hover, true);
        m_mode_selectors[i] = frame;
    }

    // Create selectors
    QFont titleFont = QFont();
    titleFont.setPointSize(42);
    titleFont.setBold(true);

    QFont descriptionFont = QFont();
    descriptionFont.setPointSize(16);

    for (int i = 0; i < static_cast<int>(WiiMixEnums::Mode::END); i++) {
        QString titleText = WiiMixSettings::ModeToTitle(WiiMixEnums::Mode(i));
        if (titleText == QStringLiteral("")) {
            continue;
        }

        // Title
        QLabel* title = new QLabel(titleText);
        title->setFont(titleFont);
        title->setAlignment(Qt::AlignCenter);
        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(title);
        m_mode_selectors[i]->setLayout(layout);
        m_mode_layout->addWidget(m_mode_selectors[i]);
    }

    setLayout(m_mode_layout);
}
bool WiiMixModesWidget::eventFilter(QObject* obj, QEvent* event) {
    if ((event->type() == QEvent::HoverLeave)) {
        QToolTip::hideText();
        for (int i = 0; i < static_cast<int>(WiiMixEnums::Mode::END); i++) {
            WiiMixButton* other_frame = (WiiMixButton*)(m_mode_layout->itemAt(i)->widget());
            if (other_frame) {
                other_frame->setGraphicsEffect(nullptr);
            }
        }
    }
    if (event->type() == QEvent::HoverEnter) {
        WiiMixButton* frame = (WiiMixButton *)(obj);
        if (frame) {
            // Highlight hovered frame
            std::string theme = Config::Get(Config::MAIN_THEME_NAME);
            auto *shadow = new QGraphicsDropShadowEffect;
            shadow->setBlurRadius(25);
            shadow->setXOffset(0);
            shadow->setYOffset(0);

            if (theme == "Clean") {
                shadow->setColor(Qt::black);
                frame->setGraphicsEffect(shadow);
            }
            else if (theme == "Clean Blue") {
                shadow->setColor(Qt::blue);
                frame->setGraphicsEffect(shadow);
            }
            else if (theme == "Clean Emerald"){
                shadow->setColor(Qt::green);
                frame->setGraphicsEffect(shadow);
            }
            else if (theme == "Clean Lite"){
                shadow->setColor(Qt::white);

                frame->setGraphicsEffect(shadow);
            }
            else if (theme == "Clean Pink"){
                shadow->setColor(QColor(255, 192, 203));
                frame->setGraphicsEffect(shadow);
            }
            else {
                qDebug("invalid theme name");
            }
            return true;
        }
    }
    if (event->type() == QEvent::MouseButtonPress) {
        WiiMixButton* frame = (WiiMixButton *)(obj);
        if (frame) {
            // Deselect all other frames
            for (int i = 0; i < static_cast<int>(WiiMixEnums::Mode::END); i++) {
                WiiMixButton* other_frame = (WiiMixButton *)(m_mode_layout->itemAt(i)->widget());
                if (other_frame) {
                    //other_frame->setGraphicsEffect(nullptr);
                    other_frame->setBorderColor("gray"); // gray is border color for non-selected frame
                }
            }
            std::vector<BingoItem> b;
            b.reserve(25);
            for (int i = 0; i < 25; i++) {
                BingoItem bItem;
                bItem.name = "Hello";
                bItem.description = "HelloHelloHello";
                bItem.icon = "Flag_Russia@2x";
                b.push_back(bItem);
            }
            BingoGame::createObjectiveWindow(b);
            std::string theme = Config::Get(Config::MAIN_THEME_NAME);

            auto *shadow = new QGraphicsDropShadowEffect;
            shadow->setBlurRadius(25);
            shadow->setXOffset(0);
            shadow->setYOffset(0);

            // Highlight the selected frame
            if (theme == "Clean") {
                shadow->setColor(Qt::black);
                //auto *colorizeEffect = new QGraphicsColorizeEffect;
                //colorizeEffect->setColor(Qt::black);
                //frame->setGraphicsEffect(colorizeEffect);
                frame->setBorderColor("black");
            }
            else if (theme == "Clean Blue") {
                shadow->setColor(Qt::blue);
                //auto *colorizeEffect = new QGraphicsColorizeEffect;
                //colorizeEffect->setColor(Qt::blue);
                //frame->setGraphicsEffect(colorizeEffect);
                frame->setBorderColor("blue");
            }
            else if (theme == "Clean Emerald"){
                shadow->setColor(Qt::green);
                //auto *colorizeEffect = new QGraphicsColorizeEffect;
                //colorizeEffect->setColor(Qt::green);
                //frame->setGraphicsEffect(colorizeEffect);
                frame->setBorderColor("green");
            }
            else if (theme == "Clean Lite"){
                shadow->setColor(Qt::white);
                //auto *colorizeEffect = new QGraphicsColorizeEffect;
                //colorizeEffect->setColor(Qt::white);
                //frame->setGraphicsEffect(colorizeEffect);
                frame->setBorderColor("white");
            }
            else if (theme == "Clean Pink"){
                shadow->setColor(QColor(255, 192, 203));
                //auto *colorizeEffect = new QGraphicsColorizeEffect;
                //colorizeEffect->setColor(QColor(255, 192, 203));
                //frame->setGraphicsEffect(colorizeEffect);
                frame->setBorderColor("rgb(255, 192, 203)");

            }
            else {
                qDebug("invalid theme name, applying default theme instead");
                frame->setStyleSheet(QStringLiteral("QFrame { border: 5px solid black; }"));
            }
            // Trigger configuration update for this WiiMixEnums::Mode
            WiiMixEnums::Mode selected_mode = static_cast<WiiMixEnums::Mode>(m_mode_layout->indexOf(frame));
            emit WiiMixModesWidget::ModeChanged(selected_mode);

            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void WiiMixModesWidget::ConnectWidgets() {
    return;
}