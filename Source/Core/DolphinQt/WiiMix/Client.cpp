#include "DolphinQt/WiiMix/Client.h"
#include "DolphinQt/QtUtils/ModalMessageBox.h"
#include "DolphinQt/WiiMix/RogueSettings.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInformation>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

#include <assert.h>
#include <algorithm>
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
        m_socket->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 512 * 1024); // Set send buffer size to 512 KB
        m_socket->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 64 * 1024); // Set receive buffer size to 64 KB
        #if defined(WIIMIX_PORT) && defined(WIIMIX_IP)
        // Use the loopback address for network testing
            m_socket->connectToHost(QString::fromStdString(WIIMIX_IP), WIIMIX_PORT);
        #else
            assert(false);
        #endif
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
        // Header format: [number of files]|[json size]|[file 1 size]|[file 2 size]|[file n size]{json}<file 1><file 2><file n>
        int firstDelimiter = data.indexOf('|');
        int secondDelimiter = data.indexOf('|', firstDelimiter + 1);

        if (firstDelimiter == -1 || secondDelimiter == -1) {
            qWarning() << "Invalid message format";
            return;
        }

        int numFiles = data.left(firstDelimiter).toInt();
        
        QByteArray jsonSizeStr = data.mid(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1);

        bool ok1 = false;
        int jsonSize = jsonSizeStr.toInt(&ok1);

        if (!ok1) {
            qWarning() << "Json size is not a number";
            return;
        }

        if (numFiles > 0) {
            int fileSizes[numFiles];
            std::vector<QByteArray> files;

            int currentPos = secondDelimiter + 1;
            for (int i = 0; i < numFiles; ++i) {
                int nextDelimiter = data.indexOf('|', currentPos);
                if (nextDelimiter == -1) {
                    qWarning() << "Invalid message format for file sizes";
                    return;
                }
                QByteArray fileSizeStr = data.mid(currentPos, nextDelimiter - currentPos);
                bool ok2 = false;
                fileSizes[i] = fileSizeStr.toInt(&ok2);
                if (!ok2) {
                    qWarning() << "File size is not a number";
                    return;
                }
                currentPos = nextDelimiter + 1;
            }

            for (int i = 0; i < numFiles; ++i) {
                files.push_back(data.mid(currentPos, fileSizes[i]));
                currentPos += fileSizes[i];
            }

            QByteArray json = data.mid(currentPos, jsonSize);
            if (json.isEmpty()) {
                qWarning() << "Json is empty";
                return;
            }

            ReceiveData(QJsonDocument::fromJson(json), files);
        }
        else {
            QByteArray json = {};
            if (jsonSize > 0) {
                json = data.mid(secondDelimiter + 1, jsonSize);
            }
            if (json.isEmpty()) {
                qWarning() << "Json is empty";
                return;
            }
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
    m_data = {};
    
    if (action == WiiMixEnums::Action::ADD_OBJECTIVE) {
        m_data.append('1');
        m_data.append('|');
    }
    else {
        m_data.append('0');
        m_data.append('|');
    }
    
    // Json size
    m_data.append(QString::number(json.toJson().size()).toUtf8());
    m_data.append('|');

    // File size
    if (action == WiiMixEnums::Action::ADD_OBJECTIVE) {
        // Load the data for the corresponding file
        int slot = obj[QStringLiteral(STATE_SLOT)].toInt() + 1;
        QString game_id = obj[QStringLiteral(OBJECTIVE_GAME_ID)].toString();
        QString buffer = QString::number(slot).rightJustified(2, QLatin1Char('0'));
        QString savestate_path = QString::fromStdString(File::GetUserPath(D_STATESAVES_IDX)) + game_id + QStringLiteral(".s") + buffer;
        QFile file(savestate_path);
        qDebug() << QStringLiteral("Opening file:") << savestate_path;
        if (!file.open(QIODevice::ReadOnly)) {
            qCritical() << "Failed to open file:" << file.errorString();
            return false;
        }
        
        m_data.append(QString::number(file.size()).toUtf8());
        m_data.append('|');
        m_data.append(json.toJson());
        qDebug() << QStringLiteral("Reading file:") << savestate_path;
        m_data.append(file.readAll());
        qDebug() << QStringLiteral("File read successfully");
    }
    else {
        m_data.append(json.toJson());
    }

    qDebug() << m_data.left(20);

    // bool success = false;
    qDebug() << QStringLiteral("Socket state") << m_socket->state();
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << QStringLiteral("Sending data to server");
        m_bytes_written = 0;
        connect(m_socket, &QTcpSocket::bytesWritten, this, &WiiMixClient::BytesWritten, Qt::QueuedConnection);
        BytesWritten();
        // emit writeBytes();
        // Start the first chunk write
        // const qint64 kbSize = 1024;
        // qint64 written = m_socket->write(m_data.mid(m_bytes_written, qMin(64 * kbSize, m_data.size())));
        // if (written == -1) {
        //     qCritical() << "Failed to write data to socket:" << m_socket->errorString();
        //     ModalMessageBox::critical(nullptr, tr("Error"), tr("Failed to write data to socket: %1").arg(m_socket->errorString()));
        //     return false;
        // }
        // qDebug() << QStringLiteral("Bytes to write: ") << m_socket->bytesToWrite();

        // // // Update bytes written
        // m_bytes_written += written;
        // qDebug() << "Initial bytes written:" << m_bytes_written;
        // while (bytesWritten < data.size()) {
        //     qint64 chunkSize = m_socket->write(data.mid(m_bytes_written, 16384)); // Write in 16kb chunks
        //     if (chunkSize == -1) {
        //         qCritical() << "Failed to write data to socket:" << m_socket->errorString();
        //         success = false;
        //         break;
        //     }
        //     bytesWritten += chunkSize;
        //     while (m_socket->bytesToWrite() > 0) {
        //         if (!m_socket->waitForBytesWritten(10000)) {  // 10 seconds timeout
        //             qCritical() << "Failed to send data to server:" << m_socket->errorString();
        //             break;
        //         }
        //     }
        //     // qDebug() << QStringLiteral("Bytes to write: ") << data.size() - bytesWritten;
        //     if (bytesWritten % (16384 * 10) == 0) {
        //         emit onBytesWritten(bytesWritten, data.size());
        //     }
        // success = (m_bytes_written == std::min(m_data.size(), static_cast<qint64>(16384)));
        // if (success) {
        //     qDebug() << "First chunk of data sent successfully";
        // }
        // else {
        //     if (m_socket->error() != QAbstractSocket::UnknownSocketError) {
        //         qDebug() << "Socket error:" << m_socket->errorString();
        //     }
        //     else {
        //         qCritical() << "Socket is not connected. Cannot send data to server.";
        //     }
        // }
    }
    return true;
}

