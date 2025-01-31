#include "DolphinQt/WiiMix/GameManager.h"

#include "DolphinQt/WiiMix/GlobalSettings.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"

#include "Common/FileUtil.h"

WiiMixGameManager::WiiMixGameManager() {
  m_running = false;
  m_timer = new QTimer();
  if (!File::IsDirectory(File::GetUserPath(D_OBJECTIVES_IDX))) {
    qDebug() << "Creating directory objectives";
    File::CreateDir(File::GetUserPath(D_OBJECTIVES_IDX));
  }
  if (!File::IsDirectory(File::GetUserPath(D_WIIMIX_LIVE_STATESAVES_IDX))) {
    qDebug() << "Creating directory live savestates";
    File::CreateDir(File::GetUserPath(D_WIIMIX_LIVE_STATESAVES_IDX));
  }
}

void WiiMixGameManager::ClearLiveStates() {
  File::DeleteDirRecursively(File::GetUserPath(D_WIIMIX_LIVE_STATESAVES_IDX));
  File::CreateDir(File::GetUserPath(D_WIIMIX_LIVE_STATESAVES_IDX));
}

void WiiMixGameManager::Reset() {
  m_running = false;
  WiiMixGlobalSettings::instance()->SetCurrentObjective(-1);
  m_timer->stop();
  m_timer->setSingleShot(true);
}

void WiiMixGameManager::OnAchievementGet(std::set<uint32_t> achievements)
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
  if (WiiMixGlobalSettings::instance()->GetCurrentObjective() == -1) {
    // This shouldn't happen since we are not in a game
    return;
  }
  uint32_t achievement = *achievements.begin();
  std::vector<WiiMixObjective> objectives = WiiMixShuffleSettings::instance()->GetObjectives();
  if (objectives[WiiMixGlobalSettings::instance()->GetCurrentObjective()].GetAchievementId() != achievement) {
    return;
  }
  emit onAchievementGet(achievements);
  objectives.erase(objectives.begin() + WiiMixGlobalSettings::instance()->GetCurrentObjective()); // why are vectors weird lol
  WiiMixShuffleSettings::instance()->SetObjectives(objectives);
  WiiMixGlobalSettings::instance()->SetCurrentObjective(-1);
  // TODO: generalize this for all of the modes
  switch (WiiMixGlobalSettings::instance()->GetMode())
  {
    case WiiMixEnums::Mode::SHUFFLE:
      emit onShuffleUpdate();
      break;
    case WiiMixEnums::Mode::ROGUE:
      break;
    case WiiMixEnums::Mode::BINGO:
      break;
  }
}

bool WiiMixGameManager::IsRunning()
{
  return m_running;
}

void WiiMixGameManager::SetIsRunning(bool state)
{
  m_running = state;
}

QTimer *WiiMixGameManager::GetTimer()
{
  return m_timer;
}
