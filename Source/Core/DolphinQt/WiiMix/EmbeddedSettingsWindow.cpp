
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QCheckBox>
#include <QSlider>
#include <QPushButton>
#include <QKeyEvent>
#include <QGraphicsDropShadowEffect>
#include "EmbeddedSettingsWindow.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"
#include "DolphinQt/Resources.h"
#include "Common/FileUtil.h"

EmbeddedSettingsWindow::EmbeddedSettingsWindow(QWidget *parent) {
    setParent(parent);
    setAutoFillBackground(true);
    QPixmap background = (Resources::GetResourceIcon("wiimix_background_top").pixmap(1200, 800));
    background.scaled(this->size());
    QPalette bg_palette;
    QBrush* backgroundBrush = new QBrush();
    backgroundBrush->setTexture(background);
    bg_palette.setBrush(QPalette::Window, *backgroundBrush);
    this->setPalette(bg_palette);
    //m_config_layout = new QVBoxLayout(this);
    QFont* font = new QFont();
    font->setPointSize(32);
    font->setBold(true);
    selectedSetting = 0;
    QVBoxLayout* shuffle_settings_outer_outer_layout = new QVBoxLayout(this);
    shuffle_settings_outer_outer_layout->installEventFilter(this);
    setFocusPolicy(Qt::TabFocus);
    QVBoxLayout* shuffle_settings_outer_layout = new QVBoxLayout();
    shuffle_settings_outer_layout->setContentsMargins(100, 100, 100, 100);
    QWidget* settingsLayoutWrapper = new QWidget();
    settingsLayoutWrapper->setStyleSheet(QStringLiteral(".QWidget{border-radius: 5px; background-color: rgba(0, 0, 0, 120)}"));
    QVBoxLayout* shuffle_settings_layout = new QVBoxLayout(settingsLayoutWrapper);
    returnButton = new QPushButton();
    returnButton->setFixedSize(100, 100);
    // connect(returnButton, &QPushButton::clicked, this, [this]() {
    //     qDebug() << "Back to upper widget";
    //     emit backToUpperWidget();
    // });

    // returnButton->installEventFilter(this);
    char *rButtonStyleSheet = (char *) malloc(400);
    std::string returnButtonPng = File::GetSysDirectory() + "Resources/wiimix_back_button.png";
    printf("%s\n", returnButtonPng.data());
    sprintf(rButtonStyleSheet, ".QPushButton{background-image: url(\"%s\"); background-color: transparent; border-color: transparent}", returnButtonPng.data());
    returnButton->setStyleSheet(QString::fromStdString(rButtonStyleSheet));
    shuffle_settings_outer_outer_layout->addWidget(returnButton);
    // shuffle_settings_layout->addSpacing(40);

    int num_switches = Config::Get(Config::WIIMIX_NUMBER_OF_SWITCHES);
    QHBoxLayout* num_switches_layout = new QHBoxLayout();

    QLabel* num_switches_label = new QLabel(tr("Number of Players:"));
    num_switches_label->setFont(*font);
    m_num_players = new QSpinBox();
    m_num_players->setRange(1, 2);
    num_switches_layout->addSpacing(120);
    num_switches_layout->addWidget(num_switches_label);
    num_switches_layout->addWidget(m_num_players_label);
    num_switches_layout->addSpacing(120);

    // shuffle_settings_layout->addWidget(num_switches_label);
    shuffle_settings_layout->addLayout(num_switches_layout);
    // shuffle_settings_layout->addLayout(endlessHLayout);
    QHBoxLayout* minSwitchHBox = new QHBoxLayout();
    m_min_switch_time_label = new QLabel(QStringLiteral("Min Time Between Shuffles: ") + QString::number(Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH)));
    m_min_switch_time_label->setFont(*font);
    m_min_time_between_switch = new QSlider(Qt::Horizontal);
    m_min_time_between_switch->setTracking(true);
    m_min_time_between_switch->setRange(DEFAULT_MIN_SWITCH_TIME, DEFAULT_MAX_SWITCH_TIME);
    m_min_time_between_switch->setValue(Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH));
    m_min_time_between_switch->setSliderPosition(Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH));
    QHBoxLayout* maxSwitchHBox = new QHBoxLayout();
    m_max_switch_time_label = new QLabel(QStringLiteral("Max Time Between Shuffles: ") + QString::number(Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH)));
    m_max_switch_time_label->setFont(*font);
    m_max_time_between_switch = new QSlider(Qt::Horizontal);
    m_max_time_between_switch->setTracking(true);
    m_max_time_between_switch->setRange(DEFAULT_MIN_SWITCH_TIME, DEFAULT_MAX_SWITCH_TIME);
    m_max_time_between_switch->setValue(Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH));
    m_max_time_between_switch->setSliderPosition(Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH));
    // minSwitchHBox->addSpacing(120);
    // setTabOrder(m_num_players, m_min_time_between_switch);
    minSwitchHBox->addWidget(m_min_switch_time_label);
    minSwitchHBox->addWidget(m_min_time_between_switch);
    // minSwitchHBox->addSpacing(120);
    shuffle_settings_layout->addLayout(minSwitchHBox);
    // maxSwitchHBox->addSpacing(120);
    setTabOrder(m_min_time_between_switch, m_max_time_between_switch);
    maxSwitchHBox->addWidget(m_max_switch_time_label);
    maxSwitchHBox->addWidget(m_max_time_between_switch);
    // maxSwitchHBox->addSpacing(120);
    shuffle_settings_layout->addLayout(maxSwitchHBox);
    QHBoxLayout* startButtonBox = new QHBoxLayout();
    //startButtonBox->addSpacing(120);
    startButton = new QPushButton(QStringLiteral("Start"));
    startButtonBox->addWidget(startButton);
    // connect(startButton, &QPushButton::clicked, this, [this]() {
    //     qDebug() << "Start Shuffle";
    //     WiiMixShuffleSettings::instance()->SetMinTimeBetweenSwitch(m_min_time_between_switch->value());
    //     WiiMixShuffleSettings::instance()->SetMaxTimeBetweenSwitch(m_max_time_between_switch->value());
    //     // Really just number of objectives 
    //     WiiMixShuffleSettings::instance()->SetNumberOfSwitches(3);
    //     emit StartWiiMixShuffle(WiiMixShuffleSettings::instance());
    // });
    // startButton->installEventFilter(this);
    // startButtonBox->addSpacing(120);
    shuffle_settings_layout->addLayout(startButtonBox);

    shuffle_settings_layout->addSpacing(100);
    shuffle_settings_outer_layout->addWidget(settingsLayoutWrapper);
    shuffle_settings_outer_outer_layout->addLayout(shuffle_settings_outer_layout);


    connect(m_num_players, &QSpinBox::textChanged, this, [this](const QString& text) {
        setNumSwitches(text.toInt());
    });

    // Connect endless_mode to disable/enable number of switches
    // connect(m_endless_mode, &QCheckBox::toggled, this, [this](bool checked) {
    //     setEndless(checked);
    // });

    // Connect slider value change to update the label
    connect(m_min_time_between_switch, &QSlider::valueChanged, this, [this](int value) {
        setMinTimeBetweenSwitch(value);
    });

    connect(m_max_time_between_switch, &QSlider::valueChanged, this, [this](int value) {
        setMaxTimeBetweenSwitch(value);
    });

    //shuffle_settings_box->setLayout(shuffle_settings_layout);
    //m_config_layout->addWidget(shuffle_settings_box);
}

