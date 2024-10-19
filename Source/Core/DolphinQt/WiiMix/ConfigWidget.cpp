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
#include <QList>
#include <QDialog>

#include "Core/Config/MainSettings.h"

#include "DolphinQt/WiiMix/BingoLobbyWidget.h"
#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/ConfigWidget.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"
#include "DolphinQt/WiiMix/Settings.h"
#include "DolphinQt/Resources.h"

WiiMixConfigWidget::WiiMixConfigWidget(QWidget* parent) : QDialog(parent) {
    ConnectWidgets();
}

void WiiMixConfigWidget::CreateLayout(WiiMixEnums::Mode mode) {
    // m_config_box = new QGroupBox(tr("Configuration"));
    QVBoxLayout* config_layout = new QVBoxLayout();

    if (mode == WiiMixEnums::Mode::BINGO) {
        bool is_lockout = Config::Get(Config::WIIMIX_IS_LOCKOUT);
        m_bingo_button = new QRadioButton(tr("Bingo"));
        m_bingo_button->setChecked(!is_lockout);

        connect(m_bingo_button, &QRadioButton::toggled, this, [this](bool checked) {
            m_lockout_button->setChecked(!checked);
            Config::Set(Config::LayerType::Base, Config::WIIMIX_IS_LOCKOUT, !checked);
        });

        m_lockout_button = new QRadioButton(tr("Lockout"));
        m_lockout_button->setChecked(is_lockout);

        connect(m_lockout_button, &QRadioButton::toggled, this, [this](bool checked) {
            m_bingo_button->setChecked(!checked);
            Config::Set(Config::LayerType::Base, Config::WIIMIX_IS_LOCKOUT, checked);
        });

        QHBoxLayout* mode_layout = new QHBoxLayout();
        config_layout->addWidget(m_bingo_button);
        config_layout->addWidget(m_lockout_button);
        // config_layout->addLayout(mode_layout);
        
        QLabel* card_size_label = new QLabel(tr("Card Size:"));
        int card_size = Config::Get(Config::WIIMIX_CARD_SIZE);
        m_card_size = new QComboBox();
        m_card_size->addItem(QStringLiteral("3x3"));
        m_card_size->addItem(QStringLiteral("5x5"));
        m_card_size->addItem(QStringLiteral("7x7"));
        if (card_size == 9) {
            m_card_size->setCurrentIndex(0);
        }
        else if (card_size == 25) {
            m_card_size->setCurrentIndex(1);
        }
        else if (card_size == 49) {
            m_card_size->setCurrentIndex(2);
        }

        connect(m_card_size, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
            int card_size = 0;
            if (index == 0) {
                card_size = 9;
            }
            else if (index == 1) {
                card_size = 25;
            }
            else if (index == 2) {
                card_size = 49;
            }
            Config::Set(Config::LayerType::Base, Config::WIIMIX_CARD_SIZE, card_size);
        });

        // Add bingo lobby widget
        // WiiMixBingoLobbyWidget* bingo_lobby = new WiiMixBingoLobbyWidget(this);

        m_bingo_lobby = new QGroupBox(tr("Bingo Lobby"));
        QHBoxLayout* teams_enabled_layout = new QHBoxLayout();
        m_teams_enabled = new QCheckBox();
        m_team_selectors = QList<QCheckBox*>();
        m_team_selector_states = QList<std::string>();

        connect(m_teams_enabled, &QCheckBox::clicked, this, [this](bool checked) {
            Config::Set(Config::LayerType::Base, Config::WIIMIX_TEAMS, checked);
            if (checked) {
                // If teams is toggled, sets the players to be on teams
                int mid = m_team_selectors.size() / 2;
                for (int i = 0; i < mid; i++) {
                    m_team_selectors.at(i)->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(WII_MIX_RED)));
                    m_team_selector_states[i] = WII_MIX_RED;
                }
                for (int i = mid; i < m_team_selectors.size(); i++) {
                    m_team_selectors.at(i)->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(WII_MIX_BLUE)));
                    m_team_selector_states[i] = WII_MIX_BLUE;
                }
            }
            else {
                // If teams is not toggled, give each player it's individual color again
                for (int i = 0; i < MAX_PLAYERS; i++) {
                    // Enable or disable team indicators based on the checkbox state
                    m_team_selectors.at(i)->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(ColorToHex(static_cast<WiiMixEnums::Color>(i)))));
                    m_team_selector_states[i] = ColorToHex(static_cast<WiiMixEnums::Color>(i));
                }
            }
        });

        QVBoxLayout* lobby_layout = new QVBoxLayout();
        for (int i = 0; i < MAX_PLAYERS; i++) {
            QHBoxLayout* player_layout = new QHBoxLayout();
            player_layout->setAlignment(Qt::AlignLeft);
            QCheckBox* team_selector = new QCheckBox();
            team_selector->setStyleSheet(QStringLiteral(".QCheckBox { background-color: %1; }").arg(QString::fromStdString(ColorToHex(static_cast<WiiMixEnums::Color>(i)))));
            connect(team_selector, &QCheckBox::clicked, this, [this, team_selector](bool checked) {
                int index = m_team_selectors.indexOf(team_selector);
                qDebug() << "Index: " << index;
                team_selector->setChecked(false);
                // Change the color to indicate team selection
                // If teams, select the next color regardless of whether someone has it selected or not
                if (m_teams_enabled->isChecked()) {
                    int color_index = static_cast<int>(WiiMixEnums::StringToColor(m_team_selector_states[index]));
                    qDebug() << "color_index: " << color_index;
                    color_index += 1;
                    qDebug() << "end: " << static_cast<int>(WiiMixEnums::Color::END);
                    color_index %= static_cast<int>(WiiMixEnums::Color::END); 
                    m_team_selector_states[index] = ColorToHex(static_cast<WiiMixEnums::Color>(color_index));
                    m_team_selectors.at(index)->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(ColorToHex(static_cast<WiiMixEnums::Color>(color_index)))));
                }
                    // else {
                    //     // If not teams, select the next available color
                    //     int color_index = static_cast<int>(WiiMixEnums::StringToColor(m_team_selector_states[index]));
                    //     for (int i = 0; i < 4; i++) {
                    //         color_index += 1;
                    //         if (color_index > static_cast<int>(WiiMixEnums::Color::END)) {
                    //             color_index = 0;
                    //         }
                    //         if (color_index ) {

                    //         }   
                    //     }
                    //     m_team_selectors.at(index)->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(WiiMixEnums::ColorToHex(static_cast<WiiMixEnums::Color>(color_index)))));
                    // }
                }
                // } else {
                //     // Reset the color to the individual player's color
                //     int index = m_team_selectors.indexOf(team_selector);
                //     team_selector->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(WiiMixEnums::ColorToHex(static_cast<WiiMixEnums::Color>(index)))));
                // }
            );
            QLabel* player_widget = new QLabel(tr("Player ") + QString::number(i + 1) + tr(":"));
            // TODOx: fix this event with player joined
            // connect(player_widget, &QLabel::linkActivated, this, [this, i]() {
            //     // Open a dialog to select a player
            // });
            m_team_selectors.append(team_selector);
            m_team_selector_states.append(ColorToHex(static_cast<WiiMixEnums::Color>(i)));
            player_layout->addWidget(m_team_selectors.at(i));
            player_layout->addWidget(player_widget);
            lobby_layout->addLayout(player_layout);
        }
        m_bingo_lobby->setLayout(lobby_layout);
        // Each layout contains # of connected players and some unique identifier for each player

        config_layout->addWidget(card_size_label);
        config_layout->addWidget(m_card_size);
        QLabel* teams_enabled_label = new QLabel(tr("Teams Enabled:"));
        teams_enabled_layout->addWidget(teams_enabled_label);
        teams_enabled_layout->addWidget(m_teams_enabled);
        config_layout->addLayout(teams_enabled_layout);
        // config_layout->addWidget(m_teams_enabled);
        config_layout->addWidget(m_bingo_lobby);
    }
    else if (mode == WiiMixEnums::Mode::SHUFFLE) {
        int num_switches = Config::Get(Config::WIIMIX_NUMBER_OF_SWITCHES);
        QHBoxLayout* num_switches_layout = new QHBoxLayout();

        QLabel* num_switches_label = new QLabel(tr("Number of Switches:"));
        m_num_switches = new QLineEdit();
        m_num_switches->setText(QString::number(num_switches));
        QIntValidator* int_validator = new QIntValidator(MIN_NUM_OBJECTIVES, MAX_NUM_OBJECTIVES, this);
        m_num_switches->setValidator(int_validator);

        m_endless_mode = new QCheckBox(tr("Endless Mode"));
        m_endless_mode->setChecked(Config::Get(Config::WIIMIX_IS_ENDLESS));

        num_switches_layout->addWidget(m_num_switches);
        num_switches_layout->addWidget(m_endless_mode);

        config_layout->addWidget(num_switches_label);
        config_layout->addLayout(num_switches_layout);

        QLabel* min_switch_time_label = new QLabel(tr("Min Time Between Shuffles: 15"));
        m_min_time_between_switch = new QSlider(Qt::Horizontal);
        m_min_time_between_switch->setRange(DEFAULT_MIN_SWITCH_TIME, DEFAULT_MAX_SWITCH_TIME);
        m_min_time_between_switch->setValue(Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH));
        QLabel* max_switch_time_label = new QLabel(tr("Max Time Between Shuffles: 60"));
        m_max_time_between_switch = new QSlider(Qt::Horizontal);
        m_max_time_between_switch->setRange(DEFAULT_MIN_SWITCH_TIME, DEFAULT_MAX_SWITCH_TIME);
        m_max_time_between_switch->setValue(Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH));
        config_layout->addWidget(min_switch_time_label);
        config_layout->addWidget(m_min_time_between_switch);
        config_layout->addWidget(max_switch_time_label);
        config_layout->addWidget(m_max_time_between_switch);
        
        connect(m_num_switches, &QLineEdit::textChanged, this, [this](const QString& text) {
            Config::Set(Config::LayerType::Base, Config::WIIMIX_NUMBER_OF_SWITCHES, text.toInt());
        });

        connect(m_endless_mode, &QCheckBox::toggled, this, [this](bool checked) {
            Config::Set(Config::LayerType::Base, Config::WIIMIX_IS_ENDLESS, checked);
        });

        // Connect endless_mode to disable/enable number of switches
        connect(m_endless_mode, &QCheckBox::toggled, this, [this](bool checked) {
            m_num_switches->setDisabled(checked);
        });

        // Connect slider value change to update the label
        connect(m_min_time_between_switch, &QSlider::valueChanged, this, [min_switch_time_label](int value) {
            min_switch_time_label->setText(QStringLiteral("Min Time Between Switches: ") + QString::number(value));
            Config::Set(Config::LayerType::Base, Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH, value);
        });
        connect(m_max_time_between_switch, &QSlider::valueChanged, this, [max_switch_time_label](int value) {
            max_switch_time_label->setText(QStringLiteral("Max Time Between Switches: ") + QString::number(value));
            Config::Set(Config::LayerType::Base, Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH, value);
        });
    }
    else if (mode == WiiMixEnums::Mode::ROGUE) {
        // QLabel* num_stages_label = new QLabel(tr("Number of Stages:"));
        // QLineEdit* m_num_stages = new QLineEdit();
        // QIntValidator* int_validator = new QIntValidator(MIN_NUM_OBJECTIVES, MAX_NUM_OBJECTIVES, this);
        // m_num_stages->setValidator(int_validator);
        // config_layout->addWidget(num_stages_label);
        // config_layout->addWidget(m_num_stages);
    }

    // WiiMixEnums::Difficulty
    QLabel* difficulty_label = new QLabel(tr("Objective Difficulty:"));
    m_difficulty = new QComboBox();
    for (int i = 0; i < static_cast<int>(WiiMixEnums::Difficulty::END); i++) {
        m_difficulty->addItem(WiiMixSettings::DifficultyToString(static_cast<WiiMixEnums::Difficulty>(i)));
    }
    WiiMixEnums::Difficulty difficulty = Config::Get(Config::WIIMIX_DIFFICULTY);
    m_difficulty->setCurrentIndex(static_cast<int>(difficulty));
    config_layout->addWidget(difficulty_label);
    config_layout->addWidget(m_difficulty);

    // Time (maybe I make this an estimate at the bottom rather than a parameter)
    // QLabel* time_label = new QLabel(tr("Time:"));
    // m_time = new QLabel();
    // config_layout->addWidget(time_label);
    // config_layout->addWidget(m_time);

    QLabel* save_state_label = new QLabel(tr("Save State Bank:"));
    m_save_state_bank = new QComboBox();
    for (int i = 0; i < static_cast<int>(WiiMixEnums::SaveStateBank::END); i++) {
        m_save_state_bank->addItem(WiiMixSettings::SaveStateBankToString(static_cast<WiiMixEnums::SaveStateBank>(i)));
    }
    WiiMixEnums::SaveStateBank bank = Config::Get(Config::WIIMIX_SAVE_STATE_BANK);
    m_save_state_bank->setCurrentIndex(static_cast<int>(bank));
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