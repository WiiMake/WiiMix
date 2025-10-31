#pragma once

#include <QObject>
#include <map>
#include <string>
#include <QPixmap>

class WiiMixWebAPI : public QObject  {
  Q_OBJECT

  public:
    static WiiMixWebAPI* instance() {
      if (!s_instance) {
        s_instance = new WiiMixWebAPI();
      }
      return s_instance;
    }
    // static void RALoginCallback(std::string username, std::string token); // currently unused
    std::map<int, std::string> getGameAchievements(int game_id);
    std::map<std::string, int> getGameList(int console_id); // 16 for gamecube
    int getRetroAchievementsGameID(int achievement_id);
    int getBadgeID(int game_id, int achievement_id);
    std::vector<uint8_t> getAchievementIcon(int game_id, int achievement_id);
    bool basicRequest(std::string token);
    bool isConnected();
    void setConnected(bool connected);
    // void WiiMixWebAPI::setToken(std::string token);
    void secureSet(const QString& key, const QString& value);
    void secureGet(const QString& key, std::function<void(const QString& value)> onLoaded);
    void setToken(std::string token) { m_token = token; }
    std::string getToken() { return m_token; }
    std::string getUsername() { return m_username; }
    void setUsername(std::string username) { m_username = username; }
    void setPassword(std::string password) { m_password = password; }
    std::string getPassword() { return m_password; }

    #define RETROACHIEVEMENTS_API_TOKEN "RA_API_TOKEN"
    #define RETROCACHIEVEMENTS_PASSWORD "RA_PASSWORD"

  signals:
    void onRetroachievementsConnection(bool connected);

  protected:
    explicit WiiMixWebAPI(QObject *parent = nullptr, std::string username = "", std::string token = "", bool connected = false);

    inline static WiiMixWebAPI* s_instance = nullptr;

  private:
    std::string m_username;
    std::string m_token;
    std::string m_password;
    bool m_is_connected;
};
