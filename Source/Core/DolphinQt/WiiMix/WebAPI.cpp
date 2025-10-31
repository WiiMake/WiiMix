#include "DolphinQt/WiiMix/WebAPI.h"
#include <curl/curl.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QByteArray>
#include <QBuffer>
#include <qt6keychain/keychain.h>

#include <curl/curl.h>
#include "Core/Config/MainSettings.h"
#include "Core/Config/AchievementSettings.h"
#include <qcoreapplication.h>

WiiMixWebAPI::WiiMixWebAPI(QObject *parent, std::string username, std::string token, bool connected) : QObject(parent), m_username(username), m_token(token), m_is_connected(connected) {}

void WiiMixWebAPI::secureSet(const QString& key, const QString& value)
{
  // Use the application name as the "service"
  QString service = qApp->applicationName();
  
  QKeychain::WritePasswordJob *job = new QKeychain::WritePasswordJob(service);
  job->setKey(key);
  job->setTextData(value);

  // Connect to 'finished' just to log errors (optional but recommended)
  QObject::connect(job, &QKeychain::Job::finished, [job, value, key]() {
    if (job->error()) {
      qWarning() << "Keychain write error:" << job->errorString();
    } else {
      qDebug() << "Keychain value stored successfully.";
      if (key == QStringLiteral(RETROACHIEVEMENTS_API_TOKEN)) {
        WiiMixWebAPI::instance()->setToken(value.toStdString());
        Config::SetCurrent(Config::RA_API_TOKEN, value.toStdString());
      } else if (key == QStringLiteral(RETROCACHIEVEMENTS_PASSWORD)) {
        WiiMixWebAPI::instance()->setPassword(value.toStdString());
      }
      // Test connection
      bool connected = WiiMixWebAPI::instance()->basicRequest(WiiMixWebAPI::instance()->getToken());
      emit WiiMixWebAPI::instance()->onRetroachievementsConnection(connected);
    }
    job->deleteLater(); // Clean up
  });

  job->start();
}

void WiiMixWebAPI::secureGet(const QString& key, std::function<void(const QString& value)> onLoaded)
{
  QString service = qApp->applicationName();
  QKeychain::ReadPasswordJob *job = new QKeychain::ReadPasswordJob(service);
  job->setKey(key);
  job->setAutoDelete(true); // Automatically delete job when finished

  QObject::connect(job, &QKeychain::ReadPasswordJob::finished, [job, onLoaded]() {
    QString value;
    
    if (job->error() == QKeychain::Error::EntryNotFound) {
      qDebug() << "Keychain: No entry found for key:" << job->key();
    } else if (job->error()) {
      qWarning() << "Keychain read error:" << job->errorString();
    } else {
      qDebug() << "Keychain value loaded successfully for key:" << job->key();
      value = job->textData();
      // Set password or api token for WiiMixWebAPI based on key
      // Handled in callback function
      // if (key == QStringLiteral(RETROACHIEVEMENTS_API_TOKEN)) {
      //   WiiMixWebAPI::instance()->setToken(value.toStdString());
      // } else if (key == QStringLiteral(RETROCACHIEVEMENTS_PASSWORD)) {
      //   WiiMixWebAPI::instance()->setPassword(value.toStdString());
      // }
    }
    // Execute the callback with the value (will be empty on error)
    if (onLoaded) {
      onLoaded(value);
    }
  });

  job->start();
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

bool WiiMixWebAPI::isConnected()
{
  return m_is_connected;
}

void WiiMixWebAPI::setConnected(bool connected)
{
  m_is_connected = connected;
}

std::map<int, std::string> WiiMixWebAPI::getGameAchievements(int game_id) {
  std::map<int, std::string> achievements;
  CURL* curl;
  CURLcode res;
  std::string readBuffer;
  long http_code = 0;
  curl = curl_easy_init();
  if (curl) {
    // NOTE: &f=5 limits to 5 achievements, remove to get all
    std::string url = "https://retroachievements.org/API/API_GetGameExtended.php?i=" + std::to_string(game_id) + "&z=" + m_username + "&y=" + m_token;
    qDebug() << url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      curl_easy_cleanup(curl);
      if (http_code == 200) {
        qDebug() << QString::fromStdString(readBuffer.substr(0, 100));

        QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(readBuffer).toUtf8());
        QJsonObject root = doc.object();
        QJsonObject achievementsJson = root.value(QStringLiteral("Achievements")).toObject();
        for (auto it = achievementsJson.begin(); it != achievementsJson.end(); ++it) {
          QJsonObject achievement = it.value().toObject();
          achievements[achievement[QStringLiteral("ID")].toInt()] = achievement[QStringLiteral("Title")].toString().toStdString();
        }
      }
    }
    else {
      curl_easy_cleanup(curl);
    }
  }
  return achievements;
}

