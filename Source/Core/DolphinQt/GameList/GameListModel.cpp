// Copyright 2015 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/GameList/GameListModel.h"

#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QRegularExpression>
#include <QCheckBox>

#include "Common/FileUtil.h"

#include "Core/Config/MainSettings.h"
#include "Core/ConfigManager.h"

#include "DiscIO/Enums.h"

#include <DolphinQt/Config/GameConfigWidget.h>
#include "DolphinQt/QtUtils/ImageConverter.h"
#include "DolphinQt/Resources.h"
#include "DolphinQt/Settings.h"

#include "UICommon/GameFile.h"
#include "UICommon/UICommon.h"

#include "DolphinQt/WiiMix/GlobalSettings.h"

const QSize GAMECUBE_BANNER_SIZE(96, 32);

GameListModel::GameListModel(QObject* parent) : QAbstractTableModel(parent)
{
  connect(&m_tracker, &GameTracker::GameLoaded, this, &GameListModel::AddGame);
  connect(&m_tracker, &GameTracker::GameUpdated, this, &GameListModel::UpdateGame);
  connect(&m_tracker, &GameTracker::GameRemoved, this, &GameListModel::RemoveGame);
  // Connect the game tracker to the WiiMixSettings singleton
  
  // NOTE: commented this out (potential race condition?)
  // connect(&m_tracker, &GameTracker::GameLoaded, WiiMixGlobalSettings::instance(), &WiiMixGlobalSettings::AddGame);
  // connect(&m_tracker, &GameTracker::GameUpdated, WiiMixGlobalSettings::instance(), &WiiMixGlobalSettings::UpdateGame);
  // connect(&m_tracker, &GameTracker::GameRemoved, WiiMixGlobalSettings::instance(), &WiiMixGlobalSettings::RemoveGame);

  // connect(WiiMixGlobalSettings::instance(), &WiiMixGlobalSettings::SettingsChanged, this, &GameListModel::setWiiMixData);

  connect(&Settings::Instance(), &Settings::PathAdded, &m_tracker, &GameTracker::AddDirectory);
  connect(&Settings::Instance(), &Settings::PathRemoved, &m_tracker, &GameTracker::RemoveDirectory);
  connect(&Settings::Instance(), &Settings::GameListRefreshRequested, &m_tracker,
          &GameTracker::RefreshAll);
  connect(&Settings::Instance(), &Settings::TitleDBReloadRequested,
          [this] { m_title_database = Core::TitleDatabase(); });

  for (const QString& dir : Settings::Instance().GetPaths())
    m_tracker.AddDirectory(dir);

  m_tracker.Start();

  // TODO: once I figure out the startup process better, load settings more effectively
  // For now, I'm just loading each game config to get the WiiMix state
  for (int i = 0; i < m_games.length(); i++)
  {
      const auto& game = m_games[i];
      
      // Load the ini file
      Common::IniFile ini;
      if (!File::Exists(File::GetUserPath(D_GAMESETTINGS_IDX) + game->GetGameID() + ".ini")) {
        // Create a new ini file and set the WiiMix state
        ini = SConfig::LoadDefaultGameIni(game->GetGameID(), game->GetRevision());
        ini.GetOrCreateSection("WiiMix")->Set("WiiMix", true);
        ini.Save(File::GetUserPath(D_GAMESETTINGS_IDX) + game->GetGameID() + ".ini");
      }
      else {
        ini = SConfig::LoadLocalGameIni(game->GetGameID(), game->GetRevision());
      }
      std::string stateStr;
      Common::IniFile::Section *section = ini.GetSection("WiiMix");
      if (section == nullptr) {
        qDebug() << "WiiMix section nullptr";
        // Create a new section and set the WiiMix state
        ini.GetOrCreateSection("WiiMix")->Set("WiiMix", true);
        ini.Save(File::GetUserPath(D_GAMESETTINGS_IDX) + game->GetGameID() + ".ini");
      }
      ini.GetSection("WiiMix")->Get("WiiMix", &stateStr);

      bool state_bool = stateStr == "True" ? true : false;

      // Create a mutable copy of the game object
      auto updated_game = std::make_shared<UICommon::GameFile>(*game); // Dereference to create a copy

      // Modify the mutable copy
      updated_game->SetWiiMix(state_bool);

      // Update the m_games list with the updated game
      m_games[i] = updated_game;
      WiiMixGlobalSettings::instance()->UpdateGame(updated_game);
      qDebug() << QString::fromStdString(game->GetGameID());
      qDebug() << game->GetWiiMix();
  }

  connect(&Settings::Instance(), &Settings::ThemeChanged, [this] {
    // Tell the view to repaint. The signal 'dataChanged' also seems like it would work here, but
    // unfortunately it won't cause a repaint until the view is focused.
    emit layoutAboutToBeChanged();
    emit layoutChanged();
  });

  auto& settings = Settings::GetQSettings();

  m_tag_list = settings.value(QStringLiteral("gamelist/tags")).toStringList();
  m_game_tags = settings.value(QStringLiteral("gamelist/game_tags")).toMap();
}

