// Copyright 2015 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QMainWindow>
#include <QStringList>
#include <QGroupBox>
#include <QHBoxLayout>

#include <array>
#include <memory>
#include <optional>
#include <string>

#include "Core/Boot/Boot.h"

#include "DolphinQt/WiiMix/BingoSettings.h"
#include "DolphinQt/WiiMix/RogueSettings.h"
#include "DolphinQt/WiiMix/ShuffleSettings.h"
#include "DolphinQt/WiiMix/GameManager.h"
#include "DolphinQt/WiiMix/Client.h"
#include "DolphinQt/WiiMix/Objective.h"
#include "DolphinQt/WiiMix/WiiMixButton.h"
#include "DolphinQt/WiiMix/ModesWidget.h"

class QMenu;
class QStackedWidget;
class QString;

class AchievementsWindow;
class AssemblerWidget;
class BreakpointWidget;
struct BootParameters;
class CheatsManager;
class CodeWidget;
class ControllersWindow;
class DiscordHandler;
class DragEnterEvent;
class FIFOPlayerWindow;
class FreeLookWindow;
class GameList;
class GBATASInputWindow;
class GCTASInputWindow;
class GraphicsWindow;
class HotkeyScheduler;
class InfinityBaseWindow;
class JITWidget;
class LogConfigWidget;
class LogWidget;
class MappingWindow;
class MemoryWidget;
class MenuBar;
class NetPlayDialog;
class NetPlaySetupDialog;
class NetworkWidget;
class RegisterWidget;
class RenderWidget;
class SearchBar;
class SettingsWindow;
class SkylanderPortalWindow;
class ThreadWidget;
class ToolBar;
class WatchWidget;
class WiiMixSettingsWindow;
class WiiMixAccountLoginWindow;
class WiiMixAccountWindow;
class WiiMixStateSendMenu;
class WiiTASInputWindow;
struct WindowSystemInfo;

namespace DiscIO
{
enum class Region;
}

namespace UICommon
{
class GameFile;
}

namespace X11Utils
{
class XRRConfiguration;
}

class MainWindow final : public QMainWindow
{
  Q_OBJECT

public:
    QStackedWidget* m_stack;
  explicit MainWindow(std::unique_ptr<BootParameters> boot_parameters,
                      const std::string& movie_path);
  ~MainWindow();

  WindowSystemInfo GetWindowSystemInfo() const;

  bool eventFilter(QObject* object, QEvent* event) override;
  QMenu* createPopupMenu() override;

signals:
  void ReadOnlyModeChanged(bool read_only);
  void RecordingStatusChanged(bool recording);
  void onStateSendProgressUpdate(QString progress_text);
  

public slots:
  void WiiMixStartObjective(WiiMixObjective new_objective);
  void WiiMixStartObjective(WiiMixObjective new_objective, std::string save_path);  // not expected to call from signal, mostly for
                                                                                    // sharing code between these functions ~ @gyoder
  void WiiMixSwapObjective(WiiMixObjective new_objective, WiiMixObjective current_objective);
  void WiiMixRestartObjective(WiiMixObjective new_objective);
  void WiiMixRestartObjective(WiiMixObjective new_objective, WiiMixObjective current_objective);
  // void DisplayClientError(QString error);

private:
  void ShowWiiMixWindow();
  void Open();
  // void RefreshGameList();
  void Play(const std::optional<std::string>& savestate_path = {});
  void Pause();
  void TogglePause();

  // May ask for confirmation. Returns whether or not it actually stopped.
  bool RequestStop();
  void ForceStop();
  void Reset();
  void FrameAdvance();
  void StateLoad();
  void StateSave();
  void StateLoadSlot();
  void StateSaveSlot();
  // void ObjectiveLoadSlot(int slot);
  void StateSendSlot();
  // void GameSwapSlot();
  void StateLoadSlotAt(int slot);
  void StateSaveSlotAt(int slot);
  void StateSend(WiiMixObjective objective);
  void TrackStateSendProgress(qint64 bytesWritten, qint64 totalBytes);
  void TrackStateReadProgress(qint64 bytesWritten, qint64 totalBytes);
  void HandleAchievementGet(std::set<uint32_t> achievements);
  void ShowStateSendMenu(int slot);
  void ObjectiveResetSlotAt(int slot);
  void ResetCurrentObjective();
  void BingoReady();
  void ToggleBingoBoard();
  void StopWiiMix();
  // void GameSwapSlotAt(int slot);
  void StateLoadLastSavedAt(int slot);
  void StateLoadUndo();
  void StateSaveUndo();
  void StateSaveOldest();
  void SetStateSlot(int slot);
  // void IncrementSelectedStateSlot();
  // void DecrementSelectedStateSlot();
  void BootWiiSystemMenu();

