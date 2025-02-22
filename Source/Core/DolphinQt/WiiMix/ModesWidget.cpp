#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QResizeEvent>
#include <QGraphicsDropShadowEffect>
#include <QToolTip>
#include <QString>

#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/GlobalSettings.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"
#include "VideoCommon/OnScreenDisplay.h"
#include "Core/Core.h"
#include "Overlay.h"
#include "Common/FileUtil.h"
#include "DolphinQt/Resources.h"
#include "WiiMixButton.h"
#include "BingoGame.h"


WiiMixModesWidget::WiiMixModesWidget(std::unique_ptr<BootParameters> boot_parameters) : QMainWindow(nullptr) {
    CreateLayout();
}

void WiiMixModesWidget::CreateLayout() {
    selected_mode_n = 0;
    m_mode_box = new QGroupBox(tr("Modes"));
    m_mode_layout = new QHBoxLayout();
    m_mode_box->setLayout(m_mode_layout);
    m_mode_selectors = {};
    for (int i = 0; i < static_cast<int>(WiiMixEnums::Mode::END); i++) {
        WiiMixButton* frame = new WiiMixButton();
        frame->setFocusPolicy(Qt::TabFocus);
        frame->setFixedSize(300, 169);
        // Set up WiiMixButton
        char * backgroundImagePath = (char *) malloc(200);
        strncpy(backgroundImagePath, (File::GetSysDirectory() + "Resources" + "/temporary_gradient_bg.png").data(), 200);
        frame->setBorderRadius(18);
        frame->setBorderWidth(4);
        frame->setBorderColor("gray");
        frame->setHasBackgroundImage(true);
        frame->setBackgroundImage(backgroundImagePath);
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
        QString titleText = WiiMixGlobalSettings::ModeToTitle(WiiMixEnums::Mode(i));
        if (titleText == QStringLiteral("")) {
            continue;
        }

        // Title
        QLabel* title = new QLabel(titleText);
        title->setFont(titleFont);
        title->setStyleSheet(QStringLiteral(".QLabel {color: white;}"));
        title->setAlignment(Qt::AlignCenter);
        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(title);
        m_mode_selectors[i]->setLayout(layout);
        m_mode_layout->addWidget(m_mode_selectors[i]);
    }
    for (int i = 0; i < static_cast<int>(WiiMixEnums::Mode::END) - 1; i++) {
        setTabOrder(m_mode_selectors[i], m_mode_selectors[i + 1]);
    }
    //setTabOrder(m_mode_selectors[static_cast<int>(WiiMixEnums::Mode::END) - 1], m_mode_selectors[0]);
    setLayout(m_mode_layout);
}

void WiiMixModesWidget::keyPressEvent(QKeyEvent *keyEvent) {
    qDebug() << "keyboard press detected";
    int prev_selected = selected_mode_n;
    if (keyEvent->key() == Qt::Key_D) {
        qDebug() << "D pressed";
        // move right
        selected_mode_n += 1;
        selected_mode_n %= 3;
    } else if (keyEvent->key() == Qt::Key_A) {
        // move left
        qDebug() << "A pressed";
        selected_mode_n -= 1;
        selected_mode_n += (selected_mode_n == -1 ? 3 : 0);
    }
    //TODO: add S key for selecting wiimix launch, or maybe even map it to start button
    WiiMixButton* other_frame = (WiiMixButton*)(m_mode_selectors[prev_selected]);
    if (other_frame) {
        other_frame->setGraphicsEffect(nullptr);
    }
    WiiMixButton* frame = (WiiMixButton *)(m_mode_selectors[selected_mode_n]);
    // Highlight hovered frame
    std::string theme = Config::Get(Config::MAIN_THEME_NAME);
    auto *shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(25);
    shadow->setXOffset(0);
    shadow->setYOffset(0);

    if (theme == "Clean") {
        shadow->setColor(Qt::black);
    } else if (theme == "Clean Blue") {
        shadow->setColor(Qt::blue);
    } else if (theme == "Clean Emerald") {
        shadow->setColor(Qt::green);
    } else if (theme == "Clean Lite") {
        shadow->setColor(Qt::white);
    } else if (theme == "Clean Pink") {
        shadow->setColor(QColor(255, 192, 203));

    } else {
        qDebug("invalid theme name");
    }
    frame->setGraphicsEffect(shadow);
    if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
        if (frame) {
            std::string theme = Config::Get(Config::MAIN_THEME_NAME);
            // Highlight the selected frame
            if (theme == "Clean") { frame->setBorderColor("black"); }
            else if (theme == "Clean Blue") { frame->setBorderColor("blue"); }
            else if (theme == "Clean Emerald") { frame->setBorderColor("green"); }
            else if (theme == "Clean Lite") { frame->setBorderColor("transparent"); }
            else if (theme == "Clean Pink") { frame->setBorderColor("rgb(255, 192, 203)"); }
            else {
                qDebug("invalid theme name, applying default theme instead");
                frame->setStyleSheet(QStringLiteral("QFrame { border: 5px solid black; }"));
            }
            // Trigger configuration update for this WiiMixEnums::Mode
            WiiMixEnums::Mode selected_mode = static_cast<WiiMixEnums::Mode>(m_mode_layout->indexOf(frame));
            if (selected_mode == WiiMixEnums::Mode::SHUFFLE) {
                emit ShuffleSelected(selected_mode);
            }
        }
    }
}

