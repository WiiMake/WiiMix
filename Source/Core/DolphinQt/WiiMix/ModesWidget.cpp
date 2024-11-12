#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QResizeEvent>
#include <QGraphicsDropShadowEffect>

#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/Settings.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"
#include "VideoCommon/OnScreenDisplay.h"
#include "Core/Core.h"
#include "Overlay.h"
#include "Common/FileUtil.h"
#include "DolphinQt/Resources.h"


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
        QFrame* frame = new QFrame();
        frame->setFixedSize(300, 169);
        char * backgroundImagePath = (char *) malloc(200);
        strncpy(backgroundImagePath, (File::GetSysDirectory() + "Resources" + "/temporary_gradient_bg.png").data(), 200);
        char *modeStyleSheet = (char *) malloc(1000);
        std::snprintf(modeStyleSheet, 999, ".QFrame {border-radius: 18px; border-width: 4px; border-style: solid; border-color: gray; background-image: url(\"%s\");}", backgroundImagePath);
        frame->setStyleSheet(QString::fromStdString(modeStyleSheet));
        //frame->setStyleSheet(QStringLiteral(".QFrame {border-width: 3px; border-color: LightGray; border-style: solid; border-radius: 18px; background-image: url(\":/Flag_Russia@4x\")}"));
        //frame->setStyleSheet(QStringLiteral(".QFrame {border-width: 3px; border-color: LightGray; border-style: solid; border-radius: 18px; }"));
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

        // Icon
        QIcon icon = WiiMixSettings::ModeToIcon(WiiMixEnums::Mode(i));
        if (icon.isNull()) {
            continue;
        }
        QLabel* iconLabel = new QLabel();
        iconLabel->setPixmap(icon.pixmap(64, 64));  // Set initial size, resizable later
        iconLabel->setAlignment(Qt::AlignHCenter);

        // Description
        QString descriptionText = WiiMixSettings::ModeToDescription(WiiMixEnums::Mode(i));
        if (descriptionText == QStringLiteral("")) {
            continue;
        }
        QLabel* description = new QLabel(descriptionText);
        description->setFont(descriptionFont);
        description->setWordWrap(true);  // Ensure text wraps vertically
        description->setAlignment(Qt::AlignHCenter);
        // Create a QFrame for the selector
        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(title);
        //layout->addWidget(iconLabel, 0);
        //layout->addWidget(description, 1);
        m_mode_selectors[i]->setLayout(layout);
        m_mode_layout->addWidget(m_mode_selectors[i]);
    }

    setLayout(m_mode_layout);

    // for (int i = 0; i < static_cast<int>(WiiMixEnums::Mode::END); i++) {
    //     qDebug() << "Creating layout for WiiMixEnums::Mode " << WiiMixEnums::Mode(i);
    //     QString titleText = WiiMixSettings::ModeToTitle(WiiMixEnums::Mode(i));
    //     qDebug() << "TitleText: " << titleText;
    //     if (titleText == QStringLiteral("")) {
    //         continue;
    //     }
    //     QLabel* title = new QLabel(titleText);
    //     title->setFont(titleFont);
    //     title->setAlignment(Qt::AlignHCenter);

    //     QIcon icon = WiiMixSettings::ModeToIcon(WiiMixEnums::Mode(i));
    //     if (icon.isNull()) {
    //         continue;
    //     }
    //     QLabel* iconLabel = new QLabel();
    //     iconLabel->setPixmap(icon.pixmap(32, 32)); // Adjust the size as needed
    //     iconLabel->setAlignment(Qt::AlignHCenter);

    //     QString descriptionText = WiiMixSettings::ModeToDescription(WiiMixEnums::Mode(i));
    //     if (descriptionText == QStringLiteral("")) {
    //         continue;
    //     }
    //     QLabel* description = new QLabel(descriptionText);
    //     description->setFont(descriptionFont);
    //     description->setWordWrap(true);
    //     description->setAlignment(Qt::AlignHCenter);

    //     QWidget* container = new QWidget();
    //     container->setLayout(m_mode_selectors[i]);
    //     container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    //     m_mode_layout->addWidget(container);

    //     m_mode_selectors[i]->addWidget(title);
    //     m_mode_selectors[i]->addWidget(iconLabel);
    //     m_mode_selectors[i]->addWidget(description);
    //     m_mode_layout->addLayout(m_mode_selectors[i]);
    // }

    // setLayout(m_mode_layout);
}
bool WiiMixModesWidget::eventFilter(QObject* obj, QEvent* event) {
    if ((event->type() == QEvent::HoverLeave)) {
        for (int i = 0; i < static_cast<int>(WiiMixEnums::Mode::END); i++) {
            QFrame* other_frame = qobject_cast<QFrame*>(m_mode_layout->itemAt(i)->widget());
            if (other_frame) {
                other_frame->setGraphicsEffect(nullptr);
                //other_frame->setStyleSheet(QStringLiteral(".QFrame { border: 2px solid transparent; border-radius: 5px; }"));
            }
        }
    }
    if (event->type() == QEvent::HoverEnter) {
        QFrame* frame = qobject_cast<QFrame*>(obj);
        if (frame) {
            // Highlight hovered frame
            std::string theme = Config::Get(Config::MAIN_THEME_NAME);
            auto *shadow = new QGraphicsDropShadowEffect;

            shadow->setBlurRadius(25);
            shadow->setXOffset(0);
            shadow->setYOffset(0);

            if (theme == "Clean") {
                shadow->setColor(Qt::black);
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
//                auto *colorizeEffect = new QGraphicsColorizeEffect;
//                colorizeEffect->setColor(Qt::black);
//                frame->setGraphicsEffect(colorizeEffect);
                frame->setGraphicsEffect(shadow);
                //frame->setStyleSheet(QStringLiteral(".QFrame {border: 2px solid black; border-radius: 5px; }"));
            }
            else if (theme == "Clean Blue") {
                shadow->setColor(Qt::blue);
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
//                auto *colorizeEffect = new QGraphicsColorizeEffect;
//                colorizeEffect->setColor(Qt::blue);
//                frame->setGraphicsEffect(colorizeEffect);
                frame->setGraphicsEffect(shadow);
                //frame->setStyleSheet(QStringLiteral(".QFrame {border: 2px solid blue; border-radius: 5px; }"));
            }
            else if (theme == "Clean Emerald"){
                shadow->setColor(Qt::green);
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
                //auto *colorizeEffect = new QGraphicsColorizeEffect;
                //colorizeEffect->setColor(Qt::green);
                //frame->setGraphicsEffect(colorizeEffect);
                frame->setGraphicsEffect(shadow);
                //frame->setStyleSheet(QStringLiteral(".QFrame {border: 2px solid green; border-radius: 5px; }"));
            }
            else if (theme == "Clean Lite"){
                shadow->setColor(Qt::white);
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
//                auto *colorizeEffect = new QGraphicsColorizeEffect;
//                colorizeEffect->setColor(Qt::white);
//                frame->setGraphicsEffect(colorizeEffect);
                frame->setGraphicsEffect(shadow);
                //frame->setStyleSheet(QStringLiteral(".QFrame { border: 2px solid white; border-radius: 5px; }"));
            }
            else if (theme == "Clean Pink"){
                shadow->setColor(QColor(255, 192, 203));
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
//                auto *colorizeEffect = new QGraphicsColorizeEffect;
//                colorizeEffect->setColor(QColor(255, 192, 203));
//                frame->setGraphicsEffect(colorizeEffect);
                frame->setGraphicsEffect(shadow);
                //frame->setFrameShadow(QFrame::Shadow::Raised);
                //frame->setStyleSheet(QStringLiteral(".QFrame { border: 2px solid pink; border-radius: 5px; }"));

            }
            else {
                qDebug("invalid theme name, applying default theme instead");
                //frame->setStyleSheet(QStringLiteral("QFrame { border: 5px solid black; }"));
            }
            return true;
        }
    }
    if (event->type() == QEvent::MouseButtonPress) {
        QFrame* frame = qobject_cast<QFrame*>(obj);
        if (frame) {
            // Deselect all other frames
            for (int i = 0; i < static_cast<int>(WiiMixEnums::Mode::END); i++) {
                QFrame* other_frame = qobject_cast<QFrame*>(m_mode_layout->itemAt(i)->widget());
                if (other_frame) {
                    other_frame->setGraphicsEffect(nullptr);
                    other_frame->setStyleSheet(QStringLiteral(".QFrame { border: 2px solid transparent; border-radius: 5px; }"));
                }
            }

            // Highlight the selected frame
            std::string theme = Config::Get(Config::MAIN_THEME_NAME);
            //TODOx: use some better colors, these are just awful
            auto *shadow = new QGraphicsDropShadowEffect;

            shadow->setBlurRadius(25);
            shadow->setXOffset(0);
            shadow->setYOffset(0);

            if (theme == "Clean") {
                shadow->setColor(Qt::black);
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
                auto *colorizeEffect = new QGraphicsColorizeEffect;
                colorizeEffect->setColor(Qt::black);
                frame->setGraphicsEffect(colorizeEffect);
                frame->setStyleSheet(QStringLiteral(".QFrame {border: 2px solid black; border-radius: 5px; }"));
            }
            else if (theme == "Clean Blue") {
                shadow->setColor(Qt::blue);
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
                auto *colorizeEffect = new QGraphicsColorizeEffect;
                colorizeEffect->setColor(Qt::blue);
                frame->setGraphicsEffect(colorizeEffect);
                frame->setStyleSheet(QStringLiteral(".QFrame {border: 2px solid blue; border-radius: 5px; }"));
            }
            else if (theme == "Clean Emerald"){
                shadow->setColor(Qt::green);
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
                auto *colorizeEffect = new QGraphicsColorizeEffect;
                colorizeEffect->setColor(Qt::green);
                frame->setGraphicsEffect(colorizeEffect);
                frame->setStyleSheet(QStringLiteral(".QFrame {border: 2px solid green; border-radius: 5px; }"));
            }
            else if (theme == "Clean Lite"){
                shadow->setColor(Qt::white);
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
                auto *colorizeEffect = new QGraphicsColorizeEffect;
                colorizeEffect->setColor(Qt::white);
                frame->setGraphicsEffect(colorizeEffect);
                frame->setStyleSheet(QStringLiteral(".QFrame { border: 2px solid white; border-radius: 5px; }"));
            }
            else if (theme == "Clean Pink"){
                shadow->setColor(QColor(255, 192, 203));
                //frame->setStyleSheet(QStringLiteral("QFrame {color: pink; }"));
                //frame->setGraphicsEffect(shadow);
                auto *colorizeEffect = new QGraphicsColorizeEffect;
                colorizeEffect->setColor(QColor(255, 192, 203));
                frame->setGraphicsEffect(colorizeEffect);
                frame->setStyleSheet(QStringLiteral(".QFrame { border: 2px solid pink; border-radius: 5px;}"));
                //std::vector<std::string> icons = std::vector<std::string>(9);
                //for (int i = 0; i < 9; i++) {
                //    icons[i] = "/Users/vdadvdad/RiderProjects/WiiMix/Data/Sys/Resources/Flag_Russia.png";
                //}
                //WiiMixOverlay::displayBingoBoard(icons);
                //Core::DisplayMessage("amogus", 10000);
                //OSD::DrawMessages();


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