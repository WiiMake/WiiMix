
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QCheckBox>
#include <QSlider>
#include "EmbeddedSettingsWindow.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

EmbeddedSettingsWindow::EmbeddedSettingsWindow(QWidget *parent) {
    m_config_layout = new QVBoxLayout();

    QGroupBox* shuffle_settings_box = new QGroupBox(tr("Shuffle Settings"));
    QVBoxLayout* shuffle_settings_layout = new QVBoxLayout();

    int num_switches = Config::Get(Config::WIIMIX_NUMBER_OF_SWITCHES);
    QHBoxLayout* num_switches_layout = new QHBoxLayout();

    QLabel* num_switches_label = new QLabel(tr("Number of Objectives:"));
    m_num_switches = new QLineEdit();
    m_num_switches->setText(QString::number(num_switches));
    QIntValidator* int_validator = new QIntValidator(MIN_NUM_OBJECTIVES, MAX_NUM_OBJECTIVES, this);
    m_num_switches->setValidator(int_validator);

    m_endless_mode = new QCheckBox(tr("Endless Mode"));
    m_endless_mode->setChecked(Config::Get(Config::WIIMIX_IS_ENDLESS));
    m_num_switches->setDisabled(m_endless_mode->isChecked());

    num_switches_layout->addWidget(m_num_switches);
    num_switches_layout->addWidget(m_endless_mode);

    shuffle_settings_layout->addWidget(num_switches_label);
    shuffle_settings_layout->addLayout(num_switches_layout);

    m_min_switch_time_label = new QLabel(tr("Min Time Between Shuffles: 15"));
    m_min_time_between_switch = new QSlider(Qt::Horizontal);
    m_min_time_between_switch->setRange(DEFAULT_MIN_SWITCH_TIME, DEFAULT_MAX_SWITCH_TIME);
    m_min_time_between_switch->setValue(Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH));
    m_max_switch_time_label = new QLabel(tr("Max Time Between Shuffles: 60"));
    m_max_time_between_switch = new QSlider(Qt::Horizontal);
    m_max_time_between_switch->setRange(DEFAULT_MIN_SWITCH_TIME, DEFAULT_MAX_SWITCH_TIME);
    m_max_time_between_switch->setValue(Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH));
    shuffle_settings_layout->addWidget(m_min_switch_time_label);
    shuffle_settings_layout->addWidget(m_min_time_between_switch);
    shuffle_settings_layout->addWidget(m_max_switch_time_label);
    shuffle_settings_layout->addWidget(m_max_time_between_switch);

    connect(m_num_switches, &QLineEdit::textChanged, this, [this](const QString& text) {
        setNumSwitches(text.toInt());
    });

    // Connect endless_mode to disable/enable number of switches
    connect(m_endless_mode, &QCheckBox::toggled, this, [this](bool checked) {
        setEndless(checked);
    });

    // Connect slider value change to update the label
    connect(m_min_time_between_switch, &QSlider::valueChanged, this, [this](int value) {
        setMinTimeBetweenSwitch(value);
    });

    connect(m_max_time_between_switch, &QSlider::valueChanged, this, [this](int value) {
        setMaxTimeBetweenSwitch(value);
    });

    shuffle_settings_box->setLayout(shuffle_settings_layout);
    m_config_layout->addWidget(shuffle_settings_box);
}
void EmbeddedSettingsWindow::setNumSwitches(int num_switches) {
    Config::Set(Config::LayerType::Base, Config::WIIMIX_NUMBER_OF_SWITCHES, num_switches);
    m_num_switches->setText(QString::number(num_switches));
}

void EmbeddedSettingsWindow::setEndless(bool endless) {
    m_endless_mode->setChecked(endless);
    m_num_switches->setDisabled(endless);
    Config::Set(Config::LayerType::Base, Config::WIIMIX_IS_ENDLESS, endless);
}
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
    m_min_time_between_switch->setSliderPosition(max_time);
    m_max_switch_time_label->setText(QStringLiteral("Max Time Between Shuffles: ") + QString::number(max_time));
    Config::Set(Config::LayerType::Base, Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH, max_time);
}