QVariant GameListModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  UICommon::GameFile& game = *std::const_pointer_cast<UICommon::GameFile>(m_games[index.row()]);

  switch (static_cast<Column>(index.column()))
  {
  case Column::WiiMix:
    if (role == Qt::CheckStateRole) {
      bool state = game.GetWiiMix();
      // Upon returning the state, the game specific config files get updated 
      return state ? Qt::Checked : Qt::Unchecked; // Ensures the checkbox appears
    }

    if (role == CheckAlignmentRole) {
        return Qt::AlignHCenter; // For custom alignment
    }

    // Optional fallback using Qt's default alignment role
    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignHCenter;
    }

    // Disable the checkbox for this row if there's no objectives for the game
    if (game.GetObjectives() == 0) {
        if (role == Qt::CheckStateRole) {
            return QVariant(); // Disable the checkbox
        }
    }

    // if (role == Qt::TextAlignmentRole)
    // {
    //     // Align center horizontally and vertically
    //     return Qt::AlignHCenter;
    // }
    // if (role == Qt::DisplayRole)
      // return QVariant();
    // if (role == SORT_ROLE)
    //   // return static_cast<int>(config_widget.GetWiiMix());
    //   return 2;
    break;
  case Column::Objectives:
    if (role == Qt::DisplayRole)
      return game.GetObjectives();
    if (role == SORT_ROLE)
      return game.GetObjectives();
    break;
  case Column::Platform:
    if (role == Qt::DecorationRole)
      return Resources::GetPlatform(game.GetPlatform()).pixmap(32, 32);
    if (role == SORT_ROLE)
      return static_cast<int>(game.GetPlatform());
    break;
  case Column::Country:
    if (role == Qt::DecorationRole)
      return Resources::GetCountry(game.GetCountry()).pixmap(32, 22);
    if (role == SORT_ROLE)
      return static_cast<int>(game.GetCountry());
    break;
  case Column::Banner:
    if (role == Qt::DecorationRole)
    {
      // GameCube banners are 96x32, but Wii banners are 192x64.
      QPixmap banner = ToQPixmap(game.GetBannerImage());
      if (banner.isNull())
        banner = Resources::GetMisc(Resources::MiscID::BannerMissing).pixmap(GAMECUBE_BANNER_SIZE);

      banner.setDevicePixelRatio(
          std::max(static_cast<qreal>(banner.width()) / GAMECUBE_BANNER_SIZE.width(),
                   static_cast<qreal>(banner.height()) / GAMECUBE_BANNER_SIZE.height()));

      return banner;
    }
    break;
  case Column::Title:
    if (role == Qt::DisplayRole || role == SORT_ROLE)
    {
      QString name = QString::fromStdString(game.GetName(m_title_database));

      const int disc_number = game.GetDiscNumber() + 1;
      if (disc_number > 1 || game.IsTwoDiscGame())
      {
        // Add disc number to title if not present.
        if (!name.contains(QRegularExpression(QStringLiteral("disc ?%1").arg(disc_number),
                                              QRegularExpression::CaseInsensitiveOption)))
        {
          name.append(tr(" (Disc %1)").arg(disc_number));
        }
      }

      // For natural sorting, pad all numbers to the same length.
      if (SORT_ROLE == role)
      {
        constexpr int MAX_NUMBER_LENGTH = 10;

        const QRegularExpression rx(QStringLiteral("\\d+"));
        QRegularExpressionMatch match;
        int pos = 0;
        while ((match = rx.match(name, pos)).hasMatch())
        {
          pos = match.capturedStart();
          name.replace(pos, match.capturedLength(),
                       match.captured().rightJustified(MAX_NUMBER_LENGTH));
          pos += MAX_NUMBER_LENGTH;
        }
      }

      return name;
    }
    break;
  case Column::ID:
    if (role == Qt::DisplayRole || role == SORT_ROLE)
      return QString::fromStdString(game.GetGameID());
    break;
  case Column::Description:
    if (role == Qt::DisplayRole || role == SORT_ROLE)
    {
      return QString::fromStdString(
                 game.GetDescription(UICommon::GameFile::Variant::LongAndPossiblyCustom))
          .replace(QLatin1Char('\n'), QLatin1Char(' '));
    }
    break;
  case Column::Maker:
    if (role == Qt::DisplayRole || role == SORT_ROLE)
    {
      return QString::fromStdString(
          game.GetMaker(UICommon::GameFile::Variant::LongAndPossiblyCustom));
    }
    break;
  case Column::FileName:
    if (role == Qt::DisplayRole || role == SORT_ROLE)
      return QString::fromStdString(game.GetFileName());
    break;
  case Column::FilePath:
    if (role == Qt::DisplayRole || role == SORT_ROLE)
    {
      QString file_path = QDir::toNativeSeparators(
          QFileInfo(QString::fromStdString(game.GetFilePath())).absolutePath());
      if (!file_path.endsWith(QDir::separator()))
        file_path.append(QDir::separator());
      return file_path;
    }
    break;
  case Column::Size:
    if (role == Qt::DisplayRole)
    {
      std::string str = UICommon::FormatSize(game.GetFileSize());

      // Add asterisk to size of compressed files.
      if (game.GetFileSize() != game.GetVolumeSize())
        str += '*';

      return QString::fromStdString(str);
    }
    if (role == SORT_ROLE)
      return static_cast<quint64>(game.GetFileSize());
    break;
  case Column::FileFormat:
    if (role == Qt::DisplayRole || role == SORT_ROLE)
      return QString::fromStdString(game.GetFileFormatName());
    break;
  case Column::BlockSize:
    if (role == Qt::DisplayRole)
      return QString::fromStdString(UICommon::FormatSize(game.GetBlockSize()));
    if (role == SORT_ROLE)
      return static_cast<quint64>(game.GetBlockSize());
    break;
  case Column::Compression:
    if (role == Qt::DisplayRole || role == SORT_ROLE)
    {
      const QString compression = QString::fromStdString(game.GetCompressionMethod());
      return compression.isEmpty() ? tr("No Compression") : compression;
    }
    break;
  case Column::Tags:
    if (role == Qt::DisplayRole || role == SORT_ROLE)
    {
      auto tags = GetGameTags(game.GetFilePath());
      tags.sort();

      return tags.join(QStringLiteral(", "));
    }
    break;
  default:
    break;
  }

  return QVariant();
}

