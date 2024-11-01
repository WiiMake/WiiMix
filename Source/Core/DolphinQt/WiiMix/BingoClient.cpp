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
}

bool WiiMixBingoClient::SendData(WiiMixBingoSettings settings, WiiMixEnums::Action action) {
    bool success = false;

    // Connect a player to a lobby
    QJsonDocument connect_json = settings.toJson();
    QJsonObject connect_json_obj = connect_json.object();

    connect_json_obj[QStringLiteral(ACTION)] = static_cast<int>(action);

    // Update the json
    connect_json = QJsonDocument(connect_json_obj);

    // TODOx: error handling
    m_socket->write(QJsonDocument::fromVariant(connect_json).toJson());
    success = m_socket->waitForBytesWritten();
    return success;
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