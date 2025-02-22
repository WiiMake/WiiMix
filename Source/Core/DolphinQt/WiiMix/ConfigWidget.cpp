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
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QMessageBox>
#include <QUuid>
#include <QString>
#include <QMap>
#include <QPair>

#include "Core/Config/MainSettings.h"

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/ConfigWidget.h"
#include "DolphinQt/WiiMix/BingoSettings.h"
#include "DolphinQt/WiiMix/CommonSettings.h"
#include "DolphinQt/WiiMix/GlobalSettings.h"
#include "DolphinQt/MainWindow.h"
#include <assert.h>

WiiMixConfigWidget::WiiMixConfigWidget(QWidget* parent) : QDialog(parent) {
    // Initialize m_players
    for (int i = 0; i < MAX_PLAYERS; i++) {
        m_players.insert(static_cast<WiiMixEnums::Player>(i), QPair<WiiMixEnums::Color, QString>(WiiMixEnums::PlayerToColor(static_cast<WiiMixEnums::Player>(i)), QStringLiteral("")));
    }
    m_player_name = QStringLiteral("");
    m_player_num = 0;
}

void WiiMixConfigWidget::SetRogueSeed(QString seed) {
    m_rogue_seed->setText(seed);
}

void WiiMixConfigWidget::SetRogueLength(QString length) const {
    m_rogue_length->setCurrentText(length);
}

void WiiMixConfigWidget::DeleteLayout() {
    QLayoutItem* item;
    while ((item = m_config_layout->takeAt(0)) != nullptr)
    {
        delete item->widget(); // Delete the widgets in the layout
        delete item;           // Delete the layout item itself
    }
    delete m_config_layout;
    // Delete the menu bar
    delete m_menu_bar;
}

void WiiMixConfigWidget::DisplayClientError(QString error) {
    QMessageBox::critical(this, QStringLiteral("Error"), error);
}

void WiiMixConfigWidget::OnSettingsChanged(WiiMixBingoSettings *settings) {
    qDebug() << "Update GUI";
    // Set players
    m_players = settings->GetPlayers();
    // Set player num
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (m_players[static_cast<WiiMixEnums::Player>(i)].second == m_player_name) {
            m_player_num = i;
            WiiMixBingoSettings::instance()->SetPlayerNum(static_cast<WiiMixEnums::Player>(m_player_num));
            break;
        }
    }
    
    // Delete the connect screen (if it exists)
    DeleteLayout();
    // WiiMixBingoSettings::instance() = settings;
    CreateBingoLayout(QStringLiteral("Host"));
    CreateCommonLayout();
    setLayout(m_config_layout);

    return;
}

