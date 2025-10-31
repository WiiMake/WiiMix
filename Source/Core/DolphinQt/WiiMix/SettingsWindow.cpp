// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/SettingsWindow.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include <QMessageBox>

#include "Core/ConfigLoaders/GameConfigLoader.h"
#include "Core/Config/WiimoteSettings.h"

#include "Common/IniFile.h"
#include "Common/FileUtil.h"

#include "DolphinQt/QtUtils/WrapInScrollArea.h"
#include "DolphinQt/QtUtils/DolphinFileDialog.h"
#include "DolphinQt/Resources.h"
#include "DolphinQt/Settings.h"
#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/GameManager.h"
#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/ConfigWidget.h"
#include "DolphinQt/WiiMix/BingoSettings.h"
#include "DolphinQt/WiiMix/RogueSettings.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"
#include <iostream>
#include <QGraphicsDropShadowEffect>

#include "Core/HW/Wiimote.h"
#include "Core/HW/GCPad.h"
#include "Core/HW/SI/SI_Device.h"
#include "InputCommon/ControllerInterface/ControllerInterface.h"
#include "InputCommon/InputConfig.h"

WiiMixLogoButton* WiiMixSettingsWindow::getWiiMixLogoButton() {
    return m_wii_mix_button;
}

WiiMixSettingsWindow::WiiMixSettingsWindow(QWidget *parent) : QDialog(parent)
{
  setWindowTitle(tr("WiiMix"));
  QPixmap background = (Resources::GetResourceIcon("wiimix_background_top").pixmap(1200,800));
  background.scaled(this->size());
  QPalette palette;
  QBrush* backgroundBrush = new QBrush();
  backgroundBrush->setTexture(background);
  palette.setBrush(QPalette::Window, *backgroundBrush);
  this->setPalette(palette);
  m_modes = new WiiMixModesWidget(this);
  m_load_button_box = new QPushButton();
  m_load_button_box->setMaximumSize(QSize(60,60));
  m_load_button_box->setMinimumSize(QSize(60,60));
  char styleSheet[1000];
  std::string color = "blue"; // it always uses blue because it's better
//  if (Config::Get(Config::MAIN_THEME_NAME) == "Clean") {
//      color = "white";
//      shadowColor = Qt::white;
//  } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Blue") {
//      color = "blue";
//      shadowColor = Qt::blue;
//  } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Emerald"){
//      color = "green";
//      shadowColor = Qt::green;
//  } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Lite") {
//      color = "transparent";
//      shadowColor = Qt::transparent;
//  } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Pink") {
//      color = "rgb(255, 192, 203)";
//      shadowColor = QColor(255, 192, 203);
//  }
  std::sprintf(styleSheet, "QPushButton {font-weight: bold; border-radius: 30px; border-width: 1px; border-style: solid; border-color: %s; background-color: LightGray; color: gray}", color.data());
  m_load_button_box->setStyleSheet(QString::fromStdString(styleSheet));
  auto* effect = new QGraphicsDropShadowEffect;
  effect->setOffset(0,0);
  effect->setBlurRadius(25);
  m_load_button_box->setGraphicsEffect(effect);
  m_load_button_box->setText(QStringLiteral("Load"));
  m_load_button_box->setCursor(Qt::PointingHandCursor);
  m_save_button_box = new QPushButton();
  m_save_button_box->setMaximumSize(QSize(60,60));
  m_save_button_box->setMinimumSize(QSize(60,60));
  m_save_button_box->setStyleSheet(QString::fromStdString(styleSheet));
  auto* effect2 = new QGraphicsDropShadowEffect;
  effect2->setOffset(0,0);
  effect2->setBlurRadius(25);
  m_save_button_box->setGraphicsEffect(effect2);
  m_save_button_box->setText(QStringLiteral("Save"));
  m_save_button_box->setCursor(Qt::PointingHandCursor);
  m_wii_mix_button = new WiiMixLogoButton();
  m_wii_mix_button->setIcon(Resources::GetResourceIcon("wiimix_text"));
  m_wii_mix_button->setStyleSheet(QStringLiteral("QToolButton {background-color: #00000000; color: #00000000; border: #FFFFFF}"));
  m_wii_mix_button->setIconSize(QSize(150,100));
  // m_wii_mix_button->installEventFilter(this); // capture mouse events
  m_wii_mix_button->setCursor(Qt::PointingHandCursor);
  //m_wii_mix_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  CreateMainLayout();
  ConnectWidgets();
  return;
}

bool WiiMixSettingsWindow::eventFilter(QObject* obj, QEvent* event) {
  if ((event->type() == QEvent::HoverLeave)) {
    m_wii_mix_button->setGraphicsEffect(nullptr);
  }
  if (event->type() == QEvent::HoverEnter) {
    auto* hoverEffect = new QGraphicsDropShadowEffect;
    hoverEffect->setOffset(0, 0);
    hoverEffect->setBlurRadius(25);
    m_wii_mix_button->setGraphicsEffect(hoverEffect);
  }
  return true;
}

