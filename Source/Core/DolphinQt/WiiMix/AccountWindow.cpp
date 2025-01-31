#include "DolphinQt/WiiMix/AccountWindow.h"
#include "DolphinQt/WiiMix/GlobalSettings.h"
#include "DolphinQt/WiiMix/Player.h"
#include "DolphinQt/WiiMix/Enums.h"
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

#include <QVBoxLayout>

WiiMixAccountWindow::WiiMixAccountWindow(QWidget* parent, WiiMixPlayer *player)
    : QMainWindow(parent), m_player(player)
{
  QWidget* centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  setWindowTitle(tr("WiiMix Account"));
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  CreateLayout();
  ConnectWidgets();

  this->show();
}

void WiiMixAccountWindow::CreateLayout() {
    m_username = new QLabel(QStringLiteral("Username: %1").arg(QString::fromStdString(m_player->GetUsername())), this);
    m_logout_button = new QPushButton(tr("Logout"), this);
    m_num_objectives_completed = new QLabel(QStringLiteral("Objectives Completed: %1").arg(m_player->GetNumObjectivesCompleted()), this);
    m_num_unique_objectives_completed = new QLabel(QStringLiteral("Unique Objectives Completed: %1").arg(m_player->GetNumUniqueObjectivesCompleted()), this);
    m_num_objectives_attempted = new QLabel(QStringLiteral("Objectives Attempted: %1").arg(m_player->GetNumObjectivesAttempted()), this);
    m_num_objectives_created = new QLabel(QStringLiteral("Objectives Created: %1").arg(m_player->GetNumObjectivesCreated()), this);

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(m_username);
    layout->addWidget(m_logout_button);
    layout->addWidget(m_num_objectives_completed);
    layout->addWidget(m_num_unique_objectives_completed);
    layout->addWidget(m_num_objectives_attempted);
    layout->addWidget(m_num_objectives_created);

    centralWidget()->setLayout(layout);
    qDebug() << "Account window created";
}

void WiiMixAccountWindow::ConnectWidgets() {
    // TODOx: these signals need to be emitted in the correct places (i.e. whenever an update occurs)
    connect(WiiMixGlobalSettings::instance(), &WiiMixGlobalSettings::onSetNumObjectivesCompleted, this, [this](int num_objectives_completed) {
        m_num_objectives_completed->setText(QStringLiteral("Objectives Completed: %1").arg(num_objectives_completed));
        Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_OBJECTIVES_COMPLETED, num_objectives_completed);
    });
    connect(WiiMixGlobalSettings::instance(), &WiiMixGlobalSettings::onSetNumUniqueObjectivesCompleted, this, [this](int num_unique_objectives_completed) {
        m_num_unique_objectives_completed->setText(QStringLiteral("Unique Objectives Completed: %1").arg(num_unique_objectives_completed));
        Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_UNIQUE_OBJECTIVES_COMPLETED, num_unique_objectives_completed);
    });
    connect(WiiMixGlobalSettings::instance(), &WiiMixGlobalSettings::onSetNumObjectivesAttempted, this, [this](int num_objectives_attempted) {
        m_num_objectives_attempted->setText(QStringLiteral("Objectives Attempted: %1").arg(num_objectives_attempted));
        Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_OBJECTIVES_ATTEMPTED, num_objectives_attempted);
    });
    connect(WiiMixGlobalSettings::instance(), &WiiMixGlobalSettings::onSetNumObjectivesCreated, this, [this](int num_objectives_created) {
        m_num_objectives_created->setText(QStringLiteral("Objectives Created: %1").arg(num_objectives_created));
        Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_OBJECTIVES_CREATED, num_objectives_created);
    });
    // connect the logout button
    connect(m_logout_button, &QPushButton::clicked, this, &WiiMixAccountWindow::OnLogout);
}

void WiiMixAccountWindow::OnLogout() {
    // Clear player references
    Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_USERNAME, "");
    Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_OBJECTIVES_COMPLETED, 0);
    Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_UNIQUE_OBJECTIVES_COMPLETED, 0);
    Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_OBJECTIVES_ATTEMPTED, 0);
    Config::Set(Config::LayerType::Base, Config::WIIMIX_PLAYER_NUM_OBJECTIVES_CREATED, 0);
    WiiMixGlobalSettings::instance()->SetPlayer(nullptr);
    // Close the window
    close();
    // Emit a signal to open the login window
    emit WiiMixAccountWindow::onLogout();
}