void WiiMixConfigWidget::CreateBingoLayout(QString menu) {
    qDebug() << "Bingo Settings (Config Widget):";
    qDebug() << "Difficulty:" << static_cast<int>(WiiMixBingoSettings::instance()->GetDifficulty());
    qDebug() << "Save State Bank:" << static_cast<int>(WiiMixBingoSettings::instance()->GetSaveStateBank());
    qDebug() << "Bingo Type:" << static_cast<int>(WiiMixBingoSettings::instance()->GetBingoType());
    qDebug() << "Card Size:" << static_cast<int>(WiiMixBingoSettings::instance()->GetCardSize());
    qDebug() << "Teams Enabled:" << WiiMixBingoSettings::instance()->GetTeams();
    qDebug() << "Lobby ID:" << WiiMixBingoSettings::instance()->GetLobbyID();
    qDebug() << "Lobby Password:" << WiiMixBingoSettings::instance()->GetLobbyPassword();
    qDebug() << "Players:";
    for (const auto& player : WiiMixBingoSettings::instance()->GetPlayers()) {
        qDebug() << "  Player" << static_cast<int>(player.first) << ":" << player.second;
    }
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
        DeleteLayout();
        CreateBingoLayout(QStringLiteral("Host"));
        CreateCommonLayout();
        // Set the layout
        setLayout(m_config_layout);
    });

    connect(connect_action, &QAction::triggered, this, [this]() {
        // Delete the layout
        DeleteLayout();
        CreateBingoLayout(QStringLiteral("Connect"));
        // Set the layout
        setLayout(m_config_layout);
    });

    // Connect the local onUpdateBingoConfig event to the settings changed slot
    // TODOx: resolve when settings are changed (server vs client)
    connect(this, &WiiMixConfigWidget::onUpdateBingoConfig, this, &WiiMixConfigWidget::OnSettingsChanged);

    menu_layout->addStretch();
    menu_layout->addWidget(m_menu_bar);
    menu_layout->addStretch();

    m_config_layout->addLayout(menu_layout);

    if (menu == QStringLiteral("Host")) {
        // Create host settings
        
        // Connection Settings
        QGroupBox* connection_box = new QGroupBox(tr("Connection Settings"));
        QFormLayout* connection_layout = new QFormLayout();

        QLabel*bingo_lobby_id = new QLabel(tr("Lobby ID"));
        m_bingo_lobby_id = new QLineEdit();
        m_bingo_lobby_id->setReadOnly(true);
        // if (WiiMixBingoSettings::instance()->GetLobbyID() != QStringLiteral("")) {
        m_bingo_lobby_id->setText(WiiMixBingoSettings::instance()->GetLobbyID());
        // }

        connection_layout->addRow(bingo_lobby_id, m_bingo_lobby_id);

        QLabel*player_name = new QLabel(tr("Player Name"));
        m_bingo_player_name = new QLineEdit();
        if (m_player_name != QStringLiteral("")) {
            m_bingo_player_name->setText(m_player_name);
        }

        connection_layout->addRow(player_name, m_bingo_player_name);

        QLabel*bingo_lobby_password = new QLabel(tr("Password"));
        m_bingo_lobby_password = new QLineEdit();
        m_bingo_lobby_password->setMaxLength(MAX_LOBBY_PASSWORD_LENGTH);
        m_bingo_lobby_password->setEchoMode(QLineEdit::Password);
        m_bingo_lobby_password->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral("[A-Za-z0-9]+"))));
        // if (WiiMixBingoSettings::instance()->GetLobbyPassword() != QStringLiteral("")) {
            m_bingo_lobby_password->setText(WiiMixBingoSettings::instance()->GetLobbyPassword());
        // }

        connection_layout->addRow(bingo_lobby_password, m_bingo_lobby_password);

        m_connect_button = new QPushButton();
        m_connect_button->setText(tr("Host"));
        if (m_player_num != 0) {
            m_connect_button->setText(tr("Update"));
        }

        #if defined(WIIMIX_PORT) && defined(WIIMIX_IP)
            connect(m_connect_button, &QPushButton::clicked, this, [this]() {
                // Set the bingo lobby ID
                if (m_bingo_lobby_id->text().isEmpty()) {
                    qDebug() << "Creating lobby";
                    QString id = GenerateLobbyID();
                    m_bingo_lobby_id->setText(id);
                    // Update the first player (host)
                    WiiMixEnums::Player player = static_cast<WiiMixEnums::Player>(m_player_num);
                    m_players[player] = QPair<WiiMixEnums::Color, QString>(WiiMixEnums::PlayerToColor(player), GetPlayerName());
                    // Create a socket for communication with the server
                    // qDebug() << m_wiimix_client;
                    if (!WiiMixClient::instance()->IsConnected()) {
                        qDebug() << "Creating bingo client";
                        
                        // Upon receiving any data from the server (which should only be updated settings),
                        // the client should update its settings
                        connect(WiiMixClient::instance(), &WiiMixClient::onUpdateBingoConfig, this, &WiiMixConfigWidget::OnSettingsChanged);
                        connect(WiiMixClient::instance(), &WiiMixClient::onError, this, &WiiMixConfigWidget::DisplayClientError);
                        WiiMixClient::instance()->ConnectToServer();
                    }
                    m_player_name = GetPlayerName();
                    
                    // Creates a lobby with the given settings
                    QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::CREATE_BINGO_LOBBY)->ToJson().object();
                    obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
                    WiiMixClient::instance()->SendData(obj, WiiMixEnums::Action::CREATE_BINGO_LOBBY);
                }
                else {
                    qDebug() << "Updating lobby settings";
                    // If the lobby ID is already set, assume password or player name was updated and update lobby settings
                    m_player_name = GetPlayerName();
                    // Check if the player name is unique (i.e. not already in the lobby)
                    for (int i = 0; i < MAX_PLAYERS; i++) {
                        if (m_players[static_cast<WiiMixEnums::Player>(i)].second == m_player_name) {
                            if (i != m_player_num) {
                                QMessageBox::critical(this, tr("Error"), tr("Player name must be unique"));
                                return;
                            }
                        }
                    }
                    WiiMixEnums::Player player = static_cast<WiiMixEnums::Player>(m_player_num);
                    m_players[player] = QPair<WiiMixEnums::Color, QString>(WiiMixEnums::PlayerToColor(player), m_player_name);
                    QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::UPDATE_BINGO_LOBBY)->ToJson().object();
                    obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
                    WiiMixClient::instance()->SendData(GetBingoSettings(WiiMixEnums::Action::UPDATE_BINGO_LOBBY)->ToJson().object(), WiiMixEnums::Action::UPDATE_BINGO_LOBBY);
                }
                // try {
                //     // You should ONLY be able to access the server through sending requests...
                // }
                // catch (const std::runtime_error& e) {
                //     // If the lobby creation fails, display an error message
                //     QMessageBox::critical(this, tr("Error"), tr("Failed to create lobby: %1").arg(e.what()));
                // }
            });
        #else
            qWarning() << "No server settings found; cannot run the server";
            qDebug() << "1";
        #endif

        connection_layout->addWidget(m_connect_button);

        connection_box->setLayout(connection_layout);
        m_config_layout->addWidget(connection_box);

        QVBoxLayout* bingo_settings_layout = new QVBoxLayout();
        QGroupBox* bingo_box = new QGroupBox(tr("Bingo Settings"));

        // Lobby settings
        QVBoxLayout* lobby_layout = new QVBoxLayout();

        QHBoxLayout* teams_enabled_layout = new QHBoxLayout();
        m_teams_enabled = new QCheckBox();
        m_teams_enabled->setChecked(WiiMixBingoSettings::instance()->GetTeams());
        m_team_selectors = QList<QCheckBox*>();

        QLabel*teams_enabled_label = new QLabel(tr("Teams Enabled:"));
        teams_enabled_layout->addWidget(teams_enabled_label);
        teams_enabled_layout->addWidget(m_teams_enabled);
        lobby_layout->addLayout(teams_enabled_layout);
        for (int i = 0; i < MAX_PLAYERS; i++) {
            QHBoxLayout* player_layout = new QHBoxLayout();
            player_layout->setAlignment(Qt::AlignLeft);
            QCheckBox* team_selector = new QCheckBox();
            team_selector->setStyleSheet(QStringLiteral(".QCheckBox { background-color: %1; }").arg(QString::fromStdString(ColorToHex(m_players[static_cast<WiiMixEnums::Player>(i)].first))));
            connect(team_selector, &QCheckBox::clicked, this, [this, team_selector](bool checked) {
                int index = m_team_selectors.indexOf(team_selector);
                SetTeamSelectors(index);
            });
            QLabel* player_widget = new QLabel(tr("Player ") + QString::number(i + 1) + tr(": ") + m_players[static_cast<WiiMixEnums::Player>(i)].second);
            m_team_selectors.append(team_selector);

            player_layout->addWidget(m_team_selectors.at(i));
            player_layout->addWidget(player_widget);
            lobby_layout->addLayout(player_layout);
        }
        QGroupBox* bingo_lobby = new QGroupBox(tr("Bingo Lobby"));
        bingo_lobby->setLayout(lobby_layout);

        m_config_layout->addWidget(bingo_lobby);

        connect(m_teams_enabled, &QCheckBox::clicked, this, [this](bool checked) {
            SetTeams(checked);
        });

        WiiMixEnums::BingoType bingo_type = Config::Get(Config::WIIMIX_BINGO_TYPE);
        bingo_type = WiiMixBingoSettings::instance()->GetBingoType();
        m_bingo_button = new QRadioButton(tr("Bingo"));
        if (bingo_type == WiiMixEnums::BingoType::BINGO) {
            m_bingo_button->setChecked(true);
        }
        else {
            m_bingo_button->setChecked(false);
        }

        connect(m_bingo_button, &QRadioButton::clicked, this, [this](bool checked) {
            SetBingoType(WiiMixEnums::BingoType::BINGO);
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

        connect(m_lockout_button, &QRadioButton::clicked, this, [this](bool checked) {
            SetBingoType(WiiMixEnums::BingoType::LOCKOUT);
        });

        // m_time_attack_button = new QRadioButton(tr("Time Attack"));
        // if (bingo_type == WiiMixEnums::BingoType::TIME_ATTACK) {
        //     m_time_attack_button->setChecked(true);
        // }
        // else {
        //     m_time_attack_button->setChecked(false);
        // }

        // connect(m_time_attack_button, &QRadioButton::clicked, this, [this](bool checked) {
        //     SetBingoType(WiiMixEnums::BingoType::TIME_ATTACK);
        // });

        // bingo_settings_layout->addWidget(m_time_attack_button);
        
        QLabel* card_size_label = new QLabel(tr("Card Size:"));
        int card_size = Config::Get(Config::WIIMIX_CARD_SIZE);
        card_size = WiiMixBingoSettings::instance()->GetCardSize();
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
            SetCardSize(index);
        });

        bingo_settings_layout->addWidget(card_size_label);
        bingo_settings_layout->addWidget(m_card_size);

        // Bingo seeds just encode achievement information
        m_bingo_seed = new QLineEdit();
        // There are 3 different decoded lengths:
        // 1 + (6 * 9)
        // 1 + (6 * 16)
        // 1 + (6 * 25)
        // The bingo seed does not check whether all players have the games enabled, 
        // as they will require having the games enabled when connecting
        // The games list used will be finalized during connection
        // m_bingo_seed->setMaxLength(16);
        QRegularExpression seed_regex(QStringLiteral("[A-Za-z0-9]{16}"));
        QRegularExpressionValidator* seed_validator = new QRegularExpressionValidator(seed_regex, this);
        m_bingo_seed->setValidator(seed_validator);
        bingo_settings_layout->addWidget(new QLabel(tr("Bingo Seed:")));
        bingo_settings_layout->addWidget(m_bingo_seed);

        bingo_box->setLayout(bingo_settings_layout);
        m_config_layout->addWidget(bingo_box);
    }
    else if (menu == QStringLiteral("Connect")) {
        // Create connection settings (not host)
        QGroupBox* connection_box = new QGroupBox(tr("Connection Settings"));
        QFormLayout* connection_layout = new QFormLayout();

        QLabel* bingo_lobby_id = new QLabel(tr("Lobby ID"));
        m_bingo_lobby_id = new QLineEdit();

        connection_layout->addRow(bingo_lobby_id, m_bingo_lobby_id);

        QLabel* player_name = new QLabel(tr("Player Name"));
        m_bingo_player_name = new QLineEdit();

        connection_layout->addRow(player_name, m_bingo_player_name);

        QLabel* bingo_lobby_password = new QLabel(tr("Password"));
        m_bingo_lobby_password = new QLineEdit();
        m_bingo_lobby_password->setEchoMode(QLineEdit::Password);

        connection_layout->addRow(bingo_lobby_password, m_bingo_lobby_password);

        m_connect_button = new QPushButton();
        m_connect_button->setText(tr("Connect"));
        #if WIIMIX_PORT && defined(WIIMIX_IP)
            connect(m_connect_button, &QPushButton::clicked, this, [this]() {
                // Attempt to connect
                if (!WiiMixClient::instance()->IsConnected()) {
                    connect(WiiMixClient::instance(), &WiiMixClient::onUpdateBingoConfig, this, &WiiMixConfigWidget::OnSettingsChanged);
                    connect(WiiMixClient::instance(), &WiiMixClient::onError, this, &WiiMixConfigWidget::DisplayClientError);
                    // Make connection AFTER setting up the signals in case of error
                    WiiMixClient::instance()->ConnectToServer();
                }
                m_player_name = GetPlayerName();
                // Just use player one as a placeholder; the server will assign the player index
                m_players.insert(WiiMixEnums::Player::ONE, QPair<WiiMixEnums::Color, QString>(WiiMixEnums::PlayerToColor(WiiMixEnums::Player::ONE), m_player_name));
                // Providing the connect action should prevent you from sending settings that you don't have
                QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::CONNECT_TO_BINGO_LOBBY)->ToJson().object();
                obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
                WiiMixClient::instance()->SendData(obj, WiiMixEnums::Action::CONNECT_TO_BINGO_LOBBY);
            });
        #else
            qWarning() << "No server settings found; cannot run the server";
            qDebug() << "2";
        #endif

        connection_layout->addWidget(m_connect_button);

        connection_layout->setAlignment(Qt::AlignCenter);
        connection_box->setLayout(connection_layout);

        m_config_layout->addWidget(connection_box);
    }
    m_config_layout->setAlignment(Qt::AlignCenter);
}