  void PerformOnlineUpdate(const std::string& region);

  void SetFullScreenResolution(bool fullscreen);

  void FullScreen();
  void UnlockCursor();
  void ScreenShot();

  void CreateComponents();

  void ConnectGameList();
  void ConnectHost();
  void ConnectHotkeys();
  void ConnectWiiMix();
  void ConnectMenuBar();
  void ConnectRenderWidget();
  void ConnectStack();
  void ConnectToolBar();

  void InitControllers();
  void ShutdownControllers();

  void InitCoreCallbacks();

  enum class ScanForSecondDisc
  {
    Yes,
    No,
  };

  void ScanForSecondDiscAndStartGame(const UICommon::GameFile& game,
                                     std::unique_ptr<BootSessionData> boot_session_data = nullptr);
  void StartGame(const QString& path, ScanForSecondDisc scan,
                 std::unique_ptr<BootSessionData> boot_session_data = nullptr);
  void StartGame(const std::string& path, ScanForSecondDisc scan,
                 std::unique_ptr<BootSessionData> boot_session_data = nullptr);
  void StartGame(const std::vector<std::string>& paths,
                 std::unique_ptr<BootSessionData> boot_session_data = nullptr);
  void StartGame(std::unique_ptr<BootParameters>&& parameters);
  void StartGame(std::unique_ptr<BootParameters>&& parameters,
                 std::string save_path);
  void PopulateWiiMixBingoObjectives(WiiMixBingoSettings* settings);
  void PopulateWiiMixRogueObjectives(WiiMixRogueSettings* settings);
  void PopulateWiiMixShuffleObjectives(WiiMixShuffleSettings* settings);
  void StartWiiMixBingo(WiiMixBingoSettings* settings);
  void StartWiiMixRogue(WiiMixRogueSettings* settings);
  void StartWiiMixShuffle(WiiMixShuffleSettings* settings);
  void WiiMixShuffleUpdate();
  // void WiiMixShowcase(WiiMixBingoSettings settings);
  void ShowRenderWidget();
  void HideRenderWidget(bool reinit = true, bool is_exit = false);

  void ShowSettingsWindow();
  void ShowGeneralWindow();
  void ShowAudioWindow();
  void ShowControllersWindow();
  void ShowGraphicsWindow();
  void ShowFreeLookWindow();
  void ShowAboutDialog();
  void ShowHotkeyDialog();
  void ShowNetPlaySetupDialog();
  void ShowNetPlayBrowser();
  void ShowFIFOPlayer();
  void ShowSkylanderPortal();
  void ShowInfinityBase();
  void ShowMemcardManager();
  void ShowResourcePackManager();
  void ShowCheatsManager();
  void ShowRiivolutionBootWidget(const UICommon::GameFile& game);

#ifdef USE_RETRO_ACHIEVEMENTS
  // void ShowAchievementsWindow();
  void ShowAchievementSettings();
#endif  // USE_RETRO_ACHIEVEMENTS
  void ShowWiiMixAccountWindow();

  void NetPlayInit();
  bool NetPlayJoin();
  bool NetPlayHost(const UICommon::GameFile& game);
  void NetPlayQuit();

  void OnBootGameCubeIPL(DiscIO::Region region);
  void OnImportNANDBackup();
  void OnConnectWiiRemote(int id);

#if defined(__unix__) || defined(__unix) || defined(__APPLE__)
  void OnSignal();
#endif

  // void OnPlayRecording();
  // void OnStartRecording();
  // void OnStopRecording();
  // void OnExportRecording();
  // void OnActivateChat();
  // void OnRequestGolfControl();
  void ShowTASInput();

  void ChangeDisc();
  void EjectDisc();

  void OpenUserFolder();

  QStringList PromptFileNames();

  void UpdateScreenSaverInhibition();

