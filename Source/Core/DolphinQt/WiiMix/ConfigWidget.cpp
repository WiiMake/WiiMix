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
#include <QPushButton>
#include <QMenuBar>
#include <QFormLayout>

#include "Core/Config/MainSettings.h"

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/ConfigWidget.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"
#include "DolphinQt/WiiMix/Settings.h"
#include "DolphinQt/Resources.h"

WiiMixConfigWidget::WiiMixConfigWidget(QWidget* parent) : QDialog(parent) {
    ConnectWidgets();
}

void WiiMixConfigWidget::CreateBingoLayout(QString menu) {
    m_config_layout = new QVBoxLayout();

    // Separate menu_layout for the host and the connector
    QHBoxLayout* menu_layout = new QHBoxLayout();
    // Create the menu bar
    m_menu_bar = new QMenuBar();

    // Create actions for Host and Connect
    QAction* host_action = new QAction(tr("Host"), this);
    QAction* connect_action = new QAction(tr("Connect"), this);

    // Add the actions directly to the menu bar
    m_menu_bar->addAction(host_action);
    m_menu_bar->addAction(connect_action);

    // Connect the actions to your slots or lambda functions
    connect(host_action, &QAction::triggered, this, [this]() {
        // Delete the layout
        QLayoutItem* item;
        while ((item = m_config_layout->takeAt(0)) != nullptr)
        {
            delete item->widget(); // Delete the widgets in the layout
            delete item;           // Delete the layout item itself
        }
        delete m_config_layout;
        CreateBingoLayout(QStringLiteral("Host"));
        CreateCommonLayout();
        // Set the layout
        setLayout(m_config_layout);
    });

    connect(connect_action, &QAction::triggered, this, [this]() {
        // Delete the layout
        QLayoutItem* item;
        while ((item = m_config_layout->takeAt(0)) != nullptr)
        {
            delete item->widget(); // Delete the widgets in the layout
            delete item;           // Delete the layout item itself
        }
        delete m_config_layout;
        CreateBingoLayout(QStringLiteral("Connect"));
        // Set the layout
        setLayout(m_config_layout);
    });

    menu_layout->addStretch();
    menu_layout->addWidget(m_menu_bar);
    menu_layout->addStretch();

    m_config_layout->addLayout(menu_layout);

    if (menu == QStringLiteral("Host")) {
        // Create host settings
        
        // Connection Settings
        QGroupBox* connection_box = new QGroupBox(tr("Connection Settings"));
        QFormLayout* connection_layout = new QFormLayout();

        QLabel* bingo_lobby_id = new QLabel(tr("Lobby ID"));
        m_bingo_lobby_id = new QLineEdit();
        m_bingo_lobby_id->setReadOnly(true);

        connection_layout->addRow(bingo_lobby_id, m_bingo_lobby_id);

        QLabel* player_name = new QLabel(tr("Player Name"));
        m_bingo_player_name = new QLineEdit();

        connection_layout->addRow(player_name, m_bingo_player_name);

        QLabel* bingo_lobby_password = new QLabel(tr("Password"));
        m_bingo_lobby_password = new QLineEdit();
        m_bingo_lobby_password->setEchoMode(QLineEdit::Password);

        connection_layout->addRow(bingo_lobby_password, m_bingo_lobby_password);

        m_connect_button = new QPushButton();
        m_connect_button->setText(tr("Host"));
        connect(m_connect_button, &QPushButton::clicked, this, [this]() {
            // TODOx: Set the bingo lobby ID, the player[x] name, and the lobby password
            // Make sure to update the text of m_bingo_lobby_id so it can be shared with a friend!
            // TODOx: Host the bingo lobby
        });

        connection_layout->addWidget(m_connect_button);

        connection_box->setLayout(connection_layout);
        m_config_layout->addWidget(connection_box);

        QVBoxLayout* bingo_settings_layout = new QVBoxLayout();
        QGroupBox* bingo_box = new QGroupBox(tr("Bingo Settings"));

        // Lobby settings
        QVBoxLayout* lobby_layout = new QVBoxLayout();

        QHBoxLayout* teams_enabled_layout = new QHBoxLayout();
        m_teams_enabled = new QCheckBox();
        m_team_selectors = QList<QCheckBox*>();
        m_team_selector_states = QList<std::string>();

        QLabel* teams_enabled_label = new QLabel(tr("Teams Enabled:"));
        teams_enabled_layout->addWidget(teams_enabled_label);
        teams_enabled_layout->addWidget(m_teams_enabled);
        lobby_layout->addLayout(teams_enabled_layout);
        for (int i = 0; i < MAX_PLAYERS; i++) {
            QHBoxLayout* player_layout = new QHBoxLayout();
            player_layout->setAlignment(Qt::AlignLeft);
            QCheckBox* team_selector = new QCheckBox();
            team_selector->setStyleSheet(QStringLiteral(".QCheckBox { background-color: %1; }").arg(QString::fromStdString(ColorToHex(static_cast<WiiMixEnums::Color>(i)))));
            connect(team_selector, &QCheckBox::clicked, this, [this, team_selector](bool checked) {
                int index = m_team_selectors.indexOf(team_selector);
                team_selector->setChecked(false);
                // Change the color to indicate team selection
                // If teams, select the next color regardless of whether someone has it selected or not
                if (m_teams_enabled->isChecked()) {
                    int color_index = static_cast<int>(WiiMixEnums::StringToColor(m_team_selector_states[index]));
                    color_index += 1;
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
        QGroupBox* bingo_lobby = new QGroupBox(tr("Bingo Lobby"));
        bingo_lobby->setLayout(lobby_layout);

        m_config_layout->addWidget(bingo_lobby);

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

        WiiMixEnums::BingoType bingo_type = Config::Get(Config::WIIMIX_BINGO_TYPE);
        m_bingo_button = new QRadioButton(tr("Bingo"));
        if (bingo_type == WiiMixEnums::BingoType::BINGO) {
            m_bingo_button->setChecked(true);
        }
        else {
            m_bingo_button->setChecked(false);
        }

        connect(m_bingo_button, &QRadioButton::toggled, this, [this](bool checked) {
            m_lockout_button->setChecked(!checked);
            if (checked) {
                Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_TYPE, WiiMixEnums::BingoType::BINGO);
            }
        });

        bingo_settings_layout->addWidget(m_bingo_button);

        m_lockout_button = new QRadioButton(tr("Lockout"));
        if (bingo_type == WiiMixEnums::BingoType::LOCKOUT) {
            m_lockout_button->setChecked(true);
        }
        else {
            m_lockout_button->setChecked(false);
        }

        bingo_settings_layout->addWidget(m_lockout_button);

        connect(m_lockout_button, &QRadioButton::toggled, this, [this](bool checked) {
            m_bingo_button->setChecked(!checked);
            if (checked) {
                Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_TYPE, WiiMixEnums::BingoType::LOCKOUT);
            }
        });

        m_time_attack_button = new QRadioButton(tr("Time Attack"));
        if (bingo_type == WiiMixEnums::BingoType::TIME_ATTACK) {
            m_time_attack_button->setChecked(true);
        }
        else {
            m_time_attack_button->setChecked(false);
        }

        connect(m_time_attack_button, &QRadioButton::toggled, this, [this](bool checked) {
            m_bingo_button->setChecked(!checked);
            if (checked) {
                Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_TYPE, WiiMixEnums::BingoType::TIME_ATTACK);
            }
        });

        bingo_settings_layout->addWidget(m_time_attack_button);
        
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
            int size = 0;
            if (index == 0) {
                size = 9;
            }
            else if (index == 1) {
                size = 25;
            }
            else if (index == 2) {
                size = 49;
            }
            Config::Set(Config::LayerType::Base, Config::WIIMIX_CARD_SIZE, size);
        });

        bingo_settings_layout->addWidget(card_size_label);
        bingo_settings_layout->addWidget(m_card_size);

        bingo_box->setLayout(bingo_settings_layout);
        m_config_layout->addWidget(bingo_box);
    }
    else if (menu == QStringLiteral("Connect")) {
        // Create connection settings
        QGroupBox* connection_box = new QGroupBox(tr("Connection Settings"));
        QFormLayout* connection_layout = new QFormLayout();

        QLabel* bingo_lobby_id = new QLabel(tr("Lobby ID"));
        m_bingo_lobby_id = new QLineEdit();

        connection_layout->addRow(bingo_lobby_id, m_bingo_lobby_id);

        QLabel* bingo_lobby_password = new QLabel(tr("Password"));
        m_bingo_lobby_password = new QLineEdit();
        m_bingo_lobby_password->setEchoMode(QLineEdit::Password);

        connection_layout->addRow(bingo_lobby_password, m_bingo_lobby_password);

        m_connect_button = new QPushButton();
        m_connect_button->setText(tr("Connect"));
        connect(m_connect_button, &QPushButton::clicked, this, [this]() {
            QString lobbyID = GetLobbyID();
            QString lobbyPassword = GetLobbyPassword();
            // TODOx: Connect to the bingo lobby
        });

        connection_layout->addWidget(m_connect_button);

        connection_layout->setAlignment(Qt::AlignCenter);
        connection_box->setLayout(connection_layout);

        m_config_layout->addWidget(connection_box);
    }
}

