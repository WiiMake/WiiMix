#include "DolphinQt/WiiMix/BingoClient.h"

WiiMixBingoClient::WiiMixBingoClient(QObject *parent) : QObject(parent) {}

void WiiMixBingoClient::Connect(QString lobby_name, QString player_name, QString lobby_password) {

}

void WiiMixBingoClient::UpdateBingoCard(int objective, WiiMixEnums::Player player) {

}

void WiiMixBingoClient::BingoEnd() {

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