Qt::ItemFlags GameListModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return Qt::NoItemFlags;

  Qt::ItemFlags flag = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if (index.column() == static_cast<int>(Column::WiiMix)) {
    flag |= Qt::ItemIsUserCheckable;
    // flag &= ~Qt::ItemIsSelectable;
    // #ifndef DEBUG
    //   if (m_games.at(index.row())->GetObjectives() == 0) {
    //     flag &= ~Qt::ItemIsUserCheckable;
    //     flag &= ~Qt::ItemIsEditable;
    //   }
    // #endif
  }

  return flag;
}

void GameListModel::setWiiMixData(std::vector<std::shared_ptr<const UICommon::GameFile>> games)
{
  for (int i = 0; i < m_games.size(); ++i)
  {
    const auto& game = m_games[i];
    bool found = false;

    for (const auto& new_game : games)
    {
      if (game->GetGameID() == new_game->GetGameID())
      {
        found = true;
        break;
      }
    }

    // Load the ini file
    Common::IniFile ini;
    if (!File::Exists(File::GetUserPath(D_GAMESETTINGS_IDX) + game->GetGameID() + ".ini"))
    {
      // Create a new ini file and set the WiiMix state
      ini = SConfig::LoadDefaultGameIni(game->GetGameID(), game->GetRevision());
    }
    else
    {
      ini = SConfig::LoadLocalGameIni(game->GetGameID(), game->GetRevision());
    }

    ini.GetOrCreateSection("WiiMix")->Set("WiiMix", found);
    ini.Save(File::GetUserPath(D_GAMESETTINGS_IDX) + game->GetGameID() + ".ini");

    // Create a mutable copy of the game object
    auto updated_game = std::make_shared<UICommon::GameFile>(*game);

    // Modify the mutable copy
    updated_game->SetWiiMix(found);

    // Update the m_games list with the updated game
    m_games[i] = updated_game;

    // Updated the global games list
    emit m_tracker.GameUpdated(updated_game);

    // Emit dataChanged signal
    emit dataChanged(createIndex(i, static_cast<int>(Column::WiiMix)), createIndex(i, static_cast<int>(Column::WiiMix)), {Qt::CheckStateRole});
  }
}