// void EmbeddedSettingsWindow::setNumSwitches(int num_switches) {
//     Config::Set(Config::LayerType::Base, Config::WIIMIX_NUMBER_OF_SWITCHES, num_switches);
//     m_num_players->setText(QString::number(num_switches));
// }

// void EmbeddedSettingsWindow::setEndless(bool endless) {
//     m_endless_mode->setChecked(endless);
//     m_num_players->setDisabled(endless);
//     Config::Set(Config::LayerType::Base, Config::WIIMIX_IS_ENDLESS, endless);
// }

// WARNING: this combined with rerendering the entire window & network requests might be too costly
void EmbeddedSettingsWindow::setMinTimeBetweenSwitch(int min_time) {
    m_min_time_between_switch->setValue(min_time);
    m_min_time_between_switch->setSliderPosition(min_time);
    m_min_switch_time_label->setText(QStringLiteral("Min Time Between Shuffles: ") + QString::number(min_time));
    Config::Set(Config::LayerType::Base, Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH, min_time);
}

// WARNING: this combined with rerendering the entire window & network requests might be too costly
void EmbeddedSettingsWindow::setMaxTimeBetweenSwitch(int max_time) {
    m_max_time_between_switch->setValue(max_time);
    m_max_time_between_switch->setSliderPosition(max_time);
    m_max_switch_time_label->setText(QStringLiteral("Max Time Between Shuffles: ") + QString::number(max_time));
    Config::Set(Config::LayerType::Base, Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH, max_time);
}

