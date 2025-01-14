#include "DolphinQt/WiiMix/Client.h"
#include "DolphinQt/QtUtils/ModalMessageBox.h"
#include "DolphinQt/WiiMix/RogueSettings.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInformation>
#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

#include <assert.h>
#include <algorithm>
#include <vector>
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
    connect(m_socket, &QTcpSocket::disconnected, this, [this]() {
        qWarning() << "Disconnected from server";
        // Try to reconnect
        #if defined(WIIMIX_PORT) && defined(WIIMIX_IP)
        // Use the loopback address for network testing
            m_socket->connectToHost(QString::fromStdString(WIIMIX_IP), WIIMIX_PORT);
        #else
            assert(false);
        #endif
        qDebug() << QStringLiteral("Attempting to reconnect client to server");
        if (!m_socket->waitForConnected(3000)) {  // 3-second timeout
            qCritical() << "Failed to connect to server:" << m_socket->errorString();
            // Emit a signal to the UI to display an error message
            emit onError(QStringLiteral("Could not connect to the WiiMix server; WiiMix functionality will not work unless you are connected."));
            return;
        }
        else {
            qDebug() << m_socket->state();
        }
    }, Qt::UniqueConnection);
    connect(m_socket, &QTcpSocket::readyRead, this, [this]() {
        QByteArray data = m_socket->readAll();

        // The problem is the client resources are not consistent
        // If the client wants to go endless, we can't just load all of the objectives into memory
        // What we need to do instead is send the first objective, run the game, and then send the next objective
        // Once an objective is completed, it can be removed from memory
        // Or alternatively once a file is completely read in, it can be saved and then deleted
        // This is a more efficient way, as it gets read in anyway

        // Split header and content (json and file)
        // Header format: [number of files]|[json size]|[file 1 size]|[file 2 size]|[file n size]{json}<file 1><file 2><file n>
        // This initializes the number of read bytes, as well as the significant delimiters
        // No files are read in this initial reading for convenience
        int current_pos = 0;
        if (m_json_buffer.isEmpty() && m_json.isEmpty()) {
            int firstDelimiter = data.indexOf('|');
            int secondDelimiter = data.indexOf('|', firstDelimiter + 1);

            if (firstDelimiter == -1 || secondDelimiter == -1) {
                qWarning() << "Invalid message format";
                assert(false);
                return;
            }
            else {
                qDebug() << "Parsing initial data";
                qDebug() << data.left(100);
            }

            bool ok0 = false;
            int numFiles = data.left(firstDelimiter).toInt(&ok0);
            if (!ok0) {
                qWarning() << "Number of files is not a number";
                return;
            }
            
            QByteArray jsonSizeStr = data.mid(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1);

            bool ok1 = false;
            int jsonSize = jsonSizeStr.toInt(&ok1);

            if (!ok1) {
                qWarning() << "Json size is not a number";
                return;
            }
            m_json_size = jsonSize;
            m_data_size = jsonSize;

            if (numFiles > 0) {
                qDebug() << "Starting to read in files";
                current_pos = secondDelimiter + 1;
                for (int i = 0; i < numFiles; ++i) {
                    int nextDelimiter = data.indexOf('|', current_pos);
                    if (nextDelimiter == -1) {
                        qWarning() << "Invalid message format for file sizes";
                        return;
                    }
                    QByteArray fileSizeStr = data.mid(current_pos, nextDelimiter - current_pos);
                    bool ok2 = false;
                    int fileSize = fileSizeStr.toInt(&ok2);
                    if (!ok2) {
                        qWarning() << "File size is not a number";
                        return;
                    }
                    m_file_sizes.append(fileSize);
                    m_data_size += fileSize;
                    current_pos = nextDelimiter + 1;
                }
            }

            // Read in as much of the json as possible
            qDebug() << "starting to read in json";
            m_json_buffer = data.mid(current_pos, m_json_size);

            // If all the json was read in
            // There's something off, but I'm not sure what it is
            if (m_json_buffer.size() == m_json_size) {
                m_json = QJsonDocument::fromJson(m_json_buffer);
                // Check if the data sent is WiiMixObjectives by checking if the json contains an achievement id
                // As achievement ids should only be paired with objectives
                for (int i = 0; i < m_json.array().size(); ++i) {
                    QJsonObject obj = m_json.array()[i].toObject();
                    if (obj.contains(QStringLiteral(OBJECTIVE_ACHIEVEMENT_ID))) {
                        WiiMixObjective objective = WiiMixObjective::FromJson(obj);
                        m_objectives.push_back(objective);
                    }
                    else {
                        break;
                    }
                }
            }
            // Increment by the amount read into the json buffer
            current_pos += m_json_buffer.size();
        }
        
        if (current_pos >= data.size()) {
            return;
        }
        
        if (m_json.isEmpty()) {
            // Read in the remaining json
            int remaining_bytes = data.size() - current_pos;
            int remaining_json_size = m_json_size - m_json_buffer.size();
            if (remaining_json_size > remaining_bytes) {
                m_json_buffer.append(data.mid(current_pos, remaining_json_size));
                return;
            }
            else {
                m_json_buffer.append(data.mid(current_pos, remaining_json_size));
                current_pos += remaining_json_size;
                m_json = QJsonDocument::fromJson(m_json_buffer);
                // Check if the data sent is WiiMixObjectives by checking if the json contains an achievement id
                // As achievement ids should only be paired with objectives
                for (int i = 0; i < m_json.array().size(); ++i) {
                    QJsonObject obj = m_json.array()[i].toObject();
                    if (obj.contains(QStringLiteral(OBJECTIVE_ACHIEVEMENT_ID))) {
                        WiiMixObjective objective = WiiMixObjective::FromJson(obj);
                        m_objectives.push_back(objective);
                    }
                    else {
                        break;
                    }
                }
                qDebug() << "Number of objectives" << m_objectives.size();
                assert(m_objectives.size() > 0);
            }
        }
        // Check if all data has been read in
        if (!m_json.isEmpty() && m_files_size >= m_data_size - m_json_size) {
            // Reset all data
            qDebug() << "All data read in by client";
            m_json_size = 0;
            m_files_size = 0;
            m_data_size = 0;
            m_current_file = 0;
            // if (m_files_size == 0) {
            ReceiveData(m_json);
            // }
        }
        else {
            // read in for the current file
            // keeping track of m_current_pos and m_file_sizes to know which file is currently being read
            // If the file is completely read in, save it, increment the file counter, and clear the file buffer 
            
            // TODO: m_file_sizes is not correct
            // m_file is not being reset correctly?
            int remaining_file_size = m_file_sizes[m_current_file] - m_file.size();
            if (remaining_file_size < 0) {
                qWarning() << "File size is negative";
                return;
            }
            int bytes_to_read = remaining_file_size;
            if (bytes_to_read > data.size() - current_pos) {
                bytes_to_read = data.size() - current_pos;
            }
            m_file.append(data.mid(current_pos, bytes_to_read));
            current_pos += bytes_to_read;

            if (m_file.size() == m_file_sizes[m_current_file]) {
                // File completely read, save it
                qDebug() << "Saving file";
                QString file_path = QString::fromStdString(File::GetUserPath(D_WIIMIX_STATESAVES_IDX)) + QString::number(m_objectives[m_current_file].GetId()) + QStringLiteral(".sav");
                qDebug() << "Saving file to " << file_path;
                QFile file(file_path);
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(m_file);
                    file.close();
                }
                m_current_file++;
                // Add the amount written to the total file size
                m_files_size += m_file.size();
                // Clear the file buffer in preparation for the new file
                m_file = {};
            }
            else if (m_file.size() > m_file_sizes[m_current_file]) {
                qWarning() << "File size is larger than expected";
                assert(false);
            }
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
    // qDebug() << "Bytes to write: " << m_socket->bytesToWrite();
    // qDebug() << "Bytes available: " << m_socket->bytesAvailable();
    // Check if all data has been sent
    if (m_bytes_written >= m_data.size()) {
        qDebug() << "All data sent successfully!";
        emit onBytesWritten(m_bytes_written, m_data.size());
        return;
    }

    // Write the next chunk
    const qint64 kbSize = 1024;
    const qint64 maxSafeBuffer = 32 * kbSize;  // Adjust this based on socket behavior

    // If the buffer is too full, wait before writing more
    if (m_socket->bytesToWrite() > maxSafeBuffer) {
        qDebug() << "Buffer is full; waiting before sending more data.";
        // Flush the buffer
        m_socket->flush();
        // Don't immediately exit; allow some time for the buffer to clear up
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  // This prevents UI freezing
        return;  // Early exit; we'll retry when `bytesWritten` is emitted again
    }

    if (m_socket->bytesAvailable() < m_socket->bytesToWrite()) {
        m_socket->flush();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    }

    qint64 maxChunkSize = 8 * kbSize;  // Default 8 KB
    if (m_socket->bytesToWrite() > 16 * kbSize) {  // Large queue, reduce chunk size
        // Don't immediately exit; allow some time for the buffer to clear up
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  // This prevents UI freezing
        maxChunkSize = 1024;
    }
    qint64 remaining = m_data.size() - m_bytes_written;
    qint64 written = m_socket->write(m_data.mid(m_bytes_written, qMin(maxChunkSize, remaining)));
    if (written == -1) {
        qCritical() << "Failed to write data to socket:" << m_socket->errorString();
        ModalMessageBox::critical(nullptr, tr("Error"), tr("Failed to write data to socket: %1").arg(m_socket->errorString()));
        return;
    }
    else {
        m_bytes_written += written;
    }

    // while (m_socket->bytesToWrite() > 0) {
    //     if (!m_socket->waitForBytesWritten(2000)) {  // 2 second timeout
    //         qWarning() << "Could not send chunk to server after 1 second:" << m_socket->errorString();
    //         break;
    //     }
    // }

    // Update the total bytes written
    // qDebug() << QStringLiteral("Bytes written: ") << m_bytes_written << "/" << m_data.size();

    // Notify progress
    // if (m_bytes_written % 3 == 0) {
    emit onBytesWritten(m_bytes_written, m_data.size());
    // Don't immediately exit; allow some time for the buffer to clear up
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);  // This prevents UI freezing
    // }
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
        // std::vector<WiiMixObjective> objectives;
        // for (int i = 0; i < json.array().size(); ++i) {
        //     QJsonObject obj = json.array()[i].toObject();
        //     WiiMixObjective objective = WiiMixObjective::FromJson(obj);
        //     objectives.push_back(objective);
        // }
        emit onGetObjectives(m_objectives);
    }
    else if (response == WiiMixEnums::Response::UPDATE_BINGO_OBJECTIVES) {
        qDebug() << "Updating bingo objectives";
        // std::vector<WiiMixObjective> objectives;
        // for (int i = 0; i < json.array().size(); ++i) {
        //     QJsonObject obj = json.array()[i].toObject();
        //     WiiMixObjective objective = WiiMixObjective::FromJson(obj);
        //     objectives.push_back(objective);
        // }
        WiiMixBingoSettings::instance()->SetObjectives(m_objectives);
        // onUpdateBingoObjectives only gets emitted by the last player, and so once all the players are ready
        // the bingo will start
        QMap<WiiMixEnums::Player, bool> players_ready = WiiMixBingoSettings::instance()->GetPlayersReady(); 
        for (int player = 0; player < static_cast<int>(WiiMixEnums::Player::END); ++player) {
            if (players_ready[static_cast<WiiMixEnums::Player>(player)] != true) return true;
        }
        // If all the players are ready, the bingos will start
        emit onUpdateBingoObjectives(WiiMixBingoSettings::instance());
    }
    else if (response == WiiMixEnums::Response::UPDATE_ROGUE_OBJECTIVES) {
        qDebug() << "Updating rogue objectives";
        // std::vector<WiiMixObjective> objectives;
        // for (int i = 0; i < json.array().size(); ++i) {
        //     QJsonObject obj = json.array()[i].toObject();
        //     WiiMixObjective objective = WiiMixObjective::FromJson(obj);
        //     objectives.push_back(objective);
        // }
        WiiMixRogueSettings::instance()->SetObjectives(m_objectives);
        emit onUpdateRogueObjectives(WiiMixRogueSettings::instance());
    }
    else if (response == WiiMixEnums::Response::UPDATE_SHUFFLE_OBJECTIVES) {
        qDebug() << "Updating shuffle objectives";
        // std::vector<WiiMixObjective> objectives;
        // for (int i = 0; i < json.array().size(); ++i) {
        //     QJsonObject obj = json.array()[i].toObject();
        //     WiiMixObjective objective = WiiMixObjective::FromJson(obj);
        //     objectives.push_back(objective);
        // 
        WiiMixShuffleSettings::instance()->SetObjectives(m_objectives);
        emit onUpdateShuffleObjectives(WiiMixShuffleSettings::instance());
    }
    else {
        qWarning() << "Invalid response from server " << static_cast<int>(response);
    }
    return true;
}

