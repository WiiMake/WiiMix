#include "DolphinQt/WiiMix/ShuffleGame.h"

WiiMixShuffleGame* WiiMixShuffleGame::m_instance = nullptr;

void WiiMixShuffleGame::StartShuffle()
{
  // TODOx: not quite sure what else needs to be implimented here ~@gyoder
  // CreateShuffleConnections();
  if (WiiMixShuffleSettings::instance()->GetObjectives().empty()) {
    qDebug() << "No objectives to shuffle";
    return;
  }
  if (m_timer != nullptr) {
    StopShuffle();
  }
  UpdateShuffle();
}

void WiiMixShuffleGame::UpdateShuffle()
{
  qDebug() << "Updating shuffle";
  NextRandomObjective();
  int min_time_between_switch = WiiMixShuffleSettings::instance()->GetMinTimeBetweenSwitch();
  int max_time_between_switch = WiiMixShuffleSettings::instance()->GetMaxTimeBetweenSwitch();
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(UpdateShuffle()));
  std::random_device rd;
  std::mt19937 g(rd());
  int time_between_switch = g() % (max_time_between_switch - min_time_between_switch + 1) + min_time_between_switch;
  m_timer->start(time_between_switch * 1000);
}

void WiiMixShuffleGame::StopShuffle()
{
  //stop the update timer
  m_timer->stop();
  delete m_timer;
  m_timer = nullptr;
}

void WiiMixShuffleGame::OnAchievementGet(std::set<uint32_t> achievements)
{
  // check if achievements are valid
  if (achievements.empty())
  {
    return;
  }
  if (achievements.size() > 1)
  {
    // TODOx: this should be handled eventually but im not sure how big of an issue it will be ~@gyoder
    qDebug() << "Achievements size is greater than 1. Please contact the developer.";
    return;
  }
  if (m_current_objective == -1) {
    // This shouldn't happen since we are not in a game
    return;
  }
  uint32_t achievement = *achievements.begin();
  std::vector<WiiMixObjective> objectives = WiiMixShuffleSettings::instance()->GetObjectives();
  if (objectives[m_current_objective].GetAchievementId() != achievement) {
    return;
  }
  objectives.erase(objectives.begin() + m_current_objective); // why are vectors weird lol
  WiiMixShuffleSettings::instance()->SetObjectives(objectives);
  m_current_objective = -1;
  NextRandomObjective();
}

void WiiMixShuffleGame::NextRandomObjective()
{
  std::random_device rd;
  std::mt19937 g(rd());
  int next_objective_index = g() % WiiMixShuffleSettings::instance()->GetObjectives().size();
  if (next_objective_index == m_current_objective) {
    NextRandomObjective();
    return;
  }
  if (m_current_objective == -1) {
    emit StartObjective(WiiMixShuffleSettings::instance()->GetObjectives()[next_objective_index]);
  } else {
    emit SwapObjective(WiiMixShuffleSettings::instance()->GetObjectives()[next_objective_index],
            WiiMixShuffleSettings::instance()->GetObjectives()[m_current_objective]);
  }
  m_current_objective = next_objective_index;
  // qDebug() << "Current objective: " << WiiMixShuffleSettings::instance()->GetObjectives()[m_current_objective].GetId();
}