void WiiMixClient::BytesWritten() {
    // Check if all data has been sent
    if (m_bytes_written >= m_data.size()) {
        qDebug() << "All data sent successfully!";
        emit onBytesWritten(m_bytes_written, m_data.size());
        return;
    }

    // Write the next chunk
    const qint64 kbSize = 1024;
    qint64 remaining = m_data.size() - m_bytes_written;
    qint64 written = m_socket->write(m_data.mid(m_bytes_written, qMin(64 * kbSize, remaining)));
    if (written == -1) {
        qCritical() << "Failed to write data to socket:" << m_socket->errorString();
        ModalMessageBox::critical(nullptr, tr("Error"), tr("Failed to write data to socket: %1").arg(m_socket->errorString()));
        return;
    }

    // while (m_socket->bytesToWrite() > 0) {
    //     if (!m_socket->waitForBytesWritten(10000)) {  // 10 seconds timeout
    //         qCritical() << "Failed to send data to server:" << m_socket->errorString();
    //         break;
    //     }
    // }

    // Update the total bytes written
    m_bytes_written += written;
    qDebug() << QStringLiteral("Bytes written: ") << m_bytes_written << "/" << m_data.size();

    // Notify progress
    if (m_bytes_written % (64 * kbSize * 10) == 0) {
        emit onBytesWritten(m_bytes_written, m_data.size());
    }
}