  void OnStopComplete();
  // void dragEnterEvent(QDragEnterEvent* event) override;
  // void dropEvent(QDropEvent* event) override;
  QSize sizeHint() const override;

#ifdef _WIN32
  // This gets called for each event from the Windows message queue.
  bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
#endif

#ifdef HAVE_XRANDR
  std::unique_ptr<X11Utils::XRRConfiguration> m_xrr_config;
#endif

  // ToolBar* m_tool_bar;
  // MenuBar* m_menu_bar;
  // SearchBar* m_search_bar;
  GameList* m_game_list;
  RenderWidget* m_render_widget = nullptr;
  bool m_rendering_to_main;
  bool m_stop_confirm_showing = false;
  bool m_stop_requested = false;
  bool m_exit_requested = false;
  bool m_fullscreen_requested = false;
  bool m_load_slot_on_start = 0;
  bool m_is_screensaver_inhibited = false;
  u32 m_state_slot = 1;
  std::unique_ptr<BootParameters> m_pending_boot;

  // WiiMixSettingsWindow* m_wiimix_window = nullptr;
  // WiiMixStateSendMenu* m_state_send_menu = nullptr;
  // ControllersWindow* m_controllers_window = nullptr;
  // SettingsWindow* m_settings_window = nullptr;
  // GraphicsWindow* m_graphics_window = nullptr;
  // FIFOPlayerWindow* m_fifo_window = nullptr;
  // SkylanderPortalWindow* m_skylander_window = nullptr;
  // InfinityBaseWindow* m_infinity_window = nullptr;
  // MappingWindow* m_hotkey_window = nullptr;
  // FreeLookWindow* m_freelook_window = nullptr;

  // WiiMixBingoSettings* m_bingo_settings = nullptr;
  QString m_player_name = QStringLiteral("");
  int m_player_num = 0;
  bool m_player_ready = false;
  // QGraphicsTextItem* m_ready_text = nullptr; // TODOx: visual studio was very mad with me about this. not quite sure what it is used for but you can ask @gyoder to help debug on windows
  // bool m_bingo_started = false;
  WiiMixClient* m_wiimix_client = nullptr;
  WiiMixShuffleSettings* m_shuffle_settings = nullptr;
  WiiMixRogueSettings* m_rogue_settings = nullptr;
  QTimer* m_objective_timer = nullptr;

  HotkeyScheduler* m_hotkey_scheduler;
  // NetPlayDialog* m_netplay_dialog;
  // DiscordHandler* m_netplay_discord;
  // NetPlaySetupDialog* m_netplay_setup_dialog;
  static constexpr int num_gc_controllers = 4;
  // std::array<GCTASInputWindow*, num_gc_controllers> m_gc_tas_input_windows{};
  // std::array<GBATASInputWindow*, num_gc_controllers> m_gba_tas_input_windows{};
  static constexpr int num_wii_controllers = 4;
  // std::array<WiiTASInputWindow*, num_wii_controllers> m_wii_tas_input_windows{};

// #ifdef USE_RETRO_ACHIEVEMENTS
//   AchievementsWindow* m_achievements_window = nullptr;
// #endif  // USE_RETRO_ACHIEVEMENTS

  // WiiMixAccountWindow* m_wiimix_account_window = nullptr;
  // WiiMixAccountLoginWindow* m_wiimix_account_login_window = nullptr;

  // AssemblerWidget* m_assembler_widget;
  // BreakpointWidget* m_breakpoint_widget;
  // CodeWidget* m_code_widget;
  // JITWidget* m_jit_widget;
  // LogWidget* m_log_widget;
  // LogConfigWidget* m_log_config_widget;
  // MemoryWidget* m_memory_widget;
  // NetworkWidget* m_network_widget;
  // RegisterWidget* m_register_widget;
  // ThreadWidget* m_thread_widget;
  // WatchWidget* m_watch_widget;
  // CheatsManager* m_cheats_manager;
  QByteArray m_render_widget_geometry;

  WiiMixEnums::Mode m_mode;

  // ================ Settings Widget ================
  // QPushButton* m_load_button_box;
  // QPushButton* m_save_button_box;
  WiiMixLogoButton* m_wii_mix_button;

  // ================ Modes Widget ================
  WiiMixModesWidget* m_modes_widget;
};