// TODO: Maybe will want this in case there's a settings mismatch between people
// trying to play together
// void WiiMixSettingsWindow::showEvent(QShowEvent* event)
// {
// //   QDialog::showEvent(event);
// //   m_wiimote_controllers->UpdateBluetoothAvailableStatus();
//     return;
// }

void WiiMixSettingsWindow::CreateMainLayout()
{
  // qDebug() << "Bingo Settings:";
  // qDebug() << "Difficulty:" << static_cast<int>(WiiMixBingoSettings::instance()->GetDifficulty());
  // qDebug() << "Save State Bank:" << static_cast<int>(WiiMixBingoSettings::instance()->GetSaveStateBank());
  // // qDebug() << "Mode:" << static_cast<int>(WiiMixBingoSettings::instance()->GetMode());
  // qDebug() << "Bingo Type:" << static_cast<int>(WiiMixBingoSettings::instance()->GetBingoType());
  // qDebug() << "Card Size:" << static_cast<int>(WiiMixBingoSettings::instance()->GetCardSize());
  // qDebug() << "Teams Enabled:" << WiiMixBingoSettings::instance()->GetTeams();
  // qDebug() << "Lobby ID:" << WiiMixBingoSettings::instance()->GetLobbyID();
  // qDebug() << "Lobby Password:" << WiiMixBingoSettings::instance()->GetLobbyPassword();
  // qDebug() << "Players:";

  // qDebug() << "Shuffle Settings:";
  // qDebug() << "Min Time Between Switch:" << WiiMixShuffleSettings::instance()->GetMinTimeBetweenSwitch();
  // qDebug() << "Max Time Between Switch:" << WiiMixShuffleSettings::instance()->GetMaxTimeBetweenSwitch();
  // qDebug() << "Endless:" << WiiMixShuffleSettings::instance()->GetEndless();
  // qDebug() << "Number of Switches:" << WiiMixShuffleSettings::instance()->GetNumberOfSwitches();

  // qDebug() << "Rogue Settings:";
  // qDebug() << "Rogue Length:" << static_cast<int>(WiiMixRogueSettings::instance()->GetLength());

  // qDebug() << "Bingo Settings (again):";
  // qDebug() << "Bingo Type:" << static_cast<int>(WiiMixBingoSettings::instance()->GetBingoType());
  // qDebug() << "Lobby ID:" << WiiMixBingoSettings::instance()->GetLobbyID();

  // qDebug() << "Address of BingoSettings instance:" << WiiMixBingoSettings::instance();
  // qDebug() << "Address of ShuffleSettings instance:" << WiiMixShuffleSettings::instance();
  // qDebug() << "Address of RogueSettings instance:" << WiiMixRogueSettings::instance();


  auto* layout = new QVBoxLayout();

  layout->addWidget(m_modes);
  QWidget* bgWidget = new QWidget();
  bgWidget->setParent(this);
  bgWidget->setAutoFillBackground(true);
  bgWidget->setMaximumHeight(100);
  //bgWidget->setMinimumHeight(1000);
  layout->setContentsMargins(0,0,0,0);
  QHBoxLayout* bottom_buttons = new QHBoxLayout(bgWidget);
  //bottom_buttons->setAlignment(Qt::Alignment::);
  bottom_buttons->setSpacing(0);  // Set spacing between widgets
  bottom_buttons->setContentsMargins(2, 2, 0, 0);  // Remove any margins around the buttons
  bottom_buttons->addStretch(); // Add space before the buttons
  bottom_buttons->addWidget(m_load_button_box, 0);
  bottom_buttons->addStretch();
  bottom_buttons->addWidget(m_wii_mix_button, 0);
  bottom_buttons->addStretch();
  bottom_buttons->addWidget(m_save_button_box, 0);
  bottom_buttons->addStretch(); // Add space after the buttons

  QPixmap buttonBackground;
  if (Config::Get(Config::MAIN_THEME_NAME) == "Clean" || Config::Get(Config::MAIN_THEME_NAME) == "Clean Blue") {
      buttonBackground = (Resources::GetResourceIcon("wiimix_background_bottom").pixmap(1200,100));
  } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Emerald") {
      buttonBackground = (Resources::GetResourceIcon("wiimix_background_bottom_green").pixmap(1200,100));
  } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Pink") {
      buttonBackground = (Resources::GetResourceIcon("wiimix_background_bottom_pink").pixmap(1200,100));
  } else if (Config::Get(Config::MAIN_THEME_NAME) == "Clean Lite") {
      buttonBackground = (Resources::GetResourceIcon("wiimix_background_bottom_clean").pixmap(1200,100));
  } else {
      qDebug() << "Incorrect theme name";
  }
  buttonBackground.scaled(this->size());
  QPalette* palette = new QPalette();
  QBrush* buttonBackgroundBrush = new QBrush();
  buttonBackgroundBrush->setTexture(buttonBackground);
  palette->setBrush(QPalette::Window, *buttonBackgroundBrush);
  bgWidget->setPalette(*palette);
  layout->addWidget(bgWidget);
  //layout->addLayout(bottom_buttons);

  // WrapInScrollArea(this, layout);
  setLayout(layout);
  return;
}