// Checkbox clicks automatically routed to SetData
bool GameListModel::setData(const QModelIndex& index, const QVariant& value, int role) 
{
  if (role == Qt::CheckStateRole && index.isValid()) {
      const UICommon::GameFile& game = *m_games[index.row()];

      bool state = game.GetWiiMix();

      auto updated_game = std::make_shared<UICommon::GameFile>(game);

      // Modify the mutable copy
      updated_game->SetWiiMix(!state);

      // Replace the old game with the new updated const game in m_games
      m_games[index.row()] = updated_game;

      // Update the config file
      if (!File::Exists(File::GetUserPath(D_GAMESETTINGS_IDX) + game.GetGameID() + ".ini")) {
        // Create a new ini file and set the WiiMix state
        Common::IniFile ini = SConfig::LoadDefaultGameIni(game.GetGameID(), game.GetRevision());
        ini.GetOrCreateSection("WiiMix")->Set("WiiMix", !state);
        ini.Save(File::GetUserPath(D_GAMESETTINGS_IDX) + game.GetGameID() + ".ini");
      }
      else {
        Common::IniFile ini = SConfig::LoadLocalGameIni(game.GetGameID(), game.GetRevision());
        ini.GetOrCreateSection("WiiMix")->Set("WiiMix", !state);
        ini.Save(File::GetUserPath(D_GAMESETTINGS_IDX) + game.GetGameID() + ".ini");
      }
      // qDebug() << QString::fromStdString(File::GetUserPath(D_GAMESETTINGS_IDX) + game.GetGameID() + ".ini");

      // Update the view
      emit dataChanged(index, index, {Qt::CheckStateRole});
      return true;
  }
  return false;
}

