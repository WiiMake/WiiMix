#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/CommonSettings.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"
#include "DolphinQt/Achievements/AchievementsWindow.h"
#include "DolphinQt/MainWindow.h"

#include <QObject>
#include <QTimer>
#include <set>

class WiiMixShuffleGame : public QObject
{
        Q_OBJECT
    public:
        static WiiMixShuffleGame* instance() {
            if (!m_instance) {
                m_instance = new WiiMixShuffleGame();
            }
            return m_instance;
        }
        void StartShuffle();
        void StopShuffle();

    signals:
        void StartObjective(WiiMixObjective new_objective);
        void SwapObjective(WiiMixObjective new_objective, WiiMixObjective current_objective);
        void RestartObjective(WiiMixObjective new_objective);
        void RestartObjective(WiiMixObjective new_objective, WiiMixObjective current_objective);

    public slots:
        void OnAchievementGet(std::set<uint32_t> achievements);

    private:
        void NextRandomObjective();

        int32_t m_current_objective = -1;
        QTimer* m_timer;

        static WiiMixShuffleGame* m_instance;
    private slots:
        void UpdateShuffle();

};
