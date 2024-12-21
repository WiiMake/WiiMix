#include "DolphinQt/WiiMix/Client.h"
#include "DolphinQt/QtUtils/ModalMessageBox.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include <assert.h>

WiiMixClient::WiiMixClient(QObject *parent, QTcpSocket *socket) : QObject(parent), m_socket(socket) {}

bool WiiMixClient::ConnectToServer() {
    // Use the loopback address for network testing
    if (m_socket == nullptr) {
        m_socket = new QTcpSocket(this);
        m_socket->connectToHost(QStringLiteral("localhost"), PORT);
        qDebug() << QStringLiteral("Connecting client to server");
        if (!m_socket->waitForConnected(3000)) {  // 3-second timeout
            qCritical() << "Failed to connect to server:" << m_socket->errorString();
            // Emit a signal to the UI to display an error message
            emit onError(QStringLiteral("Could not connect to the WiiMix server."));
        }
        else {
            qDebug() << m_socket->state();
        }
    }
    connect(m_socket, &QTcpSocket::readyRead, this, [this]() {
        qDebug() << QStringLiteral("Data received from server");
        QByteArray data = m_socket->readAll();
        qDebug() << data;
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) return;
        ReceiveData(doc);
    });
}

bool WiiMixClient::SendData(WiiMixBingoSettings* settings, WiiMixEnums::Action action) {
    // For some reason m_socket here is erroring
    assert(m_socket != nullptr);
    // This statement would print otherwise
    qDebug() << QStringLiteral("Sending data to server; m_socket issue?");
    bool success = false;

    // Connect a player to a lobby
    QJsonDocument connect_json = settings->ToJson();
    // connect_json[QStringLiteral(ACTION)] = static_cast<int>(action);
    QJsonObject connect_json_obj = connect_json.object();

    connect_json_obj[QStringLiteral(ACTION)] = static_cast<int>(action);

    qDebug() << connect_json;
    // qDebug() << connect_json_obj;

    // Update the json
    connect_json = QJsonDocument(connect_json_obj);

    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << QStringLiteral("Sending data to server");
        qDebug() << connect_json.toJson();
        m_socket->write(connect_json.toJson());
        success = m_socket->waitForBytesWritten();
        if (!success) {
            qCritical() << "Failed to send data to server:" << m_socket->errorString();
        }
        else {
            qDebug() << "Data sent successfully";
        }
    }
    else {
        if (m_socket->error() != QAbstractSocket::UnknownSocketError) {
            qDebug() << "Socket error:" << m_socket->errorString();
        }
        else {
            qCritical() << "Socket is not connected. Cannot send data to server.";
        }
    }
    return success;
}

bool WiiMixClient::ReceiveData(QJsonDocument doc) {
    // Update the settings
    WiiMixBingoSettings::instance()->FromJson(doc);
    // Update the UI
    qDebug() << QStringLiteral("Emitting onSettingsChanged");
    emit onSettingsChanged(WiiMixBingoSettings::instance());
    return true;
}

// Getters (retrieves values from server)
// WiiMixEnums::BingoType WiiMixClient::GetBingoType() const {
//     return WiiMixEnums::BingoType();
// }

// std::list<WiiMixEnums::Player> WiiMixClient::GetPlayers() const {
//     return std::list<WiiMixEnums::Player>();
// }

// std::list<WiiMixObjective> WiiMixClient::GetBingoCard() const {
//     return std::list<WiiMixObjective>();
// }