void WiiMixConfigWidget::CreateShuffleLayout() {
    m_config_layout = new QVBoxLayout();

    QGroupBox* shuffle_settings_box = new QGroupBox(tr("Shuffle Settings"));
    QVBoxLayout* shuffle_settings_layout = new QVBoxLayout(); 

    int num_switches = Config::Get(Config::WIIMIX_NUMBER_OF_SWITCHES);
    QHBoxLayout* num_switches_layout = new QHBoxLayout();

    QLabel*num_switches_label = new QLabel(tr("Number of Objectives:"));
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
        SetNumSwitches(text.toInt());
    });

    // Connect endless_mode to disable/enable number of switches
    connect(m_endless_mode, &QCheckBox::toggled, this, [this](bool checked) {
        SetEndless(checked);
    });

    // Connect slider value change to update the label
    connect(m_min_time_between_switch, &QSlider::valueChanged, this, [this](int value) {
        SetMinTimeBetweenSwitch(value);
    });

    connect(m_max_time_between_switch, &QSlider::valueChanged, this, [this](int value) {
        SetMaxTimeBetweenSwitch(value);
    });

    shuffle_settings_box->setLayout(shuffle_settings_layout);
    m_config_layout->addWidget(shuffle_settings_box);
}

void WiiMixConfigWidget::CreateRogueLayout() {
    m_config_layout = new QVBoxLayout();
    m_config_layout->addWidget(new QLabel(tr("Length")));
    m_rogue_length = new QComboBox();
    m_rogue_length->addItem(WiiMixRogueSettings::LengthToString(WiiMixEnums::RogueLength::SHORT));
    m_rogue_length->addItem(WiiMixRogueSettings::LengthToString(WiiMixEnums::RogueLength::MEDIUM));
    m_rogue_length->addItem(WiiMixRogueSettings::LengthToString(WiiMixEnums::RogueLength::MARATHON));
    m_config_layout->addWidget(m_rogue_length);
    connect(m_rogue_length, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        SetRogueLength(m_rogue_length->currentText());
    });

    m_config_layout->addWidget(new QLabel(tr("Seed")));
    m_rogue_seed = new QLineEdit();
    m_rogue_seed->setPlaceholderText(tr("random"));
    m_rogue_seed->setMaxLength(16);
    QRegularExpression regex(QStringLiteral("[A-Za-z0-9]{16}"));
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(regex, this);
    m_rogue_seed->setValidator(validator);
    m_config_layout->addWidget(m_rogue_seed);

    connect(m_rogue_seed, &QLineEdit::textChanged, this, [this](const QString& text) {
        SetRogueSeed(text);
    });
}

