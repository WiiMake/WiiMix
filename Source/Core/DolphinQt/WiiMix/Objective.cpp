#include "DolphinQt/WiiMix/Objective.h"

#include "DolphinQt/WiiMix/Enums.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>
#include <curl/curl.h>
#include <fstream>
#include <string>

#include "Common/FileUtil.h"

WiiMixObjective::WiiMixObjective(int id, std::string file_hash, std::string title, int retroachievements_game_id,
                                 std::string game_id, int achievement_id,
                                 std::vector<WiiMixEnums::ObjectiveType> objective_type,
                                 std::string objective_description,
                                 std::vector<WiiMixEnums::GameGenre> game_genres,
                                 WiiMixEnums::Difficulty difficulty, int time, std::string creator_username,
                                 WiiMixEnums::ObjectiveStatus status, int num_times_completed,
                                 int num_times_attempted, WiiMixEnums::Player completed, int completion_time, std::chrono::system_clock::time_point last_attempted) {
        m_id = id;
        m_file_hash = std::move(file_hash);
        m_title = std::move(title);
        m_retroachievements_game_id = retroachievements_game_id;
        m_game_id = std::move(game_id);
        m_achievement_id = achievement_id;
        m_objective_types = objective_type;
        m_objective_description = std::move(objective_description);
        m_game_genres = std::move(game_genres);
        m_difficulty = difficulty;
        m_time = time;
        m_creator_username = creator_username;
        m_status = status;
        m_num_times_completed = num_times_completed;
        m_num_times_attempted = num_times_attempted;
        m_player_completed = completed;
        m_last_attempted = last_attempted;
        m_timer = new QElapsedTimer();
}

int WiiMixObjective::GetId()
{
  return m_id;
}

std::string WiiMixObjective::GetFileHash()
{
  return m_file_hash;
}

std::string WiiMixObjective::GetTitle()
{
  return m_title;
}

int WiiMixObjective::GetRetroAchievementsGameId()
{
  return m_retroachievements_game_id;
}

std::string WiiMixObjective::GetGameId()
{
  return m_game_id;
}

int WiiMixObjective::GetAchievementId()
{
  return m_achievement_id;
}

std::vector<WiiMixEnums::ObjectiveType> WiiMixObjective::GetObjectiveTypes()
{
  return m_objective_types;
}

std::string WiiMixObjective::GetObjectiveDescription()
{
  return m_objective_description;
}

std::vector<WiiMixEnums::GameGenre> WiiMixObjective::GetGameGenres()
{
  return m_game_genres;
}

WiiMixEnums::Difficulty WiiMixObjective::GetDifficulty()
{
  return m_difficulty;
}

int WiiMixObjective::GetTime()
{
  return m_time;
}

std::string WiiMixObjective::GetCreatorUsername()
{
  return m_creator_username;
}

QJsonObject WiiMixObjective::ToJson()
{
  QJsonObject obj;
  obj[QStringLiteral(OBJECTIVE_ID)] = m_id;
  obj[QStringLiteral(OBJECTIVE_FILE_HASH)] = QString::fromStdString(m_file_hash);
  obj[QStringLiteral(OBJECTIVE_TITLE)] = QString::fromStdString(m_title);
  obj[QStringLiteral(OBJECTIVE_RETROACHIEVEMENTS_GAME_ID)] = m_retroachievements_game_id;
  obj[QStringLiteral(OBJECTIVE_GAME_ID)] = QString::fromStdString(m_game_id);
  obj[QStringLiteral(OBJECTIVE_ACHIEVEMENT_ID)] = m_achievement_id;
  QJsonArray objective_types;
  for (const auto& type : m_objective_types)
  {
    objective_types.append(QString::fromStdString(WiiMixEnums::ObjectiveTypeToString(type)));
  }
  obj[QStringLiteral(OBJECTIVE_OBJECTIVE_TYPES)] = objective_types;
  obj[QStringLiteral(OBJECTIVE_DESCRIPTION)] = QString::fromStdString(m_objective_description);
  QJsonArray game_genres;
  for (const auto& genre : m_game_genres)
  {
    game_genres.append(QString::fromStdString(WiiMixEnums::GameGenreToString(genre)));
  }
  obj[QStringLiteral(OBJECTIVE_GAME_GENRES)] = game_genres;
  obj[QStringLiteral(OBJECTIVE_DIFFICULTY)] = QString::fromStdString(WiiMixEnums::DifficultyToString(m_difficulty));
  obj[QStringLiteral(OBJECTIVE_TIME)] = static_cast<qint64>(m_time);
  if (m_creator_username.empty())
  {
    obj[QStringLiteral(OBJECTIVE_CREATOR_USERNAME)] = QJsonValue::Null;
  }
  else
  {
    obj[QStringLiteral(OBJECTIVE_CREATOR_USERNAME)] = QString::fromStdString(m_creator_username);
  }
  obj[QStringLiteral(OBJECTIVE_HISTORY_MOST_RECENT_STATUS)] = QString::fromStdString(WiiMixEnums::ObjectiveStatusToString(m_status));
  obj[QStringLiteral(OBJECTIVE_HISTORY_NUM_TIMES_COMPLETED)] = m_num_times_completed;
  obj[QStringLiteral(OBJECTIVE_HISTORY_NUM_TIMES_ATTEMPTED)] = m_num_times_attempted;
  obj[QStringLiteral(OBJECTIVE_HISTORY_MOST_RECENT_ATTEMPT)] = static_cast<qint64>(std::chrono::duration_cast<std::chrono::seconds>(
    m_last_attempted.time_since_epoch()).count());
  obj[QStringLiteral(BINGO_SETTINGS_COMPLETED)] = static_cast<int>(m_player_completed);
  obj[QStringLiteral(BINGO_SETTINGS_COMPLETION_TIME)] = m_completion_time;
  return obj;
}

