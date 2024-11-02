#include "DolphinQt/WiiMix/Server.h"

#include <QCoreApplication>

#include <QString>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/BingoSettings.h"

// NOTE: There's probably potential here for code generation, I just wasn't sure how to do it


// Server entry point
int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    // Create an instance of your server
    WiiMixServer server = WiiMixServer();

    // Start the event loop
    return app.exec();
}

WiiMixServer::WiiMixServer(QObject *parent)
{
  m_server = new QTcpServer(this);
  
  connect(m_server, SIGNAL(newConnection()), this, SLOT(NewConnection()));

  if (!m_server->listen(QHostAddress::Any, PORT)) {
    // Could not start the server
    // TODOx: Should probably be throwing an exception for this
    // But for now no exception handling is set up
    qWarning() << "Could not start the server";
  } else {
    qDebug() << "Server started!";
  }
};

void WiiMixServer::NewConnection() {
    QTcpSocket *client_socket = m_server->nextPendingConnection();
    // Connect signals to slots
    connect(client_socket, &QTcpSocket::readyRead, this, &WiiMixServer::Read);
    connect(client_socket, &QTcpSocket::disconnected, this, &WiiMixServer::Disconnected);

    qDebug() << "New client connected:" << client_socket->peerAddress().toString();
}

void WiiMixServer::Disconnected() {
  QTcpSocket *client_socket = qobject_cast<QTcpSocket*>(sender());
  if (!client_socket) return;

  QString lobby = m_client_lobbies.value(client_socket);
  // Remove from client list
  m_client_lobbies.remove(client_socket);
  if (!m_lobbies[lobby].first.size()) {
    // Remove entire lobby
    m_lobbies.remove(lobby);
  }
  else {
    // Remove player from lobby
    m_lobbies[lobby].first.remove(client_socket);
  }

  client_socket->deleteLater();
  qDebug() << "Client disconnected and removed from lobby" << lobby;
}

void WiiMixServer::Read() {
  QTcpSocket *client_socket = qobject_cast<QTcpSocket*>(sender());
  if (!client_socket) return;

  QByteArray data = client_socket->readAll();
  QJsonDocument doc = QJsonDocument::fromJson(data);
  if (!doc.isObject()) return;

  qDebug() << "Received doc: " << doc;

  // THIS CODE SHOULDN'T NEED TO RUN Add the client to the lobby if not already in it
  // QString lobby = doc[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)].toString();
  // if (!m_lobbies.contains(lobby)) {
  //     m_lobbies.insert(lobby, QSet<QTcpSocket*>());
  // }
  // m_client_lobbies.insert(client_socket, lobby);

  QJsonObject json = doc.object();
  ProcessMessage(client_socket, json);
}

void WiiMixServer::ProcessMessage(QTcpSocket* client_socket, const QJsonObject& json) {
  WiiMixEnums::Action action = static_cast<WiiMixEnums::Action>(json.value(ACTION).toInt());
  QString lobby = json[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)].toString();

  if (action == WiiMixEnums::Action::UPDATE) {
    // Update settings
    m_lobbies[lobby].second = m_lobbies[lobby].second.FromJson(QJsonDocument(json));
    
    // Propagate the settings to all clients in the lobby
    SendToLobby(lobby, json);
    qDebug() << "Client updated lobby:" << lobby;
  }
  else if (action == WiiMixEnums::Action::CREATE_LOBBY) {
    if (!m_lobbies.contains(lobby)) {
        m_lobbies[lobby].first = QSet<QTcpSocket*>();
    }
    m_lobbies[lobby].second = WiiMixBingoSettings().FromJson(QJsonDocument(json));
    m_lobbies[lobby].first.insert(client_socket);
    m_client_lobbies[client_socket] = lobby;
  }
  else if (action == WiiMixEnums::Action::CONNECT) {
    // CREATE_LOBBY should be called for hosts; connect for extra connections
    // if (!m_lobbies.contains(lobby)) {
        // m_lobbies[lobby] = QSet<QTcpSocket*>();
    // }
    m_lobbies[lobby].first.insert(client_socket);
    m_client_lobbies[client_socket] = lobby;
  }
}

void WiiMixServer::SendToLobby(const QString& lobby, const QJsonObject& message) {
    if (!m_lobbies.contains(lobby)) return;

    QJsonDocument doc(message);
    qDebug() << "Sending doc: " << doc;
    QByteArray data = doc.toJson();
    qDebug() << "Equivalent data: " << data;

    for (QTcpSocket* client : m_lobbies[lobby].first) {
        client->write(data);
    }
}