void WiiMixConfigWidget::CreateCommonLayout() {
    // Add common configuration options
    QGroupBox* common_settings_box = new QGroupBox(tr("Common Settings"));
    QVBoxLayout* common_settings_layout = new QVBoxLayout();

    QLabel* difficulty_label = new QLabel(tr("Objective Difficulty:"));
    m_difficulty = new QComboBox();
    for (int i = 0; i < static_cast<int>(WiiMixEnums::Difficulty::END); i++) {
        m_difficulty->addItem(WiiMixCommonSettings::DifficultyToString(static_cast<WiiMixEnums::Difficulty>(i)));
    }
    WiiMixEnums::Mode mode = Config::Get(Config::WIIMIX_MODE);
    WiiMixEnums::Difficulty difficulty;
    switch (mode)
    {
        case WiiMixEnums::Mode::BINGO:
            difficulty = WiiMixBingoSettings::instance()->GetDifficulty();
            break;
        case WiiMixEnums::Mode::SHUFFLE:
            difficulty = WiiMixShuffleSettings::instance()->GetDifficulty();
            break;
        case WiiMixEnums::Mode::ROGUE:
            difficulty = WiiMixRogueSettings::instance()->GetDifficulty();
            break;
        default:
            qDebug() << "Difficulty is uninitialized";
            break;
    }
    m_difficulty->setCurrentIndex(static_cast<int>(difficulty));
    connect(m_difficulty, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        SetDifficulty(m_difficulty->currentText());
    });
    common_settings_layout->addWidget(difficulty_label);
    common_settings_layout->addWidget(m_difficulty);

    // Time (maybe I make this an estimate at the bottom rather than a parameter)
    // QLabel*time_label = new QLabel(tr("Time:"));
    // m_time = new QLabel();
    // m_config_layout->addWidget(time_label);
    // m_config_layout->addWidget(m_time);

    QLabel* save_state_label = new QLabel(tr("Save State Bank:"));
    m_save_state_bank = new QComboBox();
    for (int i = 0; i < static_cast<int>(WiiMixEnums::SaveStateBank::END); i++) {
        m_save_state_bank->addItem(WiiMixCommonSettings::SaveStateBankToString(static_cast<WiiMixEnums::SaveStateBank>(i)));
    }
    WiiMixEnums::SaveStateBank bank;
    switch (mode)
    {
        case WiiMixEnums::Mode::BINGO:
            bank = WiiMixBingoSettings::instance()->GetSaveStateBank();
            break;
        case WiiMixEnums::Mode::SHUFFLE:
            bank = WiiMixShuffleSettings::instance()->GetSaveStateBank();
            break;
        case WiiMixEnums::Mode::ROGUE:
            bank = WiiMixRogueSettings::instance()->GetSaveStateBank();
            break;
        default:
            qDebug() << "bank is uninitialized";
            break;
    }
    m_save_state_bank->setCurrentIndex(static_cast<int>(bank));
    connect(m_save_state_bank, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        SetSaveStateBank(m_save_state_bank->currentText());
    });

    common_settings_layout->addWidget(save_state_label);
    common_settings_layout->addWidget(m_save_state_bank);

    QLabel* objective_types_label = new QLabel(tr("Objective Types:"));
    m_objective_types = new QListWidget();
    m_objective_types->setSelectionMode(QAbstractItemView::MultiSelection);
    for (int i = 0; i < static_cast<int>(WiiMixEnums::ObjectiveType::END); i++) {
        m_objective_types->addItem(QString::fromStdString(WiiMixEnums::ObjectiveTypeToString(static_cast<WiiMixEnums::ObjectiveType>(i))));
    }

    common_settings_layout->addWidget(objective_types_label);
    common_settings_layout->addWidget(m_objective_types);

    QLabel* game_genres_label = new QLabel(tr("Game Genres:"));
    m_game_genres = new QListWidget();
    m_game_genres->setSelectionMode(QAbstractItemView::MultiSelection);
    for (int i = 0; i < static_cast<int>(WiiMixEnums::ObjectiveType::END); i++) {
        m_game_genres->addItem(QString::fromStdString(WiiMixEnums::GameGenreToString(static_cast<WiiMixEnums::GameGenre>(i))));
    }

    common_settings_layout->addWidget(game_genres_label);
    common_settings_layout->addWidget(m_game_genres);

    common_settings_box->setLayout(common_settings_layout);

    m_config_layout->addWidget(common_settings_box);
}