WiiMixObjective WiiMixObjective::FromJson(const QJsonObject& obj)
{
  int id = obj[QStringLiteral(OBJECTIVE_ID)].toInt();
  std::string md5sum = obj[QStringLiteral(OBJECTIVE_FILE_HASH)].toString().toStdString();
  std::string title = obj[QStringLiteral(OBJECTIVE_TITLE)].toString().toStdString();
  int retro_id = obj[QStringLiteral(OBJECTIVE_RETROACHIEVEMENTS_GAME_ID)].toInt();
  std::string game_id = obj[QStringLiteral(OBJECTIVE_GAME_ID)].toString().toStdString();
  int achievement_id = obj[QStringLiteral(OBJECTIVE_ACHIEVEMENT_ID)].toInt();
  std::vector<WiiMixEnums::ObjectiveType> objective_types;
  for (const auto& objective_type : obj[QStringLiteral(OBJECTIVE_OBJECTIVE_TYPES)].toArray())
    objective_types.push_back(WiiMixEnums::ObjectiveTypeFromString(objective_type.toString().toStdString()));
  std::string description = obj[QStringLiteral(OBJECTIVE_DESCRIPTION)].toString().toStdString();
  std::vector<WiiMixEnums::GameGenre> game_genres;
  for (const auto& genre : obj[QStringLiteral(OBJECTIVE_GAME_GENRES)].toArray())
    game_genres.push_back(WiiMixEnums::GameGenreFromString(genre.toString().toStdString()));
  auto difficulty = WiiMixEnums::DifficultyFromString(
    obj[QStringLiteral(OBJECTIVE_DIFFICULTY)].toString().toStdString());
  int time = obj[QStringLiteral(OBJECTIVE_TIME)].toInt();
  std::string creator_username = obj[QStringLiteral(OBJECTIVE_CREATOR_USERNAME)].isNull() ? "" : obj[QStringLiteral(OBJECTIVE_CREATOR_USERNAME)].toString().toStdString();
  auto status = WiiMixEnums::ObjectiveStatusFromString(obj[QStringLiteral(OBJECTIVE_HISTORY_MOST_RECENT_STATUS)].toString().toStdString());
  int num_times_completed = obj[QStringLiteral(OBJECTIVE_HISTORY_NUM_TIMES_COMPLETED)].toInt();
  int num_times_attempted = obj[QStringLiteral(OBJECTIVE_HISTORY_NUM_TIMES_ATTEMPTED)].toInt();
  std::chrono::system_clock::time_point last_attempted = std::chrono::system_clock::from_time_t(
    obj[QStringLiteral(OBJECTIVE_HISTORY_MOST_RECENT_ATTEMPT)].toInt());
  WiiMixEnums::Player bingo_settings_completed = static_cast<WiiMixEnums::Player>(obj[QStringLiteral(BINGO_SETTINGS_COMPLETED)].toInt());
  int bingo_settings_completion_time = CompletionTimeFromString(obj[QStringLiteral(BINGO_SETTINGS_COMPLETION_TIME)].toString());

  return WiiMixObjective(
    id, md5sum, title, retro_id, game_id, achievement_id, objective_types, description,
    game_genres, difficulty, time, creator_username, status, num_times_completed,
    num_times_attempted, bingo_settings_completed, bingo_settings_completion_time, last_attempted);
}

WiiMixEnums::Player WiiMixObjective::GetCompleted()
{
  return m_player_completed;
}

void WiiMixObjective::SetCompleted(WiiMixEnums::Player player)
{
  m_player_completed = player;
}

WiiMixEnums::ObjectiveStatus WiiMixObjective::GetStatus()
{
  return m_status;
}

int WiiMixObjective::GetNumTimesCompleted()
{
  return m_num_times_completed;
}

int WiiMixObjective::GetNumTimesAttempted()
{
  return m_num_times_attempted;
}

std::chrono::system_clock::time_point WiiMixObjective::GetLastAttempted()
{
  return m_last_attempted;
}

void WiiMixObjective::SetStatus(WiiMixEnums::ObjectiveStatus status)
{
  m_status = status;
}

