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

bool WiiMixBingoClient::Connect(QString lobby_name, QString player_name, QString lobby_password) {
    bool success = false;

    // Connect a player to a lobby
    QJsonObject connect_json;

    connect_json[QStringLiteral(ACTION)] = QStringLiteral(CONNECT);
    connect_json[QStringLiteral(LOBBY_NAME)] = lobby_name;
    connect_json[QStringLiteral(PLAYER_NAME)] = player_name;
    connect_json[QStringLiteral(LOBBY_PASSWORD)] = lobby_password;

    // TODOx: error handling
    m_socket->write(QJsonDocument::fromVariant(connect_json).toJson());
    success = m_socket->waitForBytesWritten();
    return success;
}

bool WiiMixBingoClient::UpdateBingoCard(int objective, WiiMixEnums::Player player) {
    bool success = false;
    // Update the bingo card on the server
    QJsonObject update_bingo_card_json;
    update_bingo_card_json[QStringLiteral(ACTION)] = QStringLiteral(UPDATE_BINGO_CARD);
    update_bingo_card_json[QStringLiteral(OBJECTIVE)] = objective;
    update_bingo_card_json[QStringLiteral(PLAYER)] = static_cast<int>(player);

    // TODOx: error handling
    m_socket->write(QJsonDocument(update_bingo_card_json).toJson());
    success = m_socket->waitForBytesWritten();
    return success;
}

bool WiiMixBingoClient::CreateLobby(QString lobby_name, QString player_name, QString lobby_password) {
    bool success = false;

    // Sends a request to the server to create the lobby
    QJsonObject create_lobby_json;
    create_lobby_json[QStringLiteral(ACTION)] = QStringLiteral(CREATE_LOBBY);
    create_lobby_json[QStringLiteral(LOBBY_NAME)] = lobby_name;
    create_lobby_json[QStringLiteral(PLAYER_NAME)] = player_name;
    create_lobby_json[QStringLiteral(LOBBY_PASSWORD)] = lobby_password;

    m_socket->write(QJsonDocument(create_lobby_json).toJson());
    // TODOx: error handling
    success = m_socket->waitForBytesWritten();
    if (!success) {
        return false;
    }

    // Connect the player to the lobby
    success = Connect(lobby_name, player_name, lobby_password);
    return success;
}

bool WiiMixBingoClient::BingoEnd() {
    bool success = false;

    // Ends the bingo session
    QJsonObject bingo_end_json;
    bingo_end_json[QStringLiteral(ACTION)] = QStringLiteral(BINGO_END);
    m_socket->write(QJsonDocument(bingo_end_json).toJson());
    // TODOx: error handling
    success = m_socket->waitForBytesWritten();

    return success;
}

// Getters (from server)
WiiMixEnums::BingoType WiiMixBingoClient::GetBingoType() const { 
    return WiiMixEnums::BingoType();
}

std::list<WiiMixEnums::Player> WiiMixBingoClient::GetPlayers() const { 
    return std::list<WiiMixEnums::Player>();
}

std::list<WiiMixObjective> WiiMixBingoClient::GetBingoCard() const { 
    return std::list<WiiMixObjective>(); 
}