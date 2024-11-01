#include "DolphinQt/WiiMix/Server.h"

int main(int argc, char* argv[])
{
  return 0;
}

WiiMixServer::WiiMixServer(QObject *parent, WiiMixEnums::BingoType bingo_type, 
  std::list<WiiMixEnums::Player> players, std::list<WiiMixObjective> bingo_card) : 
    QObject(parent), m_bingo_type(bingo_type), m_players(players), m_bingo_card(bingo_card) {
      m_server = new QTcpServer(this);
      connect(m_server, SIGNAL(newConnection()), this, SLOT(NewConnection()));

      // if (!m_server->listen(QHostAddress::Any, PORT)) {
      //   // Could not start the server; throw an exception
      //   throw std::runtime_error("Server could not start!");
      // } else {
      //   qDebug() << "Server started!";
      // }
    };

void WiiMixServer::NewConnection() {
  QTcpSocket *socket = m_server->nextPendingConnection();
  socket->waitForReadyRead(3000);

  // Read lobby name and an optional lobby password from the socket
  // Rejecting any connection that doesn't send the correct information
  QString lobby_id = QString::fromUtf8(socket->read(LOBBY_ID_LENGTH));
  QString lobby_password = QString::fromUtf8(socket->read(MAX_LOBBY_PASSWORD_LENGTH));

  // socket->write("hello client\r\n");
  // socket->flush();
  
  // socket->waitForBytesWritten(3000);

  // socket->close();
}

void WiiMixServer::CreateLobby(QString lobby_name, QString lobby_password) {}

// Bingo card objectives are read top to bottom left to right and are 0 indexed
void WiiMixServer::SetBingoType(WiiMixEnums::BingoType bingo_type) {}
WiiMixEnums::BingoType WiiMixServer::GetBingoType() const { 
  return WiiMixEnums::BingoType();
}

void WiiMixServer::SetPlayers(const std::list<WiiMixEnums::Player>& players) {

}

void WiiMixServer::AddPlayer(WiiMixEnums::Player player) {

}

void WiiMixServer::RemovePlayer(WiiMixEnums::Player player) {

}

std::list<WiiMixEnums::Player> WiiMixServer::GetPlayers() const { 
  return std::list<WiiMixEnums::Player>(); 
}

void WiiMixServer::SetBingoCard(const std::list<WiiMixObjective>& bingo_card) {

}

void WiiMixServer::UpdateBingoCard(int objective, WiiMixEnums::Player player) {

}

std::list<WiiMixObjective> WiiMixServer::GetBingoCard() const { 
  return std::list<WiiMixObjective>(); 
}