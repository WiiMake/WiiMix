#include "DolphinQt/WiiMix/WebAPI.h"
#include <curl/curl.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QByteArray>
#include <QBuffer>

#include <curl/curl.h>
#include "Core/Config/MainSettings.h"
#include "Core/Config/AchievementSettings.h"

// TODO: after testing remove the hardcoded token: "RoBoYefRsnxjSNiYdU2i8Coah9JaCRr5"
std::string WiiMixWebAPI::m_username = "WiiMix";
// std::string WiiMixWebAPI::m_token = Config::Get(Config::RA_API_TOKEN);

// void WiiMixWebAPI::setToken(std::string token) {
//   m_token = token;
// }

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

std::map<int, std::string> WiiMixWebAPI::getGameAchievements(int game_id) {
  std::map<int, std::string> achievements;
  CURL* curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    // NOTE: &f=5 limits to 5 achievements, remove to get all
    std::string url = "https://retroachievements.org/API/API_GetGameExtended.php?i=" + std::to_string(game_id) + "&z=" + m_username + "&y=" + Config::Get(Config::RA_API_TOKEN);
    qDebug() << url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
      qDebug() << readBuffer.c_str();

      QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(readBuffer).toUtf8());
      QJsonObject root = doc.object();
      QJsonObject achievementsJson = root.value(QStringLiteral("Achievements")).toObject();
      for (auto it = achievementsJson.begin(); it != achievementsJson.end(); ++it) {
        QJsonObject achievement = it.value().toObject();
        achievements[achievement[QStringLiteral("ID")].toInt()] = achievement[QStringLiteral("Title")].toString().toStdString();
      }
    }
  }
  return achievements;
}

std::map<std::string, int> WiiMixWebAPI::getGameList(int console_id) {
  std::map<std::string, int> game_list;
  CURL* curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    std::string url = "https://retroachievements.org/API/API_GetGameList.php?i=" + std::to_string(console_id) + "&z=" + m_username + "&y=" + Config::Get(Config::RA_API_TOKEN) + "&f=1&h=1";
    qDebug() << url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
      qDebug() << readBuffer.c_str();

      QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(readBuffer).toUtf8());
      QJsonArray root = doc.array();
      for (const QJsonValue& game : root) {
        QJsonObject gameObj = game.toObject();
        game_list[gameObj[QStringLiteral("Title")].toString().toStdString()] = gameObj[QStringLiteral("ID")].toInt();
      }
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
  if (curl)
  {
    std::string url = "https://retroachievements.org/API/API_GetAchievementUnlocks.php?z=" + m_username +
          "&y=" + Config::Get(Config::RA_API_TOKEN) + "&a=" + std::to_string(achievement_id);
    // qDebug() << url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK)
    {
      QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(readBuffer).toUtf8());
      qDebug() << doc;
      if (!doc.isNull() && doc.isObject())
      {
        QJsonObject root = doc.object();
        game_id = root.value(QStringLiteral("Game")).toObject().value(QStringLiteral("ID")).toInt();
      }
    }
  }
  return game_id;
}

int WiiMixWebAPI::getBadgeID(int rgame_id, int achievement_id) {
  std::map<int, std::string> achievements;
  CURL* curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    std::string url = "https://retroachievements.org/API/API_GetGameExtended.php?i=" + std::to_string(rgame_id) + "&z=" + m_username + "&y=" + Config::Get(Config::RA_API_TOKEN);
    qDebug() << url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
      qDebug() << readBuffer.c_str();

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
  return 0;
}

std::vector<uint8_t> WiiMixWebAPI::getAchievementIcon(int rgame_id, int achievement_id) {
  QPixmap icon;
  CURL* curl = curl_easy_init();
  CURLcode res;
  std::string readBuffer;
  std::vector<uint8_t> iconData;
  if (curl)
  {
    // rgame_id, achievement_id: 9602, 41890
    printf("rgame_id, achievement_id: %d, %d\n", rgame_id, achievement_id);
    int badge_id = getBadgeID(rgame_id, achievement_id);
    printf("Badge ID: %d\n", badge_id);
    printf("Badge ID: %d\n", badge_id);
    std::string url = "https://retroachievements.org/Badge/" + std::to_string(badge_id) + "_lock.png";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK)
    {
      iconData.assign(readBuffer.begin(), readBuffer.end());
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
  }
  return iconData;
}

// A simple request to check if the token is valid
bool WiiMixWebAPI::basicRequest(std::string token) {
  CURL* curl = curl_easy_init();
  CURLcode res;
  std::string readBuffer;
  if (curl)
  {
    std::string url = "https://retroachievements.org/API/API_GetUser.php?z=" + m_username +
          "&y=" + token;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK)
    {
      return true;
    }
  }
  return false;
}