// Need to also implement the corresponding parsing, checks, and probably associated error messages
// with the QValidators

// @xanmankey
void SetRogueSeed() {
    return;
}

// @xanmankey
void SetBingoSeed() {

    return;
}

// @xanmankey
void SetLobbyID() {
    return;
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
    // else if (m_time_attack_button->isChecked()) {
    //     return WiiMixEnums::BingoType::TIME_ATTACK;
    // }
    else {
        return WiiMixEnums::BingoType::END;
    }
}

QString WiiMixConfigWidget::GetCardSize() const {
    return m_card_size->currentText();
}

int WiiMixConfigWidget::GetMinTimeBetweenSwitch() const
{
    return m_min_time_between_switch->value();
}

int WiiMixConfigWidget::GetMaxTimeBetweenSwitch() const
{
    return m_max_time_between_switch->value();
}

QString WiiMixConfigWidget::GenerateLobbyID() const {
     // Generate a random string of length LOBBY_ID_LENGTH
    // const QString possible_characters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    // const int possible_characters_length = possible_characters.length();

    // QString random_string;
    // for (int i = 0; i < LOBBY_ID_LENGTH; ++i) {
    //     int index = QRandomGenerator::global()->generate() % possible_characters_length;
    //     random_string.append(possible_characters.at(index));
    // }
    // return random_string;

    // Turns out Qt has a built-in function for this
    return QUuid::createUuid().toString(QUuid::Id128); // Generates a 32 byte globally unique string 
}