std::map<std::string, int> WiiMixWebAPI::getGameList(int console_id) {
  std::map<std::string, int> game_list;
  CURL* curl;
  CURLcode res;
  std::string readBuffer;
  long http_code = 0;

  curl = curl_easy_init();
  if (curl) {
    std::string url = "https://retroachievements.org/API/API_GetGameList.php?i=" + std::to_string(console_id) + "&z=" + m_username + "&y=" + m_token + "&f=1&h=1";
    qDebug() << url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      curl_easy_cleanup(curl);

      if (http_code == 200) {
        qDebug() << QString::fromStdString(readBuffer.substr(0, 100));

        QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(readBuffer).toUtf8());
        QJsonArray root = doc.array();
        for (const QJsonValue& game : root) {
          QJsonObject gameObj = game.toObject();
          game_list[gameObj[QStringLiteral("Title")].toString().toStdString()] = gameObj[QStringLiteral("ID")].toInt();
        }
      }
    }
    else {
      curl_easy_cleanup(curl);
    }
  }
  return game_list;
}

// Get retroachievements game ID
// Note that this is different from the WiiMix game ID
int WiiMixWebAPI::getRetroAchievementsGameID(int achievement_id) {
  int game_id = 0;
  CURL* curl = curl_easy_init();
  CURLcode res;
  std::string readBuffer;
  long http_code = 0;
  if (curl)
  {
    std::string url = "https://retroachievements.org/API/API_GetAchievementUnlocks.php?z=" + m_username +
          "&y=" + m_token + "&a=" + std::to_string(achievement_id);
    // qDebug() << url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);

    if (res == CURLE_OK)
    {
      QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(readBuffer).toUtf8());
      qDebug() << doc;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      curl_easy_cleanup(curl);
      if (http_code == 200)
      {
        if (!doc.isNull() && doc.isObject())
        {
          QJsonObject root = doc.object();
          game_id = root.value(QStringLiteral("Game")).toObject().value(QStringLiteral("ID")).toInt();
        }
      }
    }
    else {
      curl_easy_cleanup(curl);
    }
  }
  return game_id;
}

int WiiMixWebAPI::getBadgeID(int rgame_id, int achievement_id) {
  std::map<int, std::string> achievements;
  CURL* curl;
  CURLcode res;
  std::string readBuffer;
  long http_code = 0;
  curl = curl_easy_init();
  if (curl) {
    std::string url = "https://retroachievements.org/API/API_GetGameExtended.php?i=" + std::to_string(rgame_id) + "&z=" + m_username + "&y=" + m_token;
    qDebug() << url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
      qDebug() << QString::fromStdString(readBuffer.substr(0, 100));
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      if (http_code == 200) {
        curl_easy_cleanup(curl);
        QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(readBuffer).toUtf8());
        QJsonObject root = doc.object();
        QJsonObject achievementsJson = root.value(QStringLiteral("Achievements")).toObject();
        for (auto it = achievementsJson.begin(); it != achievementsJson.end(); ++it) {
          QJsonObject achievement = it.value().toObject();
          if (achievement.value(QStringLiteral("ID")).toInt() == achievement_id)
          {
            // Load badge icon
            // If it is a QJsonValue, it needs to be converted to a string first before converting it to an int
            return achievement.value(QStringLiteral("BadgeName")).toString().toInt();
          }
        }
      }
    }
  }
  curl_easy_cleanup(curl);
  return 0;
}

std::vector<uint8_t> WiiMixWebAPI::getAchievementIcon(int rgame_id, int achievement_id) {
  QPixmap icon;
  CURL* curl = curl_easy_init();
  CURLcode res;
  std::string readBuffer;
  std::vector<uint8_t> iconData;
  long http_code = 0;
  if (curl)
  {
    // rgame_id, achievement_id: 9602, 41890
    printf("rgame_id, achievement_id: %d, %d\n", rgame_id, achievement_id);
    int badge_id = getBadgeID(rgame_id, achievement_id);
    printf("Badge ID: %d\n", badge_id);
    std::string url = "https://retroachievements.org/Badge/" + std::to_string(badge_id) + "_lock.png";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);

    if (res == CURLE_OK)
    {
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
      if (http_code == 200) {
        curl_easy_cleanup(curl);
        iconData.assign(readBuffer.begin(), readBuffer.end());
      }
      // QImage image;
      // image.loadFromData(reinterpret_cast<const uchar*>(iconData.data()), iconData.size(), "PNG");
      // // if (!image.isNull())
      // // {
      // //   QImage rgbaImage = image.convertToFormat(QImage::Format_RGBA8888);
      // //   uchar* pixels = rgbaImage.bits();
      // //   int size = rgbaImage.height() * rgbaImage.width() * 4;
      // //   iconData.assign(rgbaImage.bits(), rgbaImage.bits() + size);
      // // }
      // iconData.assign(image.bits(), image.bits() + image.sizeInBytes() * 8);
    }
    else {
      curl_easy_cleanup(curl);
    }
  }
  return iconData;
}

// A simple request to check if the token is valid
bool WiiMixWebAPI::basicRequest(std::string token) {
  CURL* curl = curl_easy_init();
  CURLcode res;
  std::string readBuffer;
  long http_code = 0;
  if (curl)
  {
    std::string url = "https://retroachievements.org/API/API_GetTopTenUsers.php?&y=" + token;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);

    qDebug() << "Retroachievements basic request response:";
    qDebug() << QString::fromStdString(readBuffer.substr(0, 100));

    if (res != CURLE_OK)
    {
      return false;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (http_code == 200)
    {
      return true;
    }
  }
  return false;
}