bool WiiMixSettingsWindow::VerifyPlayerInput(bool different_console_types, int player_number, DiscIO::Platform console) {
  // Check player 1 controller (bingo does not support local x online multiplayer)
  if (console == DiscIO::Platform::GameCubeDisc) {
    // Check GameCube controller for player
    // Check if GameCube controller for player is mapped (0 source means none)
    if (Config::Get(Config::GetInfoForSIDevice(player_number - 1)) == SerialInterface::SIDEVICE_NONE) 
    {
      emit ErrorLoadingSettings(QStringLiteral("Player %1 GameCube controller is not mapped. Please configure the controller before starting WiiMix.").arg(player_number));
      return false;
    }
  }
  else if (console == DiscIO::Platform::WiiDisc || console == DiscIO::Platform::WiiWAD) {
    // Check Wii controller for player 1
    // Check if Wiimote for player 1 is mapped
    if (Config::Get(Config::GetInfoForWiimoteSource(player_number - 1)) == WiimoteSource::None) 
    {
      emit ErrorLoadingSettings(QStringLiteral("Player %1 Wii controller is not mapped. Please configure the controller before starting WiiMix.").arg(player_number));
      return false;
    }
  }

  if (different_console_types) {
    // Check if the user is using the same input device and the same wiimix input device for both consoles
    // Get emulated (or real) gamecube controller and emulated wii controller
    ciface::Core::DeviceQualifier gcdevice = Pad::GetConfig()->GetController(player_number - 1)->GetDefaultDevice();
    ciface::Core::DeviceQualifier wiimotedevice = Wiimote::GetConfig()->GetController(player_number - 1)->GetDefaultDevice();
    // TODO: add wiimix err handling later
    // const ciface::Core::DeviceQualifier& wiimix_device = GetDefaultWiiMixDevice();

    if (gcdevice != wiimotedevice) {
      QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, tr("Different Console Types"),
                                    tr("You are using different console types (GameCube and Wii) in this WiiMix. "
                                      "You are currently using different devices for controllers for the respective consoles."
                                      "Do you want to continue?"),
                                    QMessageBox::Yes | QMessageBox::No);
      if (reply == QMessageBox::No) {
        return false; // User chose not to continue
      }
      different_console_types = false; // Only warn once
    }
  }
  return true;
}

