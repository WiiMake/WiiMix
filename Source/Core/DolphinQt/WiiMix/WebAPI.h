#pragma once

#include <map>
#include <string>
#include <QPixmap>

class WiiMixWebAPI {
  public:
    // static void RALoginCallback(std::string username, std::string token); // currently unused
    static std::map<int, std::string> getGameAchievements(int game_id);
    static std::map<std::string, int> getGameList(int console_id); // 16 for gamecube
    static int getRetroAchievementsGameID(int achievement_id);
    static int getBadgeID(int game_id, int achievement_id);
    static std::vector<uint8_t> getAchievementIcon(int game_id, int achievement_id);
    static bool basicRequest(std::string token);
    // void WiiMixWebAPI::setToken(std::string token);
  private:
    static std::string m_username;
    static std::string m_token;
};