// @xanmankey
QRegularExpression BingoSeedValidator() {
  return QRegularExpression();
}

// @xanmankey
QRegularExpression RogueSeedValidator() {
  return QRegularExpression();
}

// @xanmankey: for connecting to and hosting lobbies (game validation)
QRegularExpression BingoLobbyIDValidator() {
    // A validator that checks if you have all of the games in the lobby checked
    // Prevents people from connecting to lobbies without having all the games
  return QRegularExpression();
}

QString WiiMixConfigWidget::GetPlayerName() const {
    return m_bingo_player_name->text();
}

QString WiiMixConfigWidget::GetLobbyID() const {
    return m_bingo_lobby_id->text();
}

QString WiiMixConfigWidget::GetLobbyPassword() const {
    return m_bingo_lobby_password->text();
}

bool WiiMixConfigWidget::GetTeams() const {
    return m_teams_enabled->isChecked();
}

QMap<WiiMixEnums::Player, QPair<WiiMixEnums::Color, QString>> WiiMixConfigWidget::GetPlayers() const {
    return m_players;
}

WiiMixEnums::RogueLength WiiMixConfigWidget::GetRogueLength() const {
    return static_cast<WiiMixEnums::RogueLength>(m_rogue_length->currentIndex());
}

QString WiiMixConfigWidget::GetRogueSeed() const{
    return m_rogue_seed->text();
}

std::array<bool, MAX_PLAYERS> WiiMixConfigWidget::GetTeamSelectors() const {
    std::array<bool, MAX_PLAYERS> team_selectors;
    for (int i = 0; i < MAX_PLAYERS; ++i) {
        team_selectors[i] = m_team_selectors.at(i)->isChecked();
    }
    return team_selectors;
}

// Need to also implement the corresponding parsing, checks, and probably associated error messages
// with the QValidators

void WiiMixConfigWidget::SetBingoSeed(QString seed) {
    m_bingo_seed->setText(seed);
}

void WiiMixConfigWidget::SetLobbyID(QString id) {
    m_bingo_lobby_id->setText(id);
}

void WiiMixConfigWidget::SetDifficulty(QString difficulty) {
    int index = m_difficulty->findText(difficulty);
    if (index != -1) {
        m_difficulty->setCurrentIndex(index);
    }
    if (WiiMixClient::instance()->IsConnected()) {
        QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::UPDATE_BINGO_LOBBY)->ToJson().object();
        obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
        WiiMixClient::instance()->SendData(obj, WiiMixEnums::Action::UPDATE_BINGO_LOBBY);
    }
    // else {
    //     emit onUpdateBingoConfig(GetBingoSettings());
    // }
}

void WiiMixConfigWidget::SetSaveStateBank(QString bank) {
    int index = m_save_state_bank->findText(bank);
    if (index != -1) {
        m_save_state_bank->setCurrentIndex(index);
    }
    if (WiiMixClient::instance()->IsConnected()) {
        QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::UPDATE_BINGO_LOBBY)->ToJson().object();
        obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
        WiiMixClient::instance()->SendData(obj, WiiMixEnums::Action::UPDATE_BINGO_LOBBY);
    }
   // else {
   //     emit onUpdateBingoConfig(GetBingoSettings());
   // }
}

void WiiMixConfigWidget::SetNumSwitches(int num_switches) {
    Config::Set(Config::LayerType::Base, Config::WIIMIX_NUMBER_OF_SWITCHES, num_switches);
    m_num_switches->setText(QString::number(num_switches));
}

void WiiMixConfigWidget::SetEndless(bool endless) {
    m_endless_mode->setChecked(endless);
    m_num_switches->setDisabled(endless);
    Config::Set(Config::LayerType::Base, Config::WIIMIX_IS_ENDLESS, endless);
}

