#include "DolphinQt/WiiMix/Objective.h"

#include "DolphinQt/WiiMix/Enums.h"

#include <string>
#include <curl/curl.h>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <QDebug>

std::string WiiMixObjective::m_username = "";
std::string WiiMixObjective::m_token = "";

// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later
WiiMixObjective::WiiMixObjective(
    uint16_t achievement_id,
    uint16_t game_id,
    std::string title,
    std::string description,
    WiiMixEnums::Player completed
  ) : m_achievement_id(achievement_id), m_game_id(game_id), m_title(title), m_description(description), m_completed(completed){};



uint16_t WiiMixObjective::GetAchievementID() {
    return m_achievement_id;
}

uint16_t WiiMixObjective::GetGameID() {
    return m_game_id;
}

std::string WiiMixObjective::GetTitle() {
    return m_title;
}

std::string WiiMixObjective::GetDescription() {
    return m_description;
}

WiiMixEnums::Player WiiMixObjective::GetCompleted() {
    return m_completed;
}

void WiiMixObjective::SetCompleted(WiiMixEnums::Player player) {
    m_completed = player;
}

std::string WiiMixObjective::GetBadgeURL() {
    return std::format("https://media.retroachievements.org/Badge/{0}.png", m_achievement_id);
}

void WiiMixObjective::RALoginCallback(std::string username, std::string token) {
    m_username = username;
    //TODOx: this is not the correct api token
    m_token = token;
}


size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::map<uint16_t, std::string> WiiMixObjective::getGameAchievements(uint16_t game_id) {
    std::map<uint16_t, std::string> achievements;
    CURL* curl;
    CURLcode res;
    std::string readBuffer;



    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://retroachievements.org/API/API_GetGameExtended.php?i=" + std::to_string(game_id) + "&z=" + "WiiMix" + "&y=" + "RoBoYefRsnxjSNiYdU2i8Coah9JaCRr5" + "&f=5";
        qDebug() << url.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            qDebug() << readBuffer.c_str();

            json root = json::parse(readBuffer);
            const json& achievementsJson = root["Achievements"];
            for (const auto& [key, achievement] : achievementsJson.items()) {

                std::map<uint16_t, std::string> achievementMap;
                achievements[achievement["ID"].get<uint16_t>()] = achievement["Title"].get<std::string>() + ": " + achievement["Description"].get<std::string>();

            }

        }
    }
    return achievements;
}

std::map<std::string, uint16_t> WiiMixObjective::getGameList(uint16_t console_id) {
    std::map<std::string, uint16_t> game_list;
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "https://retroachievements.org/API/API_GetGameList.php?i=" + std::to_string(console_id) + "&z=" + "WiiMix" + "&y=" + "RoBoYefRsnxjSNiYdU2i8Coah9JaCRr5" + "&f=1&h=1";
        qDebug() << url.c_str();
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            qDebug() << readBuffer.c_str();

            json root = json::parse(readBuffer);
            for (const auto& game : root) {
                game_list[game["Title"].get<std::string>()] = game["ID"].get<uint16_t>();
            }
        }
    }
    return game_list;
}