bool WiiMixModesWidget::eventFilter(QObject* obj, QEvent* event) {
    if ((event->type() == QEvent::HoverLeave)) {
        //QToolTip::hideText();
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
            }
            else if (theme == "Clean Blue") {
                shadow->setColor(Qt::blue);
            }
            else if (theme == "Clean Emerald"){
                shadow->setColor(Qt::green);
            }
            else if (theme == "Clean Lite"){
                shadow->setColor(Qt::white);
            }
            else if (theme == "Clean Pink"){
                shadow->setColor(QColor(255, 192, 203));

            }
            else {
                qDebug("invalid theme name");
            }
            frame->setGraphicsEffect(shadow);
            //std::string name = "Hello";
            //std::string description = "HelloHelloHello";
            //std::string icon = File::GetSysDirectory() + "Resources/Flag_Russia@2x.png";
            //std::ifstream t(File::GetSysDirectory() + "Resources/toolTip.html");
            //std::cout << t.is_open() << "\n";
            //std::stringstream buffer;
            //buffer << t.rdbuf();
            //std::cout << (buffer.str()) << "\n";
            //char toolTipString[1000];
            //snprintf(toolTipString, 999, buffer.str().data(), icon.data(), name.data(), description.data());
            //std::cout << toolTipString << "\n";
            //snprintf(toolTipString, 999, "<style> @keyframes appearance {0%% {opacity: 0} 60%% {opacity: 0.6} 100%% {opacity: 1}} .container{display: flex;} img {max-width: 25%%; max-height:15%%; float: left;} .text {font-size: 20px; padding-left: 20px; padding-top: 20%%; float: left; } .div {animation-duration: 5s; animation-timing-function: ease-in; animation-name: appearance}</style> <div class='container'> <div class='image'> <img src='%s'> </div> <div class='text'> <h2>%s</h2> </div> </div>\n%s", icon.data(), name.data(), description.data());

            //QToolTip::showText(QCursor::pos(), QString::fromStdString(toolTipString), this, QRect(), 10000);
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
            // std::vector<BingoGame::BingoItem> b;
            // b.reserve(25);
            // for (int i = 0; i < 25; i++) {
            //     BingoGame::BingoItem bItem;
            //     bItem.name = "Hello";
            //     bItem.description = "HelloHelloHello";
            //     bItem.icon = "Flag_Russia@2x";
            //     bItem.iconFull = File::GetSysDirectory() + "Resources/Flag_Russia@2x.png";
            //     b.push_back(bItem);
            // }
            // BingoGame::createObjectiveWindow(b);
            std::string theme = Config::Get(Config::MAIN_THEME_NAME);

            // Highlight the selected frame
            if (theme == "Clean") { frame->setBorderColor("black"); }
            else if (theme == "Clean Blue") { frame->setBorderColor("blue"); }
            else if (theme == "Clean Emerald") { frame->setBorderColor("green"); }
            else if (theme == "Clean Lite"){ frame->setBorderColor("transparent"); }
            else if (theme == "Clean Pink"){ frame->setBorderColor("rgb(255, 192, 203)"); }
            else {
                qDebug("invalid theme name, applying default theme instead");
                frame->setStyleSheet(QStringLiteral("QFrame { border: 5px solid black; }"));
            }
            // Trigger configuration update for this WiiMixEnums::Mode
            WiiMixEnums::Mode selected_mode = static_cast<WiiMixEnums::Mode>(m_mode_layout->indexOf(frame));
            if (selected_mode == WiiMixEnums::Mode::SHUFFLE) {
                emit ShuffleSelected(selected_mode);
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}