void WiiMixConfigWidget::SetBingoType(WiiMixEnums::BingoType type) {
    switch (type) {
        case WiiMixEnums::BingoType::BINGO:
            m_bingo_button->setChecked(true);
            m_lockout_button->setChecked(false);
            // m_time_attack_button->setChecked(false);
            break;
        case WiiMixEnums::BingoType::LOCKOUT:
            m_bingo_button->setChecked(false);
            m_lockout_button->setChecked(true);
            // m_time_attack_button->setChecked(false);
            break;
        // case WiiMixEnums::BingoType::TIME_ATTACK:
        //     m_bingo_button->setChecked(false);
        //     m_lockout_button->setChecked(false);
        //     m_time_attack_button->setChecked(true);
        //     break;
        default:
            break;
    }
    if (WiiMixClient::instance()->IsConnected()) {
        QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::UPDATE_BINGO_LOBBY)->ToJson().object();
        obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
        WiiMixClient::instance()->SendData(obj, WiiMixEnums::Action::UPDATE_BINGO_LOBBY);
    }
    else {
        qDebug() << "Emitting local settings changed";
        emit onUpdateBingoConfig(GetBingoSettings());
    }
}

void WiiMixConfigWidget::SetCardSize(int index) {
    if (index != -1) {
        m_card_size->setCurrentIndex(index);
    }
    if (WiiMixClient::instance()->IsConnected()) {
        QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::UPDATE_BINGO_LOBBY)->ToJson().object();
        obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
        WiiMixClient::instance()->SendData(obj, WiiMixEnums::Action::UPDATE_BINGO_LOBBY);
    }
   // else {
   //     emit onUpdateBingoConfig(GetBingoSettings());
   // }
}

// WARNING: this combined with rerendering the entire window & network requests might be too costly
void WiiMixConfigWidget::SetMinTimeBetweenSwitch(int min_time) {
    m_min_time_between_switch->setValue(min_time);
    m_min_switch_time_label->setText(QStringLiteral("Min Time Between Shuffles: ") + QString::number(min_time));
    Config::Set(Config::LayerType::Base, Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH, min_time);
}

// WARNING: this combined with rerendering the entire window & network requests might be too costly
void WiiMixConfigWidget::SetMaxTimeBetweenSwitch(int max_time) {
    m_max_time_between_switch->setValue(max_time);
    m_max_switch_time_label->setText(QStringLiteral("Max Time Between Shuffles: ") + QString::number(max_time));
    Config::Set(Config::LayerType::Base, Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH, max_time);
}

void WiiMixConfigWidget::SetLobbyPassword(QString password) {
    m_bingo_lobby_password->setText(password);
    if (WiiMixClient::instance()->IsConnected()) {
        QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::UPDATE_BINGO_LOBBY)->ToJson().object();
        obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
        WiiMixClient::instance()->SendData(obj, WiiMixEnums::Action::UPDATE_BINGO_LOBBY);
    }
   // else {
   //     emit onUpdateBingoConfig(GetBingoSettings());
   // }
}


void WiiMixConfigWidget::SetTeams(bool enabled) {
    m_teams_enabled->setChecked(enabled);
    Config::Set(Config::LayerType::Base, Config::WIIMIX_TEAMS, enabled);
    if (enabled) {
        // If teams is toggled, sets the players to be on teams
        int mid = m_team_selectors.size() / 2;
        for (int i = 0; i < mid; i++) {
            m_team_selectors.at(i)->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(WII_MIX_RED)));
            m_players[static_cast<WiiMixEnums::Player>(i)].first = WiiMixEnums::Color::RED;
        }
        for (int i = mid; i < m_team_selectors.size(); i++) {
            m_team_selectors.at(i)->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(WII_MIX_BLUE)));
            m_players[static_cast<WiiMixEnums::Player>(i)].first = WiiMixEnums::Color::BLUE;
        }
    }
    else {
        // If teams is not toggled, give each player it's individual color again
        for (int i = 0; i < MAX_PLAYERS; i++) {
            // Enable or disable team indicators based on the checkbox state
            m_team_selectors.at(i)->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(ColorToHex(static_cast<WiiMixEnums::Color>(i)))));
            m_players[static_cast<WiiMixEnums::Player>(i)].first = static_cast<WiiMixEnums::Color>(i);
        }
    }
    if (WiiMixClient::instance()->IsConnected()) {
        QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::UPDATE_BINGO_LOBBY)->ToJson().object();
        obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
        WiiMixClient::instance()->SendData(obj, WiiMixEnums::Action::UPDATE_BINGO_LOBBY);
    }
   // else {
   //     emit onUpdateBingoConfig(GetBingoSettings());
   // }
}