void WiiMixSettingsWindow::ConnectWidgets()
{
    connect(m_modes, &WiiMixModesWidget::ModeChanged, this, &WiiMixSettingsWindow::CreateLayout);
    connect(m_load_button_box, &QPushButton::clicked, this, &WiiMixSettingsWindow::LoadSettings);
    connect(m_save_button_box, &QPushButton::clicked, this, &WiiMixSettingsWindow::SaveSettings);
    connect(WiiMixGameManager::instance(), &WiiMixGameManager::onSetWiiMixStarted, m_wii_mix_button, [this](bool state) {
      m_wii_mix_button->setDisabled(state);
    });
    connect(m_wii_mix_button, &QPushButton::clicked, this, [this] {
      // Only start a WiiMix if a WiiMix isn't already running
      if (m_config && !WiiMixGameManager::instance()->IsWiiMixStarted()) {
        // Error out if the game list is empty
        if (WiiMixGlobalSettings::instance()->GetGamesList().empty()) {
          emit ErrorLoadingSettings(QStringLiteral("Cannot start WiiMix: No games selected"));
          return;
        }

        // Error out if the controller configuration is invalid, i.e.:
        // A controller is not mapped for any player participating in the WiiMix for any of the consoles selected
        
        // Determine the list of consoles
        std::set<DiscIO::Platform> consoles = {};
        for (const std::shared_ptr<const UICommon::GameFile> game : WiiMixGlobalSettings::instance()->GetGamesList()) {
          // Add the console for the game if not already in the set
          // If the platform type is ELFOrDOL, need to determine if it's a Wii or GameCube game so ask the user
          if (game->GetPlatform() == DiscIO::Platform::ELFOrDOL) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Unknown Platform"));
            msgBox.setText(tr("The game \"%1\" is an ELF or DOL file. Please specify its platform:").arg(QString::fromStdString(game->GetFileName())));
            QPushButton* gcButton = msgBox.addButton(tr("GameCube"), QMessageBox::AcceptRole);
            QPushButton* wiiButton = msgBox.addButton(tr("Wii"), QMessageBox::AcceptRole);
            msgBox.setIcon(QMessageBox::Question);
            msgBox.exec();

            DiscIO::Platform selected_platform = DiscIO::Platform::GameCubeDisc;
            if (msgBox.clickedButton() == wiiButton)
              selected_platform = DiscIO::Platform::WiiDisc;

            // Use a public setter instead of direct member access
            // Create a non-const copy and set the platform
            std::shared_ptr<UICommon::GameFile> non_const_game = std::make_shared<UICommon::GameFile>(*game);
            non_const_game->SetPlatform(selected_platform);

            // Save the updated platform to the game's config file
            // NOTE: not sure if this would cause any problems
            Common::IniFile ini;
            std::string config_path = File::GetUserPath(D_GAMESETTINGS_IDX) + game->GetGameID() + ".ini";
            ini.Load(config_path);
            auto* section = ini.GetOrCreateSection("Game");
            section->Set("Platform", static_cast<int>(selected_platform));
            ini.Save(config_path);
          }
          consoles.insert(game->GetPlatform());
        }

        bool different_console_types = false;
        if (consoles.count(DiscIO::Platform::GameCubeDisc) > 0 &&
            (consoles.count(DiscIO::Platform::WiiDisc) > 0 || consoles.count(DiscIO::Platform::WiiWAD) > 0))
          different_console_types = true;

        // For each console, check the controller mappings
        for (const auto& console : consoles) {
          // Single player if bingo, otherwise possible multiplayer
          if (WiiMixGlobalSettings::instance()->GetMode() == WiiMixEnums::Mode::BINGO) {
            // Check the single player's controller
            if (!VerifyPlayerInput(different_console_types, 1, console)) {
              return; // Could not verify player input
            }
            different_console_types = false; // Only warn for different_console_types the first time
          }
          else if (WiiMixGlobalSettings::instance()->GetMode() == WiiMixEnums::Mode::SHUFFLE) {
            // Check the number of players for shuffle
            for (int player = 1; player <= WiiMixShuffleSettings::instance()->GetNumPlayers(); player++) {
              if (!VerifyPlayerInput(different_console_types, player, console)) {
                return; // Could not verify player input
              }
              different_console_types = false; // Only warn for different_console_types the first time
            }
          }
          else if (WiiMixGlobalSettings::instance()->GetMode() == WiiMixEnums::Mode::ROGUE) {
            // Check the number of players for rogue
            for (int player = 1; player <= WiiMixRogueSettings::instance()->GetNumPlayers(); player++) {
              if (!VerifyPlayerInput(different_console_types, player, console)) {
                return; // Could not verify player input
              }
              different_console_types = false; // Only warn for different_console_types the first time
            }
          }
        }
        
        // Warning if controllers are mapped differently across consoles, i.e:
        // Player 1 is mapping keyboard to Wii but Xbox Controller to GameCube
        // Option to quit and view controller configuration or continue anyway

        WiiMixGameManager::instance()->SetWiiMixStarted(true);
        if (WiiMixGlobalSettings::instance()->GetMode() == WiiMixEnums::Mode::BINGO) {
          WiiMixBingoSettings::instance()->SetDifficulty(WiiMixCommonSettings::StringToDifficulty(m_config->GetDifficulty()));
          WiiMixBingoSettings::instance()->SetSaveStateBank(WiiMixCommonSettings::StringToSaveStateBank(m_config->GetSaveStateBank()));
          WiiMixBingoSettings::instance()->SetObjectiveTypes(m_config->GetObjectiveTypes());
          WiiMixBingoSettings::instance()->SetGameGenres(m_config->GetGameGenres());
          WiiMixBingoSettings::instance()->SetCardSize(WiiMixBingoSettings::StringToCardSize(m_config->GetCardSize()));
          WiiMixBingoSettings::instance()->SetBingoType(m_config->GetBingoType());
          emit StartWiiMixBingo(WiiMixBingoSettings::instance(), WiiMixClient::instance());
        }
        else if (WiiMixGlobalSettings::instance()->GetMode() == WiiMixEnums::Mode::SHUFFLE) {
          WiiMixShuffleSettings::instance()->SetDifficulty(WiiMixCommonSettings::StringToDifficulty(m_config->GetDifficulty()));
          WiiMixShuffleSettings::instance()->SetSaveStateBank(WiiMixCommonSettings::StringToSaveStateBank(m_config->GetSaveStateBank()));
          WiiMixShuffleSettings::instance()->SetObjectiveTypes(m_config->GetObjectiveTypes());
          WiiMixShuffleSettings::instance()->SetGameGenres(m_config->GetGameGenres());
          WiiMixShuffleSettings::instance()->SetNumberOfSwitches(m_config->GetNumSwitches());
          WiiMixShuffleSettings::instance()->SetMinTimeBetweenSwitch(m_config->GetMinTimeBetweenSwitch());
          WiiMixShuffleSettings::instance()->SetMaxTimeBetweenSwitch(m_config->GetMaxTimeBetweenSwitch());
          WiiMixShuffleSettings::instance()->SetSeed(m_config->GetShuffleSeed().toStdString());
          WiiMixShuffleSettings::instance()->SetNumPlayers(m_config->GetNumPlayersShuffle());
          // WiiMixShuffleSettings::instance()->SetEndless(m_config->GetEndless());
          emit StartWiiMixShuffle(WiiMixShuffleSettings::instance());
        }
        else if (WiiMixGlobalSettings::instance()->GetMode() == WiiMixEnums::Mode::ROGUE) {
          WiiMixRogueSettings::instance()->SetDifficulty(WiiMixCommonSettings::StringToDifficulty(m_config->GetDifficulty()));
          WiiMixRogueSettings::instance()->SetSaveStateBank(WiiMixCommonSettings::StringToSaveStateBank(m_config->GetSaveStateBank()));
          WiiMixRogueSettings::instance()->SetObjectiveTypes(m_config->GetObjectiveTypes());
          WiiMixRogueSettings::instance()->SetGameGenres(m_config->GetGameGenres());
          WiiMixRogueSettings::instance()->SetLength(m_config->GetRogueLength());
          WiiMixRogueSettings::instance()->SetSeed(m_config->GetRogueSeed());
          emit StartWiiMixRogue(WiiMixRogueSettings::instance());
        }
      }
    }); // Start WiiMix
    connect(this, &WiiMixSettingsWindow::ErrorLoadingSettings, this, [this](const QString& error_message) {
      // Display Error message
      QMessageBox::critical(this, tr("Error"), error_message);
    });
}

