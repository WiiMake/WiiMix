#pragma once

#include <map>
#include <string>
#include <QPixmap>

class WiiMixWebAPI {
  public:
    // static void RALoginCallback(std::string username, std::string token); // currently unused
    static std::map<uint16_t, std::string> getGameAchievements(uint16_t game_id);
    static std::map<std::string, uint16_t> getGameList(uint16_t console_id); // 16 for gamecube
    static uint16_t getGameID(uint32_t achievement_id);
    static std::vector<uint8_t> getAchievementIcon(int achievement_id);
  private:
    static std::string m_username;
    static std::string m_token;
};
