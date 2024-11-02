#include "DolphinQt/WiiMix/BingoClient.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

WiiMixBingoClient::WiiMixBingoClient(QObject *parent, QTcpSocket *socket) : QObject(parent), m_socket(socket) {
    // TODOx: Using loopback address for now
    // Should theoretically be able to get 2 instances of WiiMix communicating on the same machine
    if (m_socket == nullptr) {
        m_socket = new QTcpSocket(this);
        m_socket->connectToHost(QStringLiteral("localhost"), PORT);
    }
    connect(m_socket, &QTcpSocket::readyRead, this, [this]() {
        QByteArray data = m_socket->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isObject()) return;
        ReceiveData(doc);
    });
}

bool WiiMixBingoClient::SendData(WiiMixBingoSettings settings, WiiMixEnums::Action action) {
    bool success = false;

    // Connect a player to a lobby
    QJsonDocument connect_json = settings.ToJson();
    QJsonObject connect_json_obj = connect_json.object();

    connect_json_obj[QStringLiteral(ACTION)] = static_cast<int>(action);

    // Update the json
    connect_json = QJsonDocument(connect_json_obj);

    // TODOx: error handling
    m_socket->write(QJsonDocument::fromVariant(connect_json).toJson());
    success = m_socket->waitForBytesWritten();
    return success;
}

bool WiiMixBingoClient::ReceiveData(QJsonDocument doc) {
    // Update the settings
    WiiMixBingoSettings bingo_settings = WiiMixBingoSettings(); 
    bingo_settings = bingo_settings.FromJson(doc);
    // Update the UI
    emit onSettingsChanged(bingo_settings);
    return true;
}

// Getters (retrieves values from server)
// WiiMixEnums::BingoType WiiMixBingoClient::GetBingoType() const { 
//     return WiiMixEnums::BingoType();
// }

// std::list<WiiMixEnums::Player> WiiMixBingoClient::GetPlayers() const { 
//     return std::list<WiiMixEnums::Player>();
// }

// std::list<WiiMixObjective> WiiMixBingoClient::GetBingoCard() const { 
//     return std::list<WiiMixObjective>(); 
// }