void WiiMixConfigWidget::CreateShuffleLayout() {
    m_config_layout = new QVBoxLayout();

    QGroupBox* shuffle_settings_box = new QGroupBox(tr("Shuffle Settings"));
    QVBoxLayout* shuffle_settings_layout = new QVBoxLayout(); 

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

    shuffle_settings_layout->addWidget(num_switches_label);
    shuffle_settings_layout->addLayout(num_switches_layout);

    QLabel* min_switch_time_label = new QLabel(tr("Min Time Between Shuffles: 15"));
    m_min_time_between_switch = new QSlider(Qt::Horizontal);
    m_min_time_between_switch->setRange(DEFAULT_MIN_SWITCH_TIME, DEFAULT_MAX_SWITCH_TIME);
    m_min_time_between_switch->setValue(Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH));
    QLabel* max_switch_time_label = new QLabel(tr("Max Time Between Shuffles: 60"));
    m_max_time_between_switch = new QSlider(Qt::Horizontal);
    m_max_time_between_switch->setRange(DEFAULT_MIN_SWITCH_TIME, DEFAULT_MAX_SWITCH_TIME);
    m_max_time_between_switch->setValue(Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH));
    shuffle_settings_layout->addWidget(min_switch_time_label);
    shuffle_settings_layout->addWidget(m_min_time_between_switch);
    shuffle_settings_layout->addWidget(max_switch_time_label);
    shuffle_settings_layout->addWidget(m_max_time_between_switch);
    
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

    shuffle_settings_box->setLayout(shuffle_settings_layout);
    m_config_layout->addWidget(shuffle_settings_box);
}