// Commented out because it's infeasible to read all of the save states into memory,
// and because dolphin loads them anyway, it makes the most sense to save them in storage 
// bool WiiMixClient::ReceiveData(QJsonDocument json, std::vector<QByteArray> files) {
//     WiiMixEnums::Response response = static_cast<WiiMixEnums::Response>(json.object()[QStringLiteral(CLIENT_RESPONSE)].toInt());

//     if (response == WiiMixEnums::Response::UPDATE_BINGO_OBJECTIVES) {
//         // For each objective in the json array, convert it to a wiimix objective
//         // and then save the savestate to the correct location [path] + id + ".sav"
//         std::string savestate_path = File::GetUserPath(D_WIIMIX_STATESAVES_IDX);
//         std::vector<WiiMixObjective> objectives;
//         for (int i = 0; i < json.array().size(); ++i) {
//             QJsonObject obj = json.array()[i].toObject();
//             WiiMixObjective objective = WiiMixObjective::FromJson(obj);
//             // Save the savestate
//             QFile file(QString::fromStdString(savestate_path + std::to_string(objective.GetId()) + ".sav"));
//             if (file.open(QIODevice::WriteOnly)) {
//                 file.write(files[i]);
//                 file.close();
//             }
//             objectives.push_back(objective);
//         }
//         WiiMixBingoSettings::instance()->SetObjectives(objectives);
//         emit onUpdateBingoObjectives(WiiMixBingoSettings::instance());
//     }
//     else if (response == WiiMixEnums::Response::UPDATE_ROGUE_OBJECTIVES) {
//         // For each objective in the json array, convert it to a wiimix objective
//         // and then save the savestate to the correct location [path] + id + ".sav"
//         std::string savestate_path = File::GetUserPath(D_WIIMIX_STATESAVES_IDX);
//         std::vector<WiiMixObjective> objectives;
//         for (int i = 0; i < json.array().size(); ++i) {
//             QJsonObject obj = json.array()[i].toObject();
//             WiiMixObjective objective = WiiMixObjective::FromJson(obj);
//             // Save the savestate
//             QFile file(QString::fromStdString(savestate_path + std::to_string(objective.GetId()) + ".sav"));
//             if (file.open(QIODevice::WriteOnly)) {
//                 file.write(files[i]);
//                 file.close();
//             }
//             objectives.push_back(objective);
//         }
//         WiiMixRogueSettings::instance()->SetObjectives(objectives);
//         emit onUpdateRogueObjectives(WiiMixRogueSettings::instance());
//     }
//     else if (response == WiiMixEnums::Response::UPDATE_SHUFFLE_OBJECTIVES) {
//         // For each objective in the json array, convert it to a wiimix objective
//         // and then save the savestate to the correct location [path] + id + ".sav"
//         std::string savestate_path = File::GetUserPath(D_WIIMIX_STATESAVES_IDX);
//         std::vector<WiiMixObjective> objectives;
//         for (int i = 0; i < json.array().size(); ++i) {
//             QJsonObject obj = json.array()[i].toObject();
//             WiiMixObjective objective = WiiMixObjective::FromJson(obj);
//             // Save the savestate
//             QFile file(QString::fromStdString(savestate_path + std::to_string(objective.GetId()) + ".sav"));
//             if (file.open(QIODevice::WriteOnly)) {
//                 file.write(files[i]);
//                 file.close();
//             }
//             objectives.push_back(objective);
//         }
//         WiiMixShuffleSettings::instance()->SetObjectives(objectives);
//         emit onUpdateShuffleObjectives(WiiMixShuffleSettings::instance());
//     }
// }

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