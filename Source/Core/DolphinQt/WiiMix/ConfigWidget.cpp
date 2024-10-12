#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QRadioButton>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QLineEdit>
#include <QDialog>

#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/ConfigWidget.h"
#include "DolphinQt/WiiMix/Settings.h"
#include "DolphinQt/Resources.h"

WiiMixConfigWidget::WiiMixConfigWidget(QWidget* parent) : QDialog(parent) {
    ConnectWidgets();
}

void WiiMixConfigWidget::CreateLayout(WiiMixSettings::Mode mode) {
    // m_config_box = new QGroupBox(tr("Configuration"));
    QVBoxLayout* config_layout = new QVBoxLayout();

    if (mode == WiiMixSettings::Mode::BINGO) {
        m_bingo_button = new QRadioButton(tr("Bingo"));
        m_bingo_button->setEnabled(true);
        m_lockout_button = new QRadioButton(tr("Lockout"));
        QHBoxLayout* mode_layout = new QHBoxLayout();
        config_layout->addWidget(m_bingo_button);
        config_layout->addWidget(m_lockout_button);
        // config_layout->addLayout(mode_layout);

        QLabel* card_size_label = new QLabel(tr("Card Size:"));
        m_card_size = new QComboBox();
        m_card_size->addItem(QStringLiteral("3x3"));
        m_card_size->addItem(QStringLiteral("5x5"));
        m_card_size->addItem(QStringLiteral("7x7"));
        config_layout->addWidget(card_size_label);
        config_layout->addWidget(m_card_size);
    }
    else if (mode == WiiMixSettings::Mode::SHUFFLE) {
        QHBoxLayout* num_switches_layout = new QHBoxLayout();

        QLabel* num_switches_label = new QLabel(tr("Number of Switches:"));
        m_num_switches = new QLineEdit();
        QIntValidator* int_validator = new QIntValidator(MIN_NUM_OBJECTIVES, MAX_NUM_OBJECTIVES, this);
        m_num_switches->setValidator(int_validator);

        m_endless_mode = new QCheckBox(tr("Endless Mode"));

        num_switches_layout->addWidget(m_num_switches);
        num_switches_layout->addWidget(m_endless_mode);

        config_layout->addWidget(num_switches_label);
        config_layout->addLayout(num_switches_layout);

        QLabel* min_switch_time_label = new QLabel(tr("Min Time Between Shuffles: 15"));
        m_min_time_between_switch = new QSlider(Qt::Horizontal);
        m_min_time_between_switch->setRange(MIN_SWITCH_TIME, MAX_SWITCH_TIME);
        QLabel* max_switch_time_label = new QLabel(tr("Max Time Between Shuffles: 60"));
        m_max_time_between_switch = new QSlider(Qt::Horizontal);
        m_max_time_between_switch->setRange(MIN_SWITCH_TIME, MAX_SWITCH_TIME);
        m_max_time_between_switch->setValue(MAX_SWITCH_TIME);
        config_layout->addWidget(min_switch_time_label);
        config_layout->addWidget(m_min_time_between_switch);
        config_layout->addWidget(max_switch_time_label);
        config_layout->addWidget(m_max_time_between_switch);
        
        // Connect endless_mode to disable/enable number of switches
        connect(m_endless_mode, &QCheckBox::toggled, this, [this](bool checked) {
            m_num_switches->setDisabled(checked);
        });

        // Connect slider value change to update the label
        connect(m_min_time_between_switch, &QSlider::valueChanged, this, [min_switch_time_label](int value) {
            min_switch_time_label->setText(QStringLiteral("Min Time Between Switches: ") + QString::number(value));
        });
        connect(m_max_time_between_switch, &QSlider::valueChanged, this, [max_switch_time_label](int value) {
            max_switch_time_label->setText(QStringLiteral("Max Time Between Switches: ") + QString::number(value));
        });
    }
    else if (mode == WiiMixSettings::Mode::ROGUE) {
        // QLabel* num_stages_label = new QLabel(tr("Number of Stages:"));
        // QLineEdit* m_num_stages = new QLineEdit();
        // QIntValidator* int_validator = new QIntValidator(MIN_NUM_OBJECTIVES, MAX_NUM_OBJECTIVES, this);
        // m_num_stages->setValidator(int_validator);
        // config_layout->addWidget(num_stages_label);
        // config_layout->addWidget(m_num_stages);
    }

    // Difficulty
    QLabel* difficulty_label = new QLabel(tr("Objective Difficulty:"));
    m_difficulty = new QComboBox();
    for (int i = 0; i < static_cast<int>(WiiMixSettings::Difficulty::END); i++) {
        m_difficulty->addItem(WiiMixSettings::DifficultyToString(static_cast<WiiMixSettings::Difficulty>(i)));
    }
    config_layout->addWidget(difficulty_label);
    config_layout->addWidget(m_difficulty);

    // Time (maybe I make this an estimate at the bottom rather than a parameter)
    // QLabel* time_label = new QLabel(tr("Time:"));
    // m_time = new QLabel();
    // config_layout->addWidget(time_label);
    // config_layout->addWidget(m_time);

    QLabel* save_state_label = new QLabel(tr("Save State Bank:"));
    m_save_state_bank = new QComboBox();
    for (int i = 0; i < static_cast<int>(WiiMixSettings::SaveStateBank::END); i++) {
        m_save_state_bank->addItem(WiiMixSettings::SaveStateBankToString(static_cast<WiiMixSettings::SaveStateBank>(i)));
    }
    config_layout->addWidget(save_state_label);
    config_layout->addWidget(m_save_state_bank);

    setLayout(config_layout);
}

// NOTE: these getters aren't null safe rn
QString WiiMixConfigWidget::GetDifficulty() const {
    return m_difficulty->currentText();
}

QString WiiMixConfigWidget::GetSaveStateBank() const {
    return m_save_state_bank->currentText();
}

int WiiMixConfigWidget::GetNumSwitches() const {
    return m_num_switches->text().toInt();
}

bool WiiMixConfigWidget::GetEndless() const {
    return m_endless_mode->isChecked();
}

bool WiiMixConfigWidget::GetIsLockout() const {
    return m_lockout_button->isChecked();
}

QString WiiMixConfigWidget::GetCardSize() const {
    return m_card_size->currentText();
}

void WiiMixConfigWidget::ConnectWidgets() {
    return;
}

int WiiMixConfigWidget::GetMinTimeBetweenSwitch() const
{
    return m_min_time_between_switch->value();
}

int WiiMixConfigWidget::GetMaxTimeBetweenSwitch() const
{
    return m_max_time_between_switch->value();
}