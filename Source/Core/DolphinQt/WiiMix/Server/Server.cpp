#include "DolphinQt/WiiMix/Server/Server.h"

#include <QCoreApplication>

#include <QString>
#include <QPair>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Server/BingoSettings.h"

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
    int player_index = m_lobbies[lobby].first.values().indexOf(client_socket);
    m_lobbies[lobby].first.remove(client_socket);
    // Remove the player from the ui
    WiiMixEnums::Player player = static_cast<WiiMixEnums::Player>(player_index);
    WiiMixBingoSettings settings = m_lobbies[lobby].second;
    settings.RemovePlayer(player);
    SendToLobby(lobby, settings.ToJson().object());
  }

  client_socket->deleteLater();
  qDebug() << "Client disconnected and removed from lobby" << lobby;
}

void WiiMixServer::Read() {
  qDebug() << "Reading data from client";
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

void WiiMixServer::ProcessMessage(QTcpSocket* client_socket, QJsonObject json) {
  qDebug() << "Processing message from client";
  WiiMixEnums::Action action = static_cast<WiiMixEnums::Action>(json.value(ACTION).toInt());
  qDebug() << "Action: " << static_cast<int>(action);
  QString lobby = json[QStringLiteral(BINGO_NETPLAY_SETTINGS_LOBBY_ID)].toString();

  if (action == WiiMixEnums::Action::UPDATE) {
    qDebug() << "Updating settings for lobby" << lobby;
    // Update settings
    m_lobbies[lobby].second = m_lobbies[lobby].second.FromJson(QJsonDocument(json));

    // Check for ready (hardcoded for burst)
    if (m_lobbies[lobby].second.GetPlayersReady().value(WiiMixEnums::Player::ONE) &&
        m_lobbies[lobby].second.GetPlayersReady().value(WiiMixEnums::Player::TWO)) {
      // populate objectives if they don't exist
      if (json[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)].toArray().size() == 0) {
        std::vector<WiiMixObjective> objectives = WiiMixObjective::GetObjectives();
        QJsonArray selected_objectives;
        std::random_shuffle(objectives.begin(), objectives.end());
        for (int i = 0; i < 9; i++) {
          selected_objectives.append(objectives[i].ToJson().object());
        }
        qDebug() << selected_objectives.size();
        // qDebug() << selected_objectives;
        json[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)] = selected_objectives;
        qDebug() << "Final JSON object:" << QJsonDocument(json).toJson(QJsonDocument::Indented);
      }
      // else {
      //   qDebug() << "objective Array size";
      //   qDebug() << json[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)].toArray().size();
      //   qDebug() << json[QStringLiteral(COMMON_NETPLAY_SETTINGS_OBJECTIVES)].toArray();
      // }
    }
    
    // Propagate the settings to all clients in the lobby
    SendToLobby(lobby, json);
  }
  else if (action == WiiMixEnums::Action::CREATE_LOBBY) {
    qDebug() << "Creating lobby" << lobby;
    // This should also double as a connect + update if the lobby is already created
    if (!m_lobbies.contains(lobby)) {
      m_lobbies[lobby].first = QSet<QTcpSocket*>();
      qDebug() << "Before Fromjson";
      m_lobbies[lobby].second = WiiMixBingoSettings::FromJson(QJsonDocument(json));
      qDebug() << "After Fromjson";
      m_lobbies[lobby].first.insert(client_socket);
      m_client_lobbies[client_socket] = lobby;
      qDebug() << "Before ToJson";
      SendToLobby(lobby, m_lobbies[lobby].second.ToJson().object());
    }
    else {
      // Connect otherwise
      qDebug() << "Connecting to lobby" << lobby;
      m_lobbies[lobby].first.insert(client_socket);
      m_client_lobbies[client_socket] = lobby;
      // Update the list of players
      // and propagate the current settings to all clients in the lobby
      QString playerName = json[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS)][QStringLiteral("0")][QStringLiteral(BINGO_NETPLAY_SETTINGS_NAME)].toString();
      QPair<WiiMixEnums::Color, QString> player_pair(WiiMixEnums::PlayerToColor(static_cast<WiiMixEnums::Player>(m_lobbies[lobby].second.GetPlayers().size())), 
        playerName);
      qDebug() << "playerName" << playerName;
      m_lobbies[lobby].second.AddPlayer(WiiMixEnums::Player::TWO, 
        player_pair);
      SendToLobby(lobby, m_lobbies[lobby].second.ToJson().object());
    }
  }
  else if (action == WiiMixEnums::Action::CONNECT) {
    qDebug() << "Connecting to lobby" << lobby;
    // CREATE_LOBBY should be called for hosts; connect for extra connections
    // if (!m_lobbies.contains(lobby)) {
        // m_lobbies[lobby] = QSet<QTcpSocket*>();
    // }
    m_lobbies[lobby].first.insert(client_socket);
    m_client_lobbies[client_socket] = lobby;
    // Update the list of players
    // and propagate the current settings to all clients in the lobby
    QString playerName = json[QStringLiteral(BINGO_NETPLAY_SETTINGS_PLAYERS)][QStringLiteral("0")][QStringLiteral(BINGO_NETPLAY_SETTINGS_NAME)].toString();
    QPair<WiiMixEnums::Color, QString> player_pair(WiiMixEnums::PlayerToColor(static_cast<WiiMixEnums::Player>(m_lobbies[lobby].second.GetPlayers().size())), 
      playerName);
    qDebug() << "playerName" << playerName;
    m_lobbies[lobby].second.AddPlayer(WiiMixEnums::Player::TWO, 
      player_pair);
    SendToLobby(lobby, m_lobbies[lobby].second.ToJson().object());
  }
}

void WiiMixServer::SendToLobby(const QString& lobby, const QJsonObject& message) {
    if (!m_lobbies.contains(lobby)) return;

    QJsonDocument doc(message);
    qDebug() << "Sending doc: " << doc;
    QByteArray data = doc.toJson();
    int success = 0;
    qDebug() << "Sending data to lobby" << lobby;
    if (!m_lobbies.contains(lobby)) {
        qCritical() << "Lobby does not exist";
        return;
    }
    if (m_lobbies[lobby].first.size() == 0) {
        qCritical() << "No clients in lobby";
        return;
    }
    qDebug() << "# of connected clients: " << m_lobbies[lobby].first.size();

    for (QTcpSocket* client : m_lobbies[lobby].first) {
      if (client->state() == QAbstractSocket::ConnectedState) {
          qDebug() << QStringLiteral("Sending data to client");
          client->write(data);
          success = client->waitForBytesWritten();
          if (!success) {
              qCritical() << "Failed to send data to client:" << client->errorString();
          }
          else {
              qDebug() << "Data sent successfully";
          }
      }
      else {
          if (client->error() != QAbstractSocket::UnknownSocketError) {
              qDebug() << "Socket error:" << client->errorString();
          }
          else {
              qCritical() << "Socket is not connected. Cannot send data to client.";
          }
      }
    }
}