void WiiMixSettingsWindow::CreateLayout(WiiMixEnums::Mode mode)
{
  m_config = new WiiMixConfigWidget(this);
  WiiMixGlobalSettings::instance()->SetMode(mode);
  m_config->CreateLayout(mode);

  // TODOx REMOVE THIS ITS ONLY FOR TESTING
  // WiiMixSettings::instance()->AddGame(UICommon::GameFile("/Users/scie/Documents/dolphinjank/iso/NewSuperMarioBrosWii.wbfs"));
  // WiiMixSettings::instance()->AddGame(UICommon::GameFile("/Users/scie/Documents/dolphinjank/iso/Wii Sports (USA) (Rev 1).wbfs"));

  // Create a new window with the configuration options
  auto* config_window = new QDialog(this);
  config_window->setWindowTitle(tr("Configuration Options"));
  auto* config_layout = new QVBoxLayout(config_window);
  config_layout->addWidget(m_config);
  config_window->setLayout(config_layout);
  config_window->show();
  // config_window->exec();
  return;
}

void WiiMixSettingsWindow::LoadSettings() {
  auto& settings = Settings::Instance().GetQSettings();
  QString path = DolphinFileDialog::getOpenFileName(
      this, tr("Select a File"),
      settings.value(QStringLiteral("mainwindow/lastdir"), QString{}).toString(),
      QStringLiteral("%1 (*.ini)")
          .arg(tr("Configuration file (*.ini)")));

  if (!path.isEmpty()) {
    // Load the settings from the file
    Common::IniFile ini = Common::IniFile();
    qDebug() << "Loading WiiMix settings from file:" << path;
    ini.Load(path.toStdString());
    // Load the settings into the config widget
    // Erroring out if any of the settings are missing
    if (!ini.GetSection("WiiMix")) {
      return;
    }
    Common::IniFile::Section *section = ini.GetSection("WiiMix");

    std::string mode_val;
    section->Get("Mode", &mode_val, "");
    qDebug() << "Value is";
    qDebug() << mode_val.c_str();

    if (mode_val.empty()) {
      QMessageBox::warning(this, tr("Warning"), tr("Could not load settings: Mode not found in WiiMix settings file"));
      return;
    }
    if (WiiMixGlobalSettings::instance()->GetMode() != WiiMixEnums::StringToMode(mode_val.c_str())) {
      QMessageBox::warning(this, tr("Warning"), tr("Could not load settings: WiiMix settings file mode does not match the currently selected mode!"));
      return;
    }
    WiiMixGlobalSettings::instance()->SetMode(WiiMixEnums::StringToMode(mode_val.c_str()));

    WiiMixEnums::Mode mode = WiiMixEnums::StringToMode(mode_val.c_str());
    switch (mode) {
      case WiiMixEnums::Mode::BINGO:
        {
          std::string val;
          // Load bingo settings
          section->Get("BingoType", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("BingoType not found in WiiMix settings file"));
            return;
          }
          WiiMixBingoSettings::instance()->SetBingoType(WiiMixBingoSettings::StringToBingoType(QString::fromStdString(val.c_str())));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_TYPE, WiiMixBingoSettings::instance()->GetBingoType());

          section->Get("CardSize", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Card Size not found in WiiMix settings file"));
            return;
          }
          WiiMixBingoSettings::instance()->SetCardSize(WiiMixBingoSettings::StringToCardSize(QString::fromStdString(val.c_str())));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_CARD_SIZE, WiiMixBingoSettings::instance()->GetCardSize());

          section->Get("Teams", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Teams not found in WiiMix settings file"));
            return;
          }
          WiiMixBingoSettings::instance()->SetTeams(val == std::string("true"));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_TEAMS, WiiMixBingoSettings::instance()->GetTeams());

          section->Get("ObjectiveTypes", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("ObjectiveTypes not found in WiiMix settings file"));
            return;
          }
          WiiMixBingoSettings::instance()->SetObjectiveTypes(WiiMixEnums::ObjectiveTypesFromString(val.c_str()));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_OBJECTIVE_TYPES, WiiMixEnums::ObjectiveTypesToString(WiiMixBingoSettings::instance()->GetObjectiveTypes()));

          section->Get("GameGenres", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("GameGenres not found in WiiMix settings file"));
            return;
          }
          WiiMixBingoSettings::instance()->SetGameGenres(WiiMixEnums::GameGenresFromString(val.c_str()));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_GAME_GENRES, WiiMixEnums::GameGenresToString(WiiMixBingoSettings::instance()->GetGameGenres()));

          section->Get("BingoSeed", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("BingoSeed not found in WiiMix settings file"));
            return;
          }
          WiiMixBingoSettings::instance()->SetSeed(QString::fromStdString(val.c_str()));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_SEED, WiiMixBingoSettings::instance()->GetSeed().toStdString());
        }
        break;
      case WiiMixEnums::Mode::SHUFFLE:
        {
          std::string val;
          // Load shuffle settings
          section->Get("NumberOfSwitches", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Number of Switches not found in WiiMix settings file"));
            return;
          }
          WiiMixShuffleSettings::instance()->SetNumberOfSwitches(std::stoi(val));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_NUMBER_OF_SWITCHES, WiiMixShuffleSettings::instance()->GetNumberOfSwitches());

          section->Get("MinTimeBetweenSwitch", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Min Time Between Switch not found in WiiMix settings file"));
            return;
          }
          WiiMixShuffleSettings::instance()->SetMinTimeBetweenSwitch(std::stoi(val));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH, WiiMixShuffleSettings::instance()->GetMinTimeBetweenSwitch());

          section->Get("MaxTimeBetweenSwitch", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("Max Time Between Switch not found in WiiMix settings file"));
            return;
          }
          WiiMixShuffleSettings::instance()->SetMaxTimeBetweenSwitch(std::stoi(val));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH, WiiMixShuffleSettings::instance()->GetMaxTimeBetweenSwitch());

          // section->Get("Endless", &val, "");
          // if (val.empty()) {
          //   QMessageBox::warning(this, tr("Warning"), tr("Endless not found in WiiMix settings file"));
          //   return;
          // }
          // WiiMixShuffleSettings::instance()->SetEndless(val == std::string("true"));
          // Config::Set(Config::LayerType::Base, Config::WIIMIX_IS_ENDLESS, WiiMixShuffleSettings::instance()->GetEndless());
          section->Get("ShuffleSeed", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("ShuffleSeed not found in WiiMix settings file"));
            return;
          }
          WiiMixShuffleSettings::instance()->SetSeed(val);
          Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_SEED, WiiMixShuffleSettings::instance()->GetSeed());

          section->Get("ObjectiveTypes", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("ObjectiveTypes not found in WiiMix settings file"));
            return;
          }
          WiiMixShuffleSettings::instance()->SetObjectiveTypes(WiiMixEnums::ObjectiveTypesFromString(val.c_str()));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_OBJECTIVE_TYPES, WiiMixEnums::ObjectiveTypesToString(WiiMixShuffleSettings::instance()->GetObjectiveTypes()));

          section->Get("GameGenres", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("GameGenres not found in WiiMix settings file"));
            return;
          }
          WiiMixShuffleSettings::instance()->SetGameGenres(WiiMixEnums::GameGenresFromString(val.c_str()));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_GAME_GENRES, WiiMixEnums::GameGenresToString(WiiMixShuffleSettings::instance()->GetGameGenres()));

          section->Get("NumPlayers", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("NumPlayers not found in WiiMix settings file"));
            return;
          }
          WiiMixShuffleSettings::instance()->SetNumPlayers(std::stoi(val));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_NUM_PLAYERS_SHUFFLE, WiiMixShuffleSettings::instance()->GetNumPlayers());

          section->Get("MultiplayerMode", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("MultiplayerMode not found in WiiMix settings file"));
            return;
          }
          WiiMixShuffleSettings::instance()->SetMultiplayerMode(WiiMixEnums::StringToMultiplayerMode(val.c_str()));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_MULTIPLAYER_MODE_SHUFFLE, WiiMixShuffleSettings::instance()->GetMultiplayerMode());
        }
        break;
      case WiiMixEnums::Mode::ROGUE:
        {
          std::string val;
          // Load rogue settings
          section->Get("RogueLength", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("RogueLength not found in WiiMix settings file"));
            return;
          }
          WiiMixRogueSettings::instance()->SetLength(WiiMixRogueSettings::StringToLength(QString::fromStdString(val.c_str())));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_LENGTH, WiiMixRogueSettings::instance()->GetLength());

          section->Get("ObjectiveTypes", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("ObjectiveTypes not found in WiiMix settings file"));
            return;
          }
          WiiMixRogueSettings::instance()->SetObjectiveTypes(WiiMixEnums::ObjectiveTypesFromString(val.c_str()));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_OBJECTIVE_TYPES, WiiMixEnums::ObjectiveTypesToString(WiiMixRogueSettings::instance()->GetObjectiveTypes()));

          section->Get("GameGenres", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("GameGenres not found in WiiMix settings file"));
            return;
          }
          WiiMixRogueSettings::instance()->SetGameGenres(WiiMixEnums::GameGenresFromString(val.c_str()));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_GAME_GENRES, WiiMixEnums::GameGenresToString(WiiMixRogueSettings::instance()->GetGameGenres()));

          section->Get("RogueSeed", &val, "");
          if (val.empty()) {
            QMessageBox::warning(this, tr("Warning"), tr("RogueSeed not found in WiiMix settings file"));
            return;
          }
          WiiMixRogueSettings::instance()->SetSeed(QString::fromStdString(val.c_str()));
          Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_SEED, WiiMixRogueSettings::instance()->GetSeed().toStdString());
        }
        break;
    }
  }
}

