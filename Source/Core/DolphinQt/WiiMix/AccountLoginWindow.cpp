#include "DolphinQt/WiiMix/AccountLoginWindow.h"
#include "DolphinQt/WiiMix/Client.h"
#include "DolphinQt/WiiMix/Player.h"
#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/GlobalSettings.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QMainWindow>

WiiMixAccountLoginWindow::WiiMixAccountLoginWindow(QWidget* parent) : QMainWindow(parent) {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    CreateLayout();
    ConnectWidgets();
    // Ensure the window is shown
    this->show();
}

void WiiMixAccountLoginWindow::OnLogin(QString username, QString password) {
    // Make a request to the server to add the player (fails if the player already exists, which is ok because nothing needs to be returned)
    WiiMixPlayer player = WiiMixPlayer(-1, username.toStdString(), password.toStdString());
    QJsonObject player_json = player.ToJson();
    WiiMixClient::instance()->SendData(player_json, WiiMixEnums::Action::ADD_PLAYER);
    // Make a request to retrieve the player and all the player data
    player_json[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::GET_PLAYER);
    // connect the client for the response
    connect(WiiMixClient::instance(), &WiiMixClient::onGetPlayer, this, [this](WiiMixPlayer player) {
        qDebug() << "Player received";
        qDebug() << player.ToJson();
        // Set the player
        WiiMixGlobalSettings::instance()->SetPlayer(&player);
        // Close the login window
        close();
        emit WiiMixAccountLoginWindow::onLogin();
    });
    WiiMixClient::instance()->SendData(player_json, WiiMixEnums::Action::GET_PLAYERS);
    return;
}

void WiiMixAccountLoginWindow::CreateLayout() {
    QVBoxLayout* layout = new QVBoxLayout;

    QLabel* username_label = new QLabel(tr("Username:"));
    m_username = new QLineEdit();
    layout->addWidget(username_label);
    layout->addWidget(m_username);

    QLabel* password_label = new QLabel(tr("Password:"));
    m_password = new QLineEdit();
    m_password->setEchoMode(QLineEdit::Password);
    layout->addWidget(password_label);
    layout->addWidget(m_password);

    m_login_button = new QPushButton(tr("Login/Register"));
    layout->addWidget(m_login_button);

    centralWidget()->setLayout(layout);
}

void WiiMixAccountLoginWindow::ConnectWidgets() {
    if (!m_login_button) {
        qDebug() << "m_login_button is nullptr";
        return;
    }

    // Connect the login button
    connect(m_login_button, &QPushButton::clicked, this, [this]() {
        QString username = m_username->text();
        QString password = m_password->text();
        OnLogin(username, password);
        emit WiiMixAccountLoginWindow::onLogin();
    });
}