QVariant GameListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Vertical || role != Qt::DisplayRole)
    return QVariant();

  switch (static_cast<Column>(section))
  {
  case Column::WiiMix:
    return tr("Mix");
  case Column::Objectives:
    return tr("Objectives");
  // case Column::Platform:
  //   return tr("Platform");
  case Column::Title:
    return tr("Title");
  case Column::ID:
    return tr("ID");
  case Column::Banner:
    return tr("Banner");
  case Column::Description:
    return tr("Description");
  case Column::Maker:
    return tr("Maker");
  case Column::FileName:
    return tr("File Name");
  case Column::FilePath:
    return tr("File Path");
  case Column::Size:
    return tr("Size");
  case Column::FileFormat:
    return tr("File Format");
  case Column::BlockSize:
    return tr("Block Size");
  case Column::Compression:
    return tr("Compression");
  case Column::Tags:
    return tr("Tags");
  default:
    break;
  }
  return QVariant();
}

int GameListModel::rowCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return m_games.size();
}

int GameListModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid())
    return 0;
  return static_cast<int>(Column::Count);
}

bool GameListModel::ShouldDisplayGameListItem(int index) const
{
  const UICommon::GameFile& game = *m_games[index];

  if (!m_term.isEmpty())
  {
    const bool matches_title = QString::fromStdString(game.GetName(m_title_database))
                                   .contains(m_term, Qt::CaseInsensitive);
    const bool filename_visible = Config::Get(Config::MAIN_GAMELIST_COLUMN_FILE_NAME);
    const bool list_view_selected = Settings::Instance().GetPreferredView();
    const bool matches_filename =
        filename_visible && list_view_selected &&
        QString::fromStdString(game.GetFileName()).contains(m_term, Qt::CaseInsensitive);
    if (!(matches_title || matches_filename))
    {
      return false;
    }
  }

  const bool show_platform = [&game] {
    switch (game.GetPlatform())
    {
    case DiscIO::Platform::GameCubeDisc:
      return Config::Get(Config::MAIN_GAMELIST_LIST_GC);
    case DiscIO::Platform::WiiDisc:
      return Config::Get(Config::MAIN_GAMELIST_LIST_WII);
    case DiscIO::Platform::WiiWAD:
      return Config::Get(Config::MAIN_GAMELIST_LIST_WAD);
    case DiscIO::Platform::ELFOrDOL:
      return Config::Get(Config::MAIN_GAMELIST_LIST_ELF_DOL);
    default:
      return false;
    }
  }();

  if (!show_platform)
    return false;

  switch (game.GetCountry())
  {
  case DiscIO::Country::Australia:
    return Config::Get(Config::MAIN_GAMELIST_LIST_AUSTRALIA);
  case DiscIO::Country::Europe:
    return Config::Get(Config::MAIN_GAMELIST_LIST_PAL);
  case DiscIO::Country::France:
    return Config::Get(Config::MAIN_GAMELIST_LIST_FRANCE);
  case DiscIO::Country::Germany:
    return Config::Get(Config::MAIN_GAMELIST_LIST_GERMANY);
  case DiscIO::Country::Italy:
    return Config::Get(Config::MAIN_GAMELIST_LIST_ITALY);
  case DiscIO::Country::Japan:
    return Config::Get(Config::MAIN_GAMELIST_LIST_JPN);
  case DiscIO::Country::Korea:
    return Config::Get(Config::MAIN_GAMELIST_LIST_KOREA);
  case DiscIO::Country::Netherlands:
    return Config::Get(Config::MAIN_GAMELIST_LIST_NETHERLANDS);
  case DiscIO::Country::Russia:
    return Config::Get(Config::MAIN_GAMELIST_LIST_RUSSIA);
  case DiscIO::Country::Spain:
    return Config::Get(Config::MAIN_GAMELIST_LIST_SPAIN);
  case DiscIO::Country::Taiwan:
    return Config::Get(Config::MAIN_GAMELIST_LIST_TAIWAN);
  case DiscIO::Country::USA:
    return Config::Get(Config::MAIN_GAMELIST_LIST_USA);
  case DiscIO::Country::World:
    return Config::Get(Config::MAIN_GAMELIST_LIST_WORLD);
  case DiscIO::Country::Unknown:
  default:
    return Config::Get(Config::MAIN_GAMELIST_LIST_UNKNOWN);
  }
}