void WiiMixConfigWidget::CreateRogueLayout() {
    m_config_layout = new QVBoxLayout();
}

void WiiMixConfigWidget::CreateCommonLayout() {
    // Add common configuration options
    QGroupBox* common_settings_box = new QGroupBox(tr("Common Settings"));
    QVBoxLayout* common_settings_layout = new QVBoxLayout();

    QLabel* difficulty_label = new QLabel(tr("Objective Difficulty:"));
    m_difficulty = new QComboBox();
    for (int i = 0; i < static_cast<int>(WiiMixEnums::Difficulty::END); i++) {
        m_difficulty->addItem(WiiMixSettings::DifficultyToString(static_cast<WiiMixEnums::Difficulty>(i)));
    }
    WiiMixEnums::Difficulty difficulty = Config::Get(Config::WIIMIX_DIFFICULTY);
    m_difficulty->setCurrentIndex(static_cast<int>(difficulty));
    common_settings_layout->addWidget(difficulty_label);
    common_settings_layout->addWidget(m_difficulty);

    // Time (maybe I make this an estimate at the bottom rather than a parameter)
    // QLabel* time_label = new QLabel(tr("Time:"));
    // m_time = new QLabel();
    // m_config_layout->addWidget(time_label);
    // m_config_layout->addWidget(m_time);

    QLabel* save_state_label = new QLabel(tr("Save State Bank:"));
    m_save_state_bank = new QComboBox();
    for (int i = 0; i < static_cast<int>(WiiMixEnums::SaveStateBank::END); i++) {
        m_save_state_bank->addItem(WiiMixSettings::SaveStateBankToString(static_cast<WiiMixEnums::SaveStateBank>(i)));
    }
    WiiMixEnums::SaveStateBank bank = Config::Get(Config::WIIMIX_SAVE_STATE_BANK);
    m_save_state_bank->setCurrentIndex(static_cast<int>(bank));
    common_settings_layout->addWidget(save_state_label);
    common_settings_layout->addWidget(m_save_state_bank);

    common_settings_box->setLayout(common_settings_layout);

    m_config_layout->addWidget(common_settings_box);
}

void WiiMixConfigWidget::CreateLayout(WiiMixEnums::Mode mode) {
    // Add mode-specific configuration options
    if (mode == WiiMixEnums::Mode::BINGO) {
        CreateBingoLayout();
    }
    else if (mode == WiiMixEnums::Mode::SHUFFLE) {
        CreateShuffleLayout();
    }
    else if (mode == WiiMixEnums::Mode::ROGUE) {
        CreateRogueLayout();
    }
    else {
        return;
    }
    
    CreateCommonLayout();

    setLayout(m_config_layout);
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

WiiMixEnums::BingoType WiiMixConfigWidget::GetBingoType() const {
    if (m_bingo_button->isChecked()) {
        return WiiMixEnums::BingoType::BINGO;
    }
    else if (m_lockout_button->isChecked()) {
        return WiiMixEnums::BingoType::LOCKOUT;
    }
    else if (m_time_attack_button->isChecked()) {
        return WiiMixEnums::BingoType::TIME_ATTACK;
    }
    else {
        return WiiMixEnums::BingoType::END;
    }
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

QString WiiMixConfigWidget::GetLobbyID() const {
    return m_bingo_lobby_id->text();
}

QString WiiMixConfigWidget::GetLobbyPassword() const {
    return m_bingo_lobby_password->text();
}

bool WiiMixConfigWidget::GetTeamsEnabled() const {
    return m_teams_enabled->isChecked();
}

std::array<bool, MAX_PLAYERS> WiiMixConfigWidget::GetTeamSelectors() const {
    std::array<bool, MAX_PLAYERS> team_selectors;
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        team_selectors[i] = m_team_selectors.at(i)->isChecked();
    }
    return team_selectors;
}