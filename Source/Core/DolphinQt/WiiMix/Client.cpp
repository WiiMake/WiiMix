#include "DolphinQt/WiiMix/Client.h"
#include "DolphinQt/QtUtils/ModalMessageBox.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInformation>
#include <QFile>

#include <assert.h>
#include <Common/FileUtil.h>

WiiMixClient::WiiMixClient(QObject *parent, QTcpSocket *socket) : QObject(parent), m_socket(socket) {}

// bool WiiMixClient::HasNetworkConnection() {
//     auto networkInfo = QNetworkInformation::instance();
//     QNetworkInformation::Reachability reachability = networkInfo->reachability();
//     if (reachability != QNetworkInformation::Reachability::Online) {
//         qCritical() << "No internet connection available!";
//         return false; // No internet connection available, return early
//     }
//     return true;
// }

bool WiiMixClient::ConnectToServer() {
    if (m_socket == nullptr) {
        m_socket = new QTcpSocket(this);
        assert (PORT != -1);
        assert (IP != "");
        // Use the loopback address for network testing
        m_socket->connectToHost(QString::fromStdString(IP), PORT);
        qDebug() << QStringLiteral("Connecting client to server");
        if (!m_socket->waitForConnected(3000)) {  // 3-second timeout
            qCritical() << "Failed to connect to server:" << m_socket->errorString();
            // Emit a signal to the UI to display an error message
            emit onError(QStringLiteral("Could not connect to the WiiMix server."));
            return false;
        }
        else {
            qDebug() << m_socket->state();
        }
    }
    connect(m_socket, &QTcpSocket::readyRead, this, [this]() {
        qDebug() << QStringLiteral("Data received from server");
        QByteArray data = m_socket->readAll();

        // Split header and content (json and file)
        // Header format: [json size]|[file size]|{json}<file>
        int firstDelimiter = data.indexOf('|');
        int secondDelimiter = data.indexOf('|', firstDelimiter + 1);

        if (firstDelimiter == -1 || secondDelimiter == -1) {
            qWarning() << "Invalid message format";
            return;
        }
        
        QByteArray jsonSizeStr = data.left(firstDelimiter);
        QByteArray fileSizeStr = data.mid(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1);

        bool ok1 = false, ok2 = false;
        int jsonSize = jsonSizeStr.toInt(&ok1);
        int fileSize = fileSizeStr.toInt(&ok2);

        if (!ok1 || !ok2) {
            qWarning() << "Json or file size is not a number";
            return;
        }

        if (jsonSize + fileSize + secondDelimiter + 1 != data.size()) {
            qWarning() << "Invalid message size";
            return;
        }

        QByteArray json = {};
        if (jsonSize > 0) {
            json = data.mid(secondDelimiter + 1, jsonSize);
        }

        QByteArray file = {};
        if (fileSize > 0) {
            file = data.mid(secondDelimiter + 1 + jsonSize, fileSize);
        }
        
        // Process the message according to the data sent
        if (!json.isEmpty() && !file.isEmpty()) {
            ReceiveData(QJsonDocument::fromJson(json), file);
        }
        else {
            ReceiveData(QJsonDocument::fromJson(json));
        }
    });

    return true;
}

bool WiiMixClient::IsConnected() const {
    if (m_socket == nullptr) {
        return false;
    }
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

bool WiiMixClient::SendData(QJsonObject obj, WiiMixEnums::Action action) {
    obj[QStringLiteral(SERVER_ACTION)] = static_cast<int>(action);
    
    QJsonDocument json(obj);
    QByteArray data = {};
    // Json size
    data.append(static_cast<int>(json.toJson().size()));
    data.append('|');

    // File size
    if (action == WiiMixEnums::Action::ADD_OBJECTIVE) {
        // Load the data for the corresponding file
        int slot = obj[QStringLiteral("slot")].toInt();
        QString game_id = obj[QStringLiteral("game_id")].toString();
        char buffer[4];
        sprintf(buffer, "%02d", slot);
        QString savestate_path = QString::fromStdString(File::GetUserPath(D_STATESAVES_IDX)) + game_id + buffer + ".sav";
        QFile file(savestate_path);
        
        data.append(static_cast<int>(file.size()));
        data.append('|');
        data.append(json.toJson());
        data.append(file.readAll());
    }
    else {
        data.append('0');
        data.append('|');
        data.append(json.toJson());
    }

    bool success = false;
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << QStringLiteral("Sending data to server");
        qDebug() << json.toJson();
        m_socket->write(json.toJson());
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

bool WiiMixClient::ReceiveData(QJsonDocument json) {
    WiiMixEnums::Action action = static_cast<WiiMixEnums::Action>(json.object()[QStringLiteral(SERVER_ACTION)].toInt());

    if (action == WiiMixEnums::Action::CREATE_BINGO_LOBBY || action == WiiMixEnums::Action::UPDATE_BINGO_LOBBY || action == WiiMixEnums::Action::CONNECT_TO_BINGO_LOBBY) {
        // Update the settings
        WiiMixBingoSettings::instance()->FromJson(json);
        // Update the UI
        qDebug() << QStringLiteral("Emitting onSettingsChanged");
        emit onSettingsChanged(WiiMixBingoSettings::instance());
    }
    else if (action == WiiMixEnums::Action::GET_OBJECTIVE_DB) {
        emit onGetObjectiveDBReceived(json);
    }
    else if (action == WiiMixEnums::Action::GET_PLAYERS) {
        emit onGetPlayersReceived(json);
    } else if (action == WiiMixEnums::Action::GET_OBJECTIVE_HISTORY) {
        emit onObjectiveHistoryReceived(json);
    } else if (action == WiiMixEnums::Action::GET_COMPLETED_OBJECTIVES_BY_PLAYER) {
        emit onCompletedObjectivesReceived(json);
    } else if (action == WiiMixEnums::Action::GET_FAILED_OBJECTIVES_BY_PLAYER) {
        emit onFailedObjectivesReceived(json);
    } else if (action == WiiMixEnums::Action::GET_OBJECTIVE_BY_ID) {
        emit onObjectiveByIDReceived(json);
    } else if (action == WiiMixEnums::Action::GET_OBJECTIVES_BY_GAME_ID) {

    } else if (action == WiiMixEnums::Action::GET_OBJECTIVES_BY_RETROACHIEVEMENTS_GAME_ID) {

    } else if (action == WiiMixEnums::Action::GET_OBJECTIVES_BY_ACHIEVEMENT_ID) {

    } else if (action == WiiMixEnums::Action::GET_OBJECTIVES_BY_PLAYER_ID) {

    } else if (action == WiiMixEnums::Action::GET_OBJECTIVES_BY_GENRE) {

    } else if (action == WiiMixEnums::Action::GET_OBJECTIVES_BY_TYPE) {

    } else if (action == WiiMixEnums::Action::GET_OBJECTIVES_BY_DIFFICULTY) {

    } else if (action == WiiMixEnums::Action::GET_OBJECTIVES_BY_TIME) {

    } else if (action == WiiMixEnums::Action::GET_OBJECTIVES_BY_CREATOR) {

    }
    return true;
}

bool WiiMixClient::ReceiveData(QJsonDocument json, QByteArray file) {
    WiiMixEnums::Action action = static_cast<WiiMixEnums::Action>(json.object()[QStringLiteral(SERVER_ACTION)].toInt());
    if (action == ) {

    }
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
