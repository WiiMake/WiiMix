#include "DolphinQt/WiiMix/AccountLoginWindow.h"
#include "DolphinQt/WiiMix/Client.h"
#include "DolphinQt/WiiMix/Player.h"

WiiMixAccountLoginWindow::WiiMixAccountLoginWindow(QWidget* parent) : QWidget(parent) {
    CreateLayout();
    ConnectWidgets();
}

void WiiMixAccountLoginWindow::OnLogin(QString username, QString password) {
    // Make a request to the server to add the player (fails if the player already exists, which is ok because nothing needs to be returned)
    WiiMixPlayer player = WiiMixPlayer(-1, username, password);
    QJsonObject player_json = player.ToJson();
    WiiMixClient::instance()->SendData(player_json, WiiMixEnums::Action::ADD_PLAYER);
    // Make a request to retrieve the player and all the player data
    player_json[QStringLiteral(CLIENT_RESPONSE)] = static_cast<int>(WiiMixEnums::Response::GET_PLAYER);
    // connect the client for the response
    connect(WiiMixClient::instance(), &WiiMixClient::onGetPlayer, this, [this](WiiMixPlayer player) {
        // Set the player
        WiiMixGlobalSettings::instance()->SetPlayer(&player);
        // Close the login window
        close();
        emit onLogin();
    });
    WiiMixClient::instance()->SendData(player.ToJson(), WiiMixEnums::Action::GET_PLAYER);
    return;
}

void WiiMixAccountLoginWindow::CreateLayout() {
    QVBoxLayout* layout = new QVBoxLayout;

    QLabel* username_label = new QLabel(tr("Username:"));
    QLineEdit* m_username = new QLineEdit();
    layout->addWidget(username_label);
    layout->addWidget(m_username);

    QLabel* password_label = new QLabel(tr("Password:"));
    QLineEdit* m_password = new QLineEdit();
    password_edit->setEchoMode(QLineEdit::Password);
    layout->addWidget(password_label);
    layout->addWidget(m_password);

    QPushButton* m_login_button = new QPushButton(tr("Login/Register"));
    layout->addWidget(m_login_button);

    setLayout(layout);
}

void WiiMixAccountLoginWindow::ConnectWidgets() {
    // Connect the login button
    connect(m_login_button, &QPushButton::clicked, this, [this]() {
        QString username = m_username->text();
        QString password = m_password->text();
        OnLogin(username, password);
        emit onLogin(m_username->text(), m_password->text());
    });
}