std::shared_ptr<const UICommon::GameFile> GameListModel::GetGameFile(int index) const
{
  return m_games[index];
}

std::string GameListModel::GetNetPlayName(const UICommon::GameFile& game) const
{
  return game.GetNetPlayName(m_title_database);
}

void GameListModel::AddGame(const std::shared_ptr<const UICommon::GameFile>& game)
{
  beginInsertRows(QModelIndex(), m_games.size(), m_games.size());
  m_games.push_back(game);
  endInsertRows();
}

void GameListModel::UpdateGame(const std::shared_ptr<const UICommon::GameFile>& game)
{
  int index = FindGameIndex(game->GetFilePath());
  if (index < 0)
  {
    AddGame(game);
  }
  else
  {
    m_games[index] = game;
    emit dataChanged(createIndex(index, 0), createIndex(index, columnCount(QModelIndex()) - 1));
  }
}

void GameListModel::RemoveGame(const std::string& path)
{
  int entry = FindGameIndex(path);
  if (entry < 0)
    return;

  beginRemoveRows(QModelIndex(), entry, entry);
  m_games.removeAt(entry);
  endRemoveRows();
}

std::shared_ptr<const UICommon::GameFile> GameListModel::FindGame(const std::string& path) const
{
  const int index = FindGameIndex(path);
  return index < 0 ? nullptr : m_games[index];
}

int GameListModel::FindGameIndex(const std::string& path) const
{
  for (int i = 0; i < m_games.size(); i++)
  {
    if (m_games[i]->GetFilePath() == path)
      return i;
  }
  return -1;
}

std::shared_ptr<const UICommon::GameFile>
GameListModel::FindSecondDisc(const UICommon::GameFile& game) const
{
  std::shared_ptr<const UICommon::GameFile> match_without_revision = nullptr;

  if (DiscIO::IsDisc(game.GetPlatform()))
  {
    for (auto& other_game : m_games)
    {
      if (game.GetGameID() == other_game->GetGameID() &&
          game.GetDiscNumber() != other_game->GetDiscNumber())
      {
        if (game.GetRevision() == other_game->GetRevision())
          return other_game;
        else
          match_without_revision = other_game;
      }
    }
  }

  return match_without_revision;
}

void GameListModel::SetSearchTerm(const QString& term)
{
  m_term = term;
}

void GameListModel::SetScale(float scale)
{
  m_scale = scale;
}

float GameListModel::GetScale() const
{
  return m_scale;
}

const QStringList& GameListModel::GetAllTags() const
{
  return m_tag_list;
}

const QStringList GameListModel::GetGameTags(const std::string& path) const
{
  return m_game_tags[QString::fromStdString(path)].toStringList();
}

void GameListModel::AddGameTag(const std::string& path, const QString& name)
{
  auto tags = GetGameTags(path);

  if (tags.contains(name))
    return;

  tags << name;

  m_game_tags[QString::fromStdString(path)] = tags;
  Settings::GetQSettings().setValue(QStringLiteral("gamelist/game_tags"), m_game_tags);
}

void GameListModel::RemoveGameTag(const std::string& path, const QString& name)
{
  auto tags = GetGameTags(path);

  tags.removeAll(name);

  m_game_tags[QString::fromStdString(path)] = tags;

  Settings::GetQSettings().setValue(QStringLiteral("gamelist/game_tags"), m_game_tags);
}

void GameListModel::NewTag(const QString& name)
{
  if (m_tag_list.contains(name))
    return;

  m_tag_list << name;

  Settings::GetQSettings().setValue(QStringLiteral("gamelist/tags"), m_tag_list);
}

void GameListModel::DeleteTag(const QString& name)
{
  m_tag_list.removeAll(name);

  for (const auto& file : m_game_tags.keys())
    RemoveGameTag(file.toStdString(), name);

  Settings::GetQSettings().setValue(QStringLiteral("gamelist/tags"), m_tag_list);
}

void GameListModel::PurgeCache()
{
  m_tracker.PurgeCache();
}