bool WiiMixClient::ReceiveData(QJsonDocument json) {
    WiiMixEnums::Response response = static_cast<WiiMixEnums::Response>(json.object()[QStringLiteral(CLIENT_RESPONSE)].toInt());

    if (response == WiiMixEnums::Response::UPDATE_BINGO_CONFIG) {
        // Update the settings
        WiiMixBingoSettings::instance()->FromJson(json);
        // Update the UI
        qDebug() << QStringLiteral("Emitting onUpdateBingoConfig");
        emit onUpdateBingoConfig(WiiMixBingoSettings::instance());
    }
    else if (response == WiiMixEnums::Response::GET_PLAYERS) {
        // TODOx: not implemented
        emit onGetPlayers({});
    }
    else if (response == WiiMixEnums::Response::GET_PLAYER) {
        QJsonObject obj = json.object();
        WiiMixPlayer player = WiiMixPlayer::FromJson(obj);
        emit onGetPlayer(player);
    }
    else if (response == WiiMixEnums::Response::GET_OBJECTIVE_HISTORY) {
        // TODOx: not implemented
        emit onGetObjectiveHistory({});
    }
    else if (response == WiiMixEnums::Response::GET_OBJECTIVES) {
        // Iterate over each json objective and create an array of WiiMixObjectives
        std::vector<WiiMixObjective> objectives;
        for (int i = 0; i < json.array().size(); ++i) {
            QJsonObject obj = json.array()[i].toObject();
            WiiMixObjective objective = WiiMixObjective::FromJson(obj);
            objectives.push_back(objective);
        }
        emit onGetObjectives(objectives);
    }
    return true;
}

bool WiiMixClient::ReceiveData(QJsonDocument json, std::vector<QByteArray> files) {
    WiiMixEnums::Response response = static_cast<WiiMixEnums::Response>(json.object()[QStringLiteral(CLIENT_RESPONSE)].toInt());

    if (response == WiiMixEnums::Response::UPDATE_BINGO_OBJECTIVES) {
        // For each objective in the json array, convert it to a wiimix objective
        // and then save the savestate to the correct location [path] + id + ".sav"
        std::string savestate_path = File::GetUserPath(D_WIIMIX_STATESAVES_IDX);
        std::vector<WiiMixObjective> objectives;
        for (int i = 0; i < json.array().size(); ++i) {
            QJsonObject obj = json.array()[i].toObject();
            WiiMixObjective objective = WiiMixObjective::FromJson(obj);
            // Save the savestate
            QFile file(QString::fromStdString(savestate_path + std::to_string(objective.GetId()) + ".sav"));
            if (file.open(QIODevice::WriteOnly)) {
                file.write(files[i]);
                file.close();
            }
            objectives.push_back(objective);
        }
        WiiMixBingoSettings::instance()->SetObjectives(objectives);
        emit onUpdateBingoObjectives(WiiMixBingoSettings::instance());
    }
    else if (response == WiiMixEnums::Response::UPDATE_ROGUE_OBJECTIVES) {
        // For each objective in the json array, convert it to a wiimix objective
        // and then save the savestate to the correct location [path] + id + ".sav"
        std::string savestate_path = File::GetUserPath(D_WIIMIX_STATESAVES_IDX);
        std::vector<WiiMixObjective> objectives;
        for (int i = 0; i < json.array().size(); ++i) {
            QJsonObject obj = json.array()[i].toObject();
            WiiMixObjective objective = WiiMixObjective::FromJson(obj);
            // Save the savestate
            QFile file(QString::fromStdString(savestate_path + std::to_string(objective.GetId()) + ".sav"));
            if (file.open(QIODevice::WriteOnly)) {
                file.write(files[i]);
                file.close();
            }
            objectives.push_back(objective);
        }
        WiiMixRogueSettings::instance()->SetObjectives(objectives);
        emit onUpdateRogueObjectives(WiiMixRogueSettings::instance());
    }
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

QTcpSocket* WiiMixClient::GetSocket() {
    return m_socket;
}