void WiiMixSettingsWindow::SaveSettings() {
  // Set all config values
  auto& settings = Settings::Instance().GetQSettings();
  WiiMixEnums::Mode mode = WiiMixGlobalSettings::instance()->GetMode();
  // Helper function to capitalize the first letter of a QString
  auto capitalizeFirst = [](const QString& str) -> QString {
    if (str.isEmpty())
      return str;
    QString result = str;
    result[0] = result[0].toUpper();
    return result;
  };

  QString default_filename = QStringLiteral("%1_Settings.ini").arg(capitalizeFirst(QString::fromStdString(WiiMixEnums::ModeToString(mode)).toLower()));

  QString path = DolphinFileDialog::getSaveFileName(
      this, tr("Save File"),
      settings.value(QStringLiteral("mainwindow/lastdir"), QString{}).toString() + QStringLiteral("/") + default_filename,
      QStringLiteral("%1 Settings (*.ini)")
          .arg((QString::fromStdString(WiiMixEnums::ModeToString(mode)))));

  if (!path.endsWith(QStringLiteral(".ini"), Qt::CaseInsensitive)) {
    path += QStringLiteral(".ini");
  }

  if (!path.isEmpty()) {
    Common::IniFile ini = Common::IniFile();
    auto* section = ini.GetOrCreateSection("WiiMix");

    // section->Set("Games", WiiMixSettings::GameFilesToGameIds(WiiMixSettings::instance()->GetGamesList()));
    // Config::Set(Config::LayerType::Base, Config::WIIMIX_GAME_IDS, WiiMixSettings::GameFilesToGameIds(WiiMixSettings::instance()->GetGamesList()));
    section->Set("Mode", WiiMixEnums::ModeToString(WiiMixGlobalSettings::instance()->GetMode()));

    if (WiiMixGlobalSettings::instance()->GetMode() == WiiMixEnums::Mode::BINGO) {
      section->Set("BingoDifficulty", WiiMixCommonSettings::DifficultyToString(WiiMixBingoSettings::instance()->GetDifficulty()).toStdString());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_DIFFICULTY, WiiMixBingoSettings::instance()->GetDifficulty());
      
      section->Set("BingoSaveStateBank", WiiMixCommonSettings::SaveStateBankToString(WiiMixBingoSettings::instance()->GetSaveStateBank()).toStdString());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_SAVE_STATE_BANK, WiiMixBingoSettings::instance()->GetSaveStateBank());

      section->Set("BingoType", WiiMixBingoSettings::instance()->GetBingoType());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_TYPE, WiiMixBingoSettings::instance()->GetBingoType());

      section->Set("CardSize", WiiMixBingoSettings::instance()->GetCardSize());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_CARD_SIZE, WiiMixBingoSettings::instance()->GetCardSize());
    
      section->Set("Teams", WiiMixBingoSettings::instance()->GetTeams());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_TEAMS, WiiMixBingoSettings::instance()->GetTeams());

      section->Set("ObjectiveTypes", WiiMixEnums::ObjectiveTypesToString(WiiMixBingoSettings::instance()->GetObjectiveTypes()));
      Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_OBJECTIVE_TYPES, WiiMixEnums::ObjectiveTypesToString(WiiMixBingoSettings::instance()->GetObjectiveTypes()));

      // Game genres
      section->Set("GameGenres", WiiMixEnums::GameGenresToString(WiiMixBingoSettings::instance()->GetGameGenres()));
      Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_GAME_GENRES, WiiMixEnums::GameGenresToString(WiiMixBingoSettings::instance()->GetGameGenres()));

      // BingoSeed
      section->Set("BingoSeed", WiiMixBingoSettings::instance()->GetSeed().toStdString());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_SEED, WiiMixBingoSettings::instance()->GetSeed().toStdString());
    }
    else if (WiiMixGlobalSettings::instance()->GetMode() == WiiMixEnums::Mode::SHUFFLE) {
      section->Set("ShuffleDifficulty", WiiMixCommonSettings::DifficultyToString(WiiMixShuffleSettings::instance()->GetDifficulty()).toStdString());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_DIFFICULTY, WiiMixShuffleSettings::instance()->GetDifficulty());
      
      section->Set("ShuffleSaveStateBank", WiiMixCommonSettings::SaveStateBankToString(WiiMixShuffleSettings::instance()->GetSaveStateBank()).toStdString());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_SAVE_STATE_BANK, WiiMixShuffleSettings::instance()->GetSaveStateBank());

      section->Set("NumberOfSwitches", WiiMixShuffleSettings::instance()->GetNumberOfSwitches());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_NUMBER_OF_SWITCHES, WiiMixShuffleSettings::instance()->GetNumberOfSwitches());

      section->Set("MinTimeBetweenSwitch", WiiMixShuffleSettings::instance()->GetMinTimeBetweenSwitch());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH, WiiMixShuffleSettings::instance()->GetMinTimeBetweenSwitch());

      section->Set("MaxTimeBetweenSwitch", WiiMixShuffleSettings::instance()->GetMaxTimeBetweenSwitch());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH, WiiMixShuffleSettings::instance()->GetMaxTimeBetweenSwitch());

      // section->Set("Endless", WiiMixShuffleSettings::instance()->GetEndless());
      // Config::Set(Config::LayerType::Base, Config::WIIMIX_IS_ENDLESS, WiiMixShuffleSettings::instance()->GetEndless());

      section->Set("ShuffleSeed", WiiMixShuffleSettings::instance()->GetSeed());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_SEED, WiiMixShuffleSettings::instance()->GetSeed());

      // Also set
      // Objective types
      section->Set("ObjectiveTypes", WiiMixEnums::ObjectiveTypesToString(WiiMixShuffleSettings::instance()->GetObjectiveTypes()));
      Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_OBJECTIVE_TYPES, WiiMixEnums::ObjectiveTypesToString(WiiMixShuffleSettings::instance()->GetObjectiveTypes()));

      // Game genres
      section->Set("GameGenres", WiiMixEnums::GameGenresToString(WiiMixShuffleSettings::instance()->GetGameGenres()));
      Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_GAME_GENRES, WiiMixEnums::GameGenresToString(WiiMixShuffleSettings::instance()->GetGameGenres()));

      // Num players
      section->Set("NumPlayers", WiiMixShuffleSettings::instance()->GetNumPlayers());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_NUM_PLAYERS_SHUFFLE, WiiMixShuffleSettings::instance()->GetNumPlayers());

      // Multiplayer Mode
      section->Set("MultiplayerMode", WiiMixShuffleSettings::instance()->GetMultiplayerMode());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_MULTIPLAYER_MODE_SHUFFLE, WiiMixShuffleSettings::instance()->GetMultiplayerMode());
    }
    else if (WiiMixGlobalSettings::instance()->GetMode() == WiiMixEnums::Mode::ROGUE) {
      section->Set("RogueDifficulty", WiiMixCommonSettings::DifficultyToString(WiiMixRogueSettings::instance()->GetDifficulty()).toStdString());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_DIFFICULTY, WiiMixRogueSettings::instance()->GetDifficulty());
      
      section->Set("RogueSaveStateBank", WiiMixCommonSettings::SaveStateBankToString(WiiMixRogueSettings::instance()->GetSaveStateBank()).toStdString());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_BINGO_SAVE_STATE_BANK, WiiMixRogueSettings::instance()->GetSaveStateBank());

      section->Set("RogueLength", WiiMixRogueSettings::instance()->GetLength());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_LENGTH, WiiMixRogueSettings::instance()->GetLength());
    
      // Also set
      // Objective types
      section->Set("ObjectiveTypes", WiiMixEnums::ObjectiveTypesToString(WiiMixRogueSettings::instance()->GetObjectiveTypes()));
      Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_OBJECTIVE_TYPES, WiiMixEnums::ObjectiveTypesToString(WiiMixRogueSettings::instance()->GetObjectiveTypes()));

      // Game genres
      section->Set("GameGenres", WiiMixEnums::GameGenresToString(WiiMixRogueSettings::instance()->GetGameGenres()));
      Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_GAME_GENRES, WiiMixEnums::GameGenresToString(WiiMixRogueSettings::instance()->GetGameGenres()));

      // RogueSeed
      section->Set("RogueSeed", WiiMixRogueSettings::instance()->GetSeed().toStdString());
      Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_SEED, WiiMixRogueSettings::instance()->GetSeed().toStdString());
    }

    ini.Save(path.toStdString());
  }
}

std::vector<UICommon::GameFile> WiiMixSettingsWindow::GetGamesList() {
  std::vector<UICommon::GameFile> game_files;
  for (const auto& game_ptr : WiiMixGlobalSettings::instance()->GetGamesList())
  {
    game_files.push_back(*game_ptr);
  }
  return game_files;
}