void EmbeddedSettingsWindow::keyPressEvent(QKeyEvent *keyEvent) {
    qDebug() << "EmbeddedSettingsWindow caught keyboard event";
    if (keyEvent->key() == Qt::Key_S) {
        int prev_selected = selectedSetting;
        selectedSetting += 1;
        selectedSetting %= 4;
        if (prev_selected == 0) {
            returnButton->setGraphicsEffect(nullptr);
        }
        else if (prev_selected == 1) {
            m_num_players_label->setGraphicsEffect(nullptr);
        }
        else if (prev_selected == 2) {
            m_min_switch_time_label->setGraphicsEffect(nullptr);
        } else if (prev_selected == 3) {
            m_max_switch_time_label->setGraphicsEffect(nullptr);
        } else {
            startButton->setGraphicsEffect(nullptr);
        }
        QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect();
        glow->setColor(Qt::blue);
        glow->setOffset(0);
        glow->setBlurRadius(10);
        if (selectedSetting == 0) {
            returnButton->setGraphicsEffect(glow);
        }
        else if (selectedSetting == 1) {
            m_num_players_label->setGraphicsEffect(glow);
        }
        else if (selectedSetting == 2) {
            m_min_switch_time_label->setGraphicsEffect(glow);
        } else if (selectedSetting == 3) {
            m_max_switch_time_label->setGraphicsEffect(glow);
        } else {
            startButton->setGraphicsEffect(glow);
        }
    } else if (keyEvent->key() == Qt::Key_W) {
        int prev_selected = selectedSetting;
        selectedSetting -= 1;
        if (selectedSetting == -1) {
            selectedSetting += 4;
        }
        if (prev_selected == 0) {
            returnButton->setGraphicsEffect(nullptr);
        }
        else if (prev_selected == 1) {
            m_num_players_label->setGraphicsEffect(nullptr);
        }
        else if (prev_selected == 2) {
            m_min_switch_time_label->setGraphicsEffect(nullptr);
        } else if (prev_selected == 3) {
            m_max_switch_time_label->setGraphicsEffect(nullptr);
        } else {
            startButton->setGraphicsEffect(nullptr);
        }
        QGraphicsDropShadowEffect* glow = new QGraphicsDropShadowEffect();
        glow->setColor(Qt::blue);
        glow->setOffset(0);
        glow->setBlurRadius(10);
        if (selectedSetting == 0) {
            returnButton->setGraphicsEffect(glow);
        }
        else if (selectedSetting == 1) {
            m_num_players_label->setGraphicsEffect(glow);
        }
        else if (selectedSetting == 2) {
            m_min_switch_time_label->setGraphicsEffect(glow);
        } else if (selectedSetting == 3) {
            m_max_switch_time_label->setGraphicsEffect(glow);
        } else {
            startButton->setGraphicsEffect(glow);
        }
    } else if (keyEvent->key() == Qt::Key_D) {
        // increase something
        if (selectedSetting == 2) { // min time
            setMinTimeBetweenSwitch(m_min_time_between_switch->value() + 1);
        } else if (selectedSetting == 3) { // max time
            setMaxTimeBetweenSwitch(m_max_time_between_switch->value() + 1);
        }
        else if (selectedSetting == 1) { // max time
            setMaxTimeBetweenSwitch(m_num_players->value() + 1);
        }
    } else if (keyEvent->key() == Qt::Key_A) {
        // decrease something
        if (selectedSetting == 2) { // min time
            setMinTimeBetweenSwitch(m_min_time_between_switch->value() - 1);
        } else if (selectedSetting == 3) { // max time
            setMaxTimeBetweenSwitch(m_max_time_between_switch->value() - 1);
        }
        else if (selectedSetting == 1) { // max time
            setMaxTimeBetweenSwitch(m_num_players->value() - 1);
        }
    } 
    else if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
        if (selectedSetting == 0) {
            // return
            emit backToUpperWidget();
        }
        if (selectedSetting == 4) {
            // start
            qDebug() << "Start Shuffle";
            WiiMixShuffleSettings::instance()->SetMinTimeBetweenSwitch(m_min_time_between_switch->value());
            WiiMixShuffleSettings::instance()->SetMaxTimeBetweenSwitch(m_max_time_between_switch->value());
            if (m_num_players->value() == 1) {
                WiiMixShuffleSettings::instance()->SetObjectiveTypes({WiiMixEnums::ObjectiveType::SINGLE_PLAYER});
            } else {
                WiiMixShuffleSettings::instance()->SetObjectiveTypes({WiiMixEnums::ObjectiveType::VERSUS});
            }
            // Really just number of objectives
            WiiMixShuffleSettings::instance()->SetNumberOfSwitches(3);
            emit StartWiiMixShuffle(WiiMixShuffleSettings::instance());
        }
    }
};

bool EmbeddedSettingsWindow::eventFilter(QObject* obj, QEvent* event) {
    return QWidget::eventFilter(obj, event);
};