// Updates the color checkboxes based on whether teams is enabled or not
void WiiMixConfigWidget::SetTeamSelectors(int index) {
    // Change the color to indicate team selection
    // If teams, select the next color regardless of whether someone has it selected or not
    if (GetTeams()) {
        int color_index = static_cast<int>(m_players.value(static_cast<WiiMixEnums::Player>(index)).first);
        color_index += 1;
        color_index %= static_cast<int>(WiiMixEnums::Color::END); 
        auto& player = m_players[static_cast<WiiMixEnums::Player>(index)];
        player.first = static_cast<WiiMixEnums::Color>(color_index);
        m_team_selectors.at(index)->setStyleSheet(QStringLiteral("QCheckBox { background-color: %1; }").arg(QString::fromStdString(ColorToHex(static_cast<WiiMixEnums::Color>(color_index)))));
    }
    // If not teams, just use the default colors associated with the players
    if (WiiMixClient::instance()->IsConnected()) {
        QJsonObject obj = GetBingoSettings(WiiMixEnums::Action::UPDATE_BINGO_LOBBY)->ToJson().object();
        obj[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::UPDATE_BINGO_CONFIG);
        WiiMixClient::instance()->SendData(obj, WiiMixEnums::Action::UPDATE_BINGO_LOBBY);
    }
   // else {
   //     emit onUpdateBingoConfig(GetBingoSettings());
   // }
}

// void WiiMixConfigWidget::SetTeamSelectorStates(std::array<std::string, MAX_PLAYERS> states) {
//     for (int i = 0; i < MAX_PLAYERS; ++i) {
//         m_team_selector_states[i] = states[i];
//     }
// }

// void WiiMixConfigWidget::SetBingoPlayerNames(QList<std::string> names) {
//     for (int i = 0; i < MAX_PLAYERS; ++i) {
//         m_bingo_player_names[i] = names[i];
//     }
// }

/// Creates a settings instance from the current configuration
/// and sends it to the Bingo server
WiiMixBingoSettings* WiiMixConfigWidget::GetBingoSettings(WiiMixEnums::Action action) {
    // Create a settings instance from the current configuration
    if (action == WiiMixEnums::Action::CONNECT_TO_BINGO_LOBBY) {
        qDebug() << QStringLiteral("Connecting to lobby: %1").arg(GetLobbyID());
        // WiiMixBingoSettings::instance()->SetDifficulty(WiiMixSettings::StringToDifficulty(GetDifficulty()));
        // WiiMixBingoSettings::instance()->SetSaveStateBank(WiiMixSettings::StringToSaveStateBank(GetSaveStateBank()));
        // WiiMixBingoSettings::instance()->SetMode(WiiMixEnums::Mode::BINGO);
        // WiiMixBingoSettings::instance()->SetBingoType(GetBingoType());
        // WiiMixBingoSettings::instance()->SetCardSize(WiiMixSettings::StringToCardSize(GetCardSize()));
        // WiiMixBingoSettings::instance()->SetTeams(GetTeams());
        WiiMixBingoSettings::instance()->SetPlayers(GetPlayers());
        WiiMixBingoSettings::instance()->SetLobbyID(GetLobbyID());
        WiiMixBingoSettings::instance()->SetLobbyPassword(GetLobbyPassword());
    }
    else {
        WiiMixBingoSettings::instance()->SetDifficulty(WiiMixCommonSettings::StringToDifficulty(GetDifficulty()));
        qDebug() << QStringLiteral("Difficulty set to: %1").arg(GetDifficulty());
        
        WiiMixBingoSettings::instance()->SetSaveStateBank(WiiMixCommonSettings::StringToSaveStateBank(GetSaveStateBank()));
        qDebug() << QStringLiteral("SaveStateBank set to: %1").arg(GetSaveStateBank());
        
        WiiMixGlobalSettings::instance()->SetMode(WiiMixEnums::Mode::BINGO);
        qDebug() << QStringLiteral("Mode set to: BINGO");
        
        WiiMixBingoSettings::instance()->SetBingoType(GetBingoType());
        qDebug() << QStringLiteral("BingoType set to: %1").arg(static_cast<int>(GetBingoType()));
        
        WiiMixBingoSettings::instance()->SetCardSize(WiiMixBingoSettings::StringToCardSize(GetCardSize()));
        qDebug() << QStringLiteral("CardSize set to: %1").arg(GetCardSize());
        qDebug() << QStringLiteral("Possible error with StringToCardSize: %1").arg(WiiMixBingoSettings::StringToCardSize(GetCardSize()));
        
        WiiMixBingoSettings::instance()->SetTeams(GetTeams());
        qDebug() << QStringLiteral("Teams set to: %1").arg(GetTeams());
        
        WiiMixBingoSettings::instance()->SetPlayers(GetPlayers());
        qDebug() << QStringLiteral("Players set to: %1").arg(GetPlayers().size());
        
        WiiMixBingoSettings::instance()->SetLobbyID(GetLobbyID());
        qDebug() << QStringLiteral("LobbyID set to: %1").arg(GetLobbyID());
        
        WiiMixBingoSettings::instance()->SetLobbyPassword(GetLobbyPassword());
        qDebug() << QStringLiteral("LobbyPassword set to: %1").arg(GetLobbyPassword());
    }
    return WiiMixBingoSettings::instance();
}

void WiiMixConfigWidget::SetPlayerName(QString name) {
    m_player_name = GetPlayerName();
    m_bingo_player_name->setText(name);
}
