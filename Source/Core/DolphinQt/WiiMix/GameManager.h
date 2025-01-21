#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/CommonSettings.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"
#include "DolphinQt/Achievements/AchievementsWindow.h"

#include <QObject>
#include <QTimer>
#include <set>

class WiiMixGameManager : public QObject
{
    Q_OBJECT
public:
    static WiiMixGameManager* instance() {
        if (!s_instance) {
            s_instance = new WiiMixGameManager();
        }
        return s_instance;
    }
    
    void Reset();
    void SetIsRunning(bool state);
    bool IsRunning();
    QTimer *GetTimer();

signals:
    void onShuffleUpdate();
    void onAchievementGet(std::set<uint32_t> achievements);

public slots:
    void OnAchievementGet(std::set<uint32_t> achievements);

protected:
  explicit WiiMixGameManager();

private:
    bool m_running;
    QTimer* m_timer;

    inline static WiiMixGameManager* s_instance = nullptr; // Singleton instance
};
