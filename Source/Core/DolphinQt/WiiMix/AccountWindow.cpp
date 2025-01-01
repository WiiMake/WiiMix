#include "DolphinQt/WiiMix/AccountWindow.h"
#include "DolphinQt/WiiMix/GlobalSettings.h"
#include "DolphinQt/WiiMix/Player.h"


WiiMixAccountWindow::WiiMixAccountWindow(QWidget* parent, WiiMixPlayer *player)
    : QWidget(parent), m_player(player)
{
  setWindowTitle(tr("WiiMix Account"));
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  CreateLayout();
  ConnectWidgets();
}

void WiiMixAccountWindow::CreateLayout() {
    m_username = new QLabel(tr("Username: " + m_player->GetUsername()), this);
    m_logout_button = new QPushButton(tr("Logout"), this);
    m_num_objectives_completed = new QLabel(tr("Objectives Completed: " + m_player->GetNumObjectivesCompleted()), this);
    m_num_unique_objectives_completed = new QLabel(tr("Unique Objectives Completed: " + m_player->GetNumUniqueObjectivesCompleted()), this);
    m_num_objectives_attempted = new QLabel(tr("Objectives Attempted: " + m_player->GetNumObjectivesAttempted()), this);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(m_username);
    layout->addWidget(m_logout_button);
    layout->addWidget(m_num_objectives_completed);
    layout->addWidget(m_num_unique_objectives_completed);
    layout->addWidget(m_num_objectives_attempted);

    setLayout(layout);
}

void WiiMixAccountWindow::ConnectWidgets() {
    connect(m_player, &WiiMixPlayer::onSetNumObjectivesCompleted, this, [this](int num_objectives_completed) {
        m_num_objectives_completed->setText(tr("Objectives Completed: " + num_objectives_completed));
        Config::Set(Config::WIIMIX_PLAYER_NUM_OBJECTIVES_COMPLETED, num_objectives_completed);
    });
    connect(m_player, &WiiMixPlayer::onSetNumUniqueObjectivesCompleted, this, [this](int num_unique_objectives_completed) {
        m_num_unique_objectives_completed->setText(tr("Unique Objectives Completed: " + num_unique_objectives_completed));
        Config::Set(Config::WIIMIX_PLAYER_NUM_UNIQUE_OBJECTIVES_COMPLETED, num_unique_objectives_completed);
    });
    connect(m_player, &WiiMixPlayer::onSetNumObjectivesAttempted, this, [this](int num_objectives_attempted) {
        m_num_objectives_attempted->setText(tr("Objectives Attempted: " + num_objectives_attempted));
        Config::Set(Config::WIIMIX_PLAYER_NUM_OBJECTIVES_ATTEMPTED, num_objectives_attempted);
    });
    // connect the logout button
    connect(m_logout_button, &QPushButton::clicked, this, &WiiMixAccountWindow::OnLogout);
}

void WiiMixAccountWindow::OnLogout() {
    // Clear player references
    Config::Set(Config::WIIMIX_PLAYER_USERNAME, "");
    Config::Set(Config::WIIMIX_PLAYER_NUM_OBJECTIVES_COMPLETED, 0);
    Config::Set(Config::WIIMIX_PLAYER_NUM_UNIQUE_OBJECTIVES_COMPLETED, 0);
    Config::Set(Config::WIIMIX_PLAYER_NUM_OBJECTIVES_ATTEMPTED, 0);
    WiiMixGlobalSettings::m_player = nullptr;
    // Close the window
    close();
    // Emit a signal to open the login window
    emit onLogout();
}