void WiiMixObjective::SetFileHash(std::string file_hash)
{
  m_file_hash = file_hash;
}

void WiiMixObjective::SetNumTimesCompleted(int num_times_completed)
{
  m_num_times_completed = num_times_completed;
}

void WiiMixObjective::SetNumTimesAttempted(int num_times_attempted)
{
  m_num_times_attempted = num_times_attempted;
}

void WiiMixObjective::SetLastAttempted(std::chrono::system_clock::time_point last_attempted)
{
  m_last_attempted = last_attempted;
}

int WiiMixObjective::GetCompletionTime()
{
  return m_completion_time;
  // return m_timer->elapsed();
}

void WiiMixObjective::SetCompletionTime(int completion_time)
{
  m_completion_time = completion_time;
}

// // -------- stopwatch -----------
// void WiiMixObjective::StartCompletionTimer()
// {
//   m_completion_timer->start(86400000); // 24 hours in milliseconds
// }

// void WiiMixObjective::ResetCompletionTimer()
// {
//   QTime final_time_difference = QTime(24, 0, 0).addMSecs(-m_completion_timer->remainingTime());
//   StopCompletionTimer();
//   StartCompletionTimer();
//   // Note that in bingo, the completion time is ignored if another player has a lower time
//   m_completion_time = final_time_difference;
// }

// void WiiMixObjective::StopCompletionTimer() {
//   m_completion_timer->stop();
// }

// void WiiMixObjective::DeleteCompletionTimer() {
//   delete m_completion_timer;
//   m_completion_timer = nullptr;
// }

// // ---------- countdown ------------
// void WiiMixObjective::SetCompletionTimer(int time_to_complete) {
//   StopCompletionTimer();
//   m_completion_timer->start(time_to_complete);
// }

// // For connecting events to the timeout signal
// QTimer *WiiMixObjective::GetCompletionTimer() {
//   return m_completion_timer;
// }
QString WiiMixObjective::CompletionTimeToString(int ms)
{
    int seconds = (ms / 1000) % 60;
    int minutes = (ms / (1000 * 60)) % 60;
    int hours = (ms / (1000 * 60 * 60)) % 24;
    int days = ms / (1000 * 60 * 60 * 24);

    // Format the string as "days:hh:mm:ss" with leading zeros
    return QStringLiteral("%1:%2:%3:%4")
        .arg(days)
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

int WiiMixObjective::CompletionTimeFromString(QString time)
{
    // Split the input time into days and "hh:mm:ss"
    qDebug() << "Time: " << time;
    QStringList parts = time.split(QStringLiteral(":"));
    if (parts.size() != 4)
    {
        qWarning("Invalid time format. Expected format: days:hh:mm:ss");
        return -1; // Return error code
    }

    // Parse days
    int days = parts[0].toInt();

    // Use QTime for parsing the "hh:mm:ss" part
    QTime qtime = QTime::fromString(parts[1] + QStringLiteral(":") + parts[2] + QStringLiteral(":") + parts[3], QStringLiteral("hh:mm:ss"));
    if (!qtime.isValid())
    {
        qWarning("Invalid time format for hh:mm:ss");
        return -1; // Return error code
    }

    // Convert days and time into milliseconds
    return (days * 24 * 60 * 60 * 1000) + QTime(0, 0).msecsTo(qtime);
}

// Comma-separated list of local objectives
#ifdef QT_GUI_LIB
  std::string WiiMixObjective::GetLocalObjectiveString() {
    qDebug() << "Get local objective string";
    std::string local_objectives;
    std::string objectives_dir = File::GetUserPath(D_OBJECTIVES_IDX);
    qDebug() << QString::fromStdString(objectives_dir);
    qDebug() << "Size: " << std::distance(std::filesystem::directory_iterator(objectives_dir), std::filesystem::directory_iterator{});
    for (const auto& entry : std::filesystem::directory_iterator(objectives_dir)) {
      if (entry.is_regular_file()) {
        std::string filename = entry.path().filename().string();
        qDebug() << QString::fromStdString(filename);
        if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".sav") {
          filename = filename.substr(0, filename.size() - 4); // Remove ".sav"
          bool is_number = std::all_of(filename.begin(), filename.end(), ::isdigit);
          if (!is_number) {
            continue;
          }
        }
        local_objectives += filename + ",";
      }
    }
    if (!local_objectives.empty()) {
      local_objectives.pop_back(); // Remove the trailing comma
    }
    return local_objectives;
  }
#else
  std::string WiiMixObjective::GetLocalObjectiveString() {
    qDebug() << "No code local objective string";
    return "";
  }
#endif

QList<int> WiiMixObjective::GetLocalObjectiveList(QString local_objectives) {
  QList<int> objectives;
  QStringList objectives_list = QString::fromStdString(local_objectives.toStdString()).split(QStringLiteral(","));
  for (const auto& objective : objectives_list) {
    objectives.append(objective.toInt());
  }
  return objectives;
}