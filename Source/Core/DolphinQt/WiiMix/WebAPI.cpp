#include "DolphinQt/WiiMix/WebAPI.h"
#include <curl/curl.h>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

std::string WiiMixWebAPI::m_username = "WiiMix";
std::string WiiMixWebAPI::m_token = "RoBoYefRsnxjSNiYdU2i8Coah9JaCRr5";

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

std::map<uint16_t, std::string> WiiMixWebAPI::getGameAchievements(uint16_t game_id) {
  std::map<uint16_t, std::string> achievements;
  CURL* curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    std::string url = "https://retroachievements.org/API/API_GetGameExtended.php?i=" + std::to_string(game_id) + "&z=" + m_username + "&y=" + m_token + "&f=5";
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

std::map<std::string, uint16_t> WiiMixWebAPI::getGameList(uint16_t console_id) {
  std::map<std::string, uint16_t> game_list;
  CURL* curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if (curl) {
    std::string url = "https://retroachievements.org/API/API_GetGameList.php?i=" + std::to_string(console_id) + "&z=" + m_username + "&y=" + m_token + "&f=1&h=1";
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

uint16_t WiiMixWebAPI::getGameID(uint32_t achievement_id) {
  uint16_t game_id = 0;
  CURL* curl = curl_easy_init();
  CURLcode res;
  std::string readBuffer;
  if (curl)
  {
    std::string url = "https://retroachievements.org/API/API_GetAchievementUnlocks.php?z=" + m_username +
          "&y=" + m_token + "&a=" + std::to_string(achievement_id);
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
