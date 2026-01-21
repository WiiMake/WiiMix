// Copyright 2008 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinNoGUI/Platform.h"

#include <OptionParser.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <signal.h>
#include <string>
#include <vector>

#ifndef _WIN32
#include <unistd.h>
#else
#include <Windows.h>
#endif

#include "Common/ScopeGuard.h"
#include "Common/StringUtil.h"
#include "Core/Boot/Boot.h"
#include "Core/BootManager.h"
#include "Core/Core.h"
#include "Core/Config/MainSettings.h"
#include "Core/DolphinAnalytics.h"
#include "Core/Host.h"
#include "Core/System.h"

#include "UICommon/CommandLineParse.h"
#ifdef USE_DISCORD_PRESENCE
#include "UICommon/DiscordPresence.h"
#endif
#include "UICommon/UICommon.h"

#include "InputCommon/GCAdapter.h"

#include "VideoCommon/VideoBackendBase.h"
#include "Core/State.h"
#include <DolphinQt/Host.h>
#include <chrono>
#include <thread> 

static std::unique_ptr<Platform> s_platform;

static void signal_handler(int)
{
  const char message[] = "A signal was received. A second signal will force Dolphin to stop.\n";
#ifdef _WIN32
  puts(message);
#else
  if (write(STDERR_FILENO, message, sizeof(message)) < 0)
  {
  }
#endif

  s_platform->RequestShutdown();
}

std::vector<std::string> Host_GetPreferredLocales()
{
  return {};
}

void Host_PPCSymbolsChanged()
{
}

void Host_RefreshDSPDebuggerWindow()
{
}

bool Host_UIBlocksControllerState()
{
  return false;
}

static Common::Event s_update_main_frame_event;
void Host_Message(HostMessageID id)
{
  if (id == HostMessageID::WMUserStop)
    s_platform->Stop();
}

void Host_UpdateTitle(const std::string& title)
{
  s_platform->SetTitle(title);
}

void Host_UpdateDisasmDialog()
{
}

void Host_UpdateMainFrame()
{
  s_update_main_frame_event.Set();
}

void Host_RequestRenderWindowSize(int width, int height)
{
}

bool Host_RendererHasFocus()
{
  return s_platform->IsWindowFocused();
}

bool Host_RendererHasFullFocus()
{
  // Mouse capturing isn't implemented
  return Host_RendererHasFocus();
}

bool Host_RendererIsFullscreen()
{
  return s_platform->IsWindowFullscreen();
}

bool Host_TASInputHasFocus()
{
  return false;
}

void Host_YieldToUI()
{
}

void Host_TitleChanged()
{
#ifdef USE_DISCORD_PRESENCE
  Discord::UpdateDiscordPresence();
#endif
}

void Host_UpdateDiscordClientID(const std::string& client_id)
{
#ifdef USE_DISCORD_PRESENCE
  Discord::UpdateClientID(client_id);
#endif
}

bool Host_UpdateDiscordPresenceRaw(const std::string& details, const std::string& state,
                                   const std::string& large_image_key,
                                   const std::string& large_image_text,
                                   const std::string& small_image_key,
                                   const std::string& small_image_text,
                                   const int64_t start_timestamp, const int64_t end_timestamp,
                                   const int party_size, const int party_max)
{
#ifdef USE_DISCORD_PRESENCE
  return Discord::UpdateDiscordPresenceRaw(details, state, large_image_key, large_image_text,
                                           small_image_key, small_image_text, start_timestamp,
                                           end_timestamp, party_size, party_max);
#else
  return false;
#endif
}

std::unique_ptr<GBAHostInterface> Host_CreateGBAHost(std::weak_ptr<HW::GBA::Core> core)
{
  return nullptr;
}

static std::unique_ptr<Platform> GetPlatform(const optparse::Values& options)
{
  std::string platform_name = static_cast<const char*>(options.get("platform"));

#if HAVE_X11
  if (platform_name == "x11" || platform_name.empty())
    return Platform::CreateX11Platform();
#endif

#ifdef __linux__
  if (platform_name == "fbdev" || platform_name.empty())
    return Platform::CreateFBDevPlatform();
#endif

#ifdef _WIN32
  if (platform_name == "win32" || platform_name.empty())
    return Platform::CreateWin32Platform();
#endif
#ifdef __APPLE__
  if (platform_name == "macos" || platform_name.empty())
    return Platform::CreateMacOSPlatform();
#endif

  if (platform_name == "headless" || platform_name.empty())
    return Platform::CreateHeadlessPlatform();

  return nullptr;
}

#ifdef _WIN32
#define main app_main
#endif

int main(int argc, char* argv[])
{
  // Disable buffering for stdout and stderr (for mac os testing primarily)
  setvbuf(stdout, nullptr, _IONBF, 0);
  setvbuf(stderr, nullptr, _IONBF, 0);

  Core::DeclareAsHostThread();

  auto parser = CommandLineParse::CreateParser(CommandLineParse::ParserOptions::OmitGUIOptions);
  parser->add_option("-p", "--platform")
      .action("store")
      .help("Window platform to use [%choices]")
      .choices({
        "headless"
#ifdef __linux__
            ,
            "fbdev"
#endif
#if HAVE_X11
            ,
            "x11"
#endif
#ifdef _WIN32
            ,
            "win32"
#endif
#ifdef __APPLE__
            ,
            "macos"
#endif
      });
  
  // Force true for testing on headless (at least for the time being)
  parser->add_option("-dt", "--diff-test")
      .action("store_true")
      .help("Run in headless diff test mode (for automated testing)");

  // Add Identity Test Flag
  parser->add_option("-it", "--identity-test")
      .action("store_true")
      .help("Run WiiMix Identity Test (Save A -> Load A -> Save B -> Compare)");

  // Add Poison Mask Flag
  parser->add_option("--poison")
      .action("store")
      .help("Bitmask (Int/Hex) for subsystem poisoning to detect missing state data");

  // Add jitter flag
  parser->add_option("--jitter").action("store").help("adapt memory and other host-architecture elements to simulate being on a different system");

  // parser->add_option("-fr", "--frames")
  //     .help("Specify the number of frames to step when run in Diff Test mode (WiiMix feature)");

  parser->add_option("-i", "--interpreter")
      .action("store_true")
      .help("Use the interpreter core for PowerPC emulation; otherwise use JIT for the corresponding architecture");

  optparse::Values& options = CommandLineParse::ParseArguments(parser.get(), argc, argv);
  std::vector<std::string> args = parser->args();

  std::optional<std::string> save_state_path;
  if (options.is_set("save_state"))
  {
    save_state_path = static_cast<const char*>(options.get("save_state"));
  }

  // --- Apply Poison Mask Settings ---
  if (options.is_set("poison"))
  {
    const std::string val = static_cast<const char*>(options.get("poison"));
    try {
        int mask = std::stoi(val, nullptr, 0); 
        
        // FIX: Use State::SetWiiMixPoisonMask instead of Config::SetBaseOrCurrent
        State::SetWiiMixPoisonMask(mask);
        
        printf("POISON: Mask set to 0x%08X via CLI\n", mask);
    } catch (...) {
        fprintf(stderr, "Invalid poison mask format\n");
    }
  }

  // Apply jitter settings
  if (options.is_set("jitter")) {
    State::SetWiiMixJitterHostEnvironment(true);
  }

  std::unique_ptr<BootParameters> boot;
  bool game_specified = false;
  if (options.is_set("exec"))
  {
    const std::list<std::string> paths_list = options.all("exec");
    const std::vector<std::string> paths{std::make_move_iterator(std::begin(paths_list)),
                                         std::make_move_iterator(std::end(paths_list))};
    boot = BootParameters::GenerateFromFile(
        paths, BootSessionData(save_state_path, DeleteSavestateAfterBoot::No));
    game_specified = true;
  }
  else if (options.is_set("nand_title"))
  {
    const std::string hex_string = static_cast<const char*>(options.get("nand_title"));
    if (hex_string.length() != 16)
    {
      fprintf(stderr, "Invalid title ID\n");
      parser->print_help();
      return 1;
    }
    const u64 title_id = std::stoull(hex_string, nullptr, 16);
    boot = std::make_unique<BootParameters>(BootParameters::NANDTitle{title_id});
  }
  else if (args.size())
  {
    boot = BootParameters::GenerateFromFile(
        args.front(), BootSessionData(save_state_path, DeleteSavestateAfterBoot::No));
    args.erase(args.begin());
    game_specified = true;
  }
  else
  {
    parser->print_help();
    return 0;
  }

  std::string user_directory;
  if (options.is_set("user"))
    user_directory = static_cast<const char*>(options.get("user"));

  s_platform = GetPlatform(options);
  if (!s_platform || !s_platform->Init())
  {
    fprintf(stderr, "No platform found, or failed to initialize.\n");
    return 1;
  }

  const WindowSystemInfo wsi = s_platform->GetWindowSystemInfo();

  UICommon::SetUserDirectory(user_directory);
  UICommon::Init();

  Common::ScopeGuard ui_common_guard([] {
    UICommon::ShutdownControllers();
    UICommon::Shutdown();
  });

  if (save_state_path && !game_specified)
  {
    fprintf(stderr, "A save state cannot be loaded without specifying a game to launch.\n");
    return 1;
  }

  Core::AddOnStateChangedCallback([](Core::State state) {
    if (state == Core::State::Uninitialized)
      s_platform->Stop();
  });

#ifdef _WIN32
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
#else
  // Shut down cleanly on SIGINT and SIGTERM
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESETHAND;
  sigaction(SIGINT, &sa, nullptr);
  sigaction(SIGTERM, &sa, nullptr);
#endif

  // Force enable MMU for WiiMix savestate compatibility
  Config::SetBaseOrCurrent(Config::MAIN_MMU, true); 

  DolphinAnalytics::Instance().ReportDolphinStart("nogui");

  UICommon::InitControllers(wsi);

  if (!BootManager::BootCore(Core::System::GetInstance(), std::move(boot), wsi))
  {
    fprintf(stderr, "Could not boot the specified file\n");
    return 1;
  }

#ifdef USE_DISCORD_PRESENCE
  Discord::UpdateDiscordPresence();
#endif

  // --- WiiMix Testing Logic ---
  // Shared logic for both Diff Tests and Identity Tests
  bool diff_test = options.is_set("diff_test");
  bool identity_test = options.is_set("identity_test");

  if (diff_test || identity_test)
  {
    // --- 1. Environment Setup ---
    
    // Set the corresponding interpreter
    if (options.is_set("interpreter"))
    {
      printf("INTERPRETER IS SET\n");
      State::WIIMIX_DIFF_TEST_CPU_CORE = PowerPC::CPUCore::Interpreter;
    }
    else
    {
      #if defined(__aarch64__) || defined(_M_ARM64)
        State::WIIMIX_DIFF_TEST_CPU_CORE = PowerPC::CPUCore::JITARM64;
      #else
        State::WIIMIX_DIFF_TEST_CPU_CORE = PowerPC::CPUCore::JIT64;
      #endif
    }
    
    // Force Single Core for Determinism
    Config::SetBaseOrCurrent(Config::MAIN_CPU_THREAD, false);

    // Update Core determinism settings
    Core::UpdateWantDeterminism(Core::System::GetInstance(), true); 
    
    // --- 2. Boot Wait Loop ---
    // We (the MainThread) must wait for the EmuThread to finish booting
    // and enter its 'Running' state. This polling loop solves the race condition.
    int retries = 0;
    while (!Core::IsRunning(Core::System::GetInstance()) && retries < 200) // 10-second timeout
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(50));
      retries++;
    }

    // Check if the emulator successfully booted
    if (!Core::IsRunning(Core::System::GetInstance()))
    {
       fprintf(stderr, "Test FAILED: Emulator failed to start running.\n");
       fflush(stderr);
       return 1; 
    }
    
    // --- 3. Execute Specific Test ---
    int test_result = 0;
    
    if (identity_test)
    {
        // Run Identity Test (A -> Load -> A')
        test_result = ::State::WiiMixIdentityTest(Core::System::GetInstance());
    }
    else if (diff_test)
    {
        // Run Diff Test (A -> Step -> B vs A -> Load -> Step -> C)
        int frames = 1;
        if (options.is_set("frames"))
        {
          const std::string frames_str = static_cast<const char*>(options.get("frames"));
          frames = std::stoi(frames_str);
          if (frames <= 0)
          {
            fprintf(stderr, "Invalid number of frames specified for Diff Test.\n");
            return 1;
          }
        }
        test_result = ::State::WiiMixDiffTest(Core::System::GetInstance(), frames);
    }

    // --- 4. Cleanup & Exit ---
    Core::Shutdown(Core::System::GetInstance());
    
    printf("s_platform.reset() in main\n");
    s_platform.reset(); // Clean up platform resources

    fflush(stdout);
    fflush(stderr);
    
    return test_result;
  }

  s_platform->MainLoop();
  Core::Stop(Core::System::GetInstance());

  Core::Shutdown(Core::System::GetInstance());
  s_platform.reset();

  return 0;
}

#ifdef _WIN32
int wmain(int, wchar_t*[], wchar_t*[])
{
  std::vector<std::string> args = Common::CommandLineToUtf8Argv(GetCommandLineW());
  const int argc = static_cast<int>(args.size());
  std::vector<char*> argv(args.size());
  for (size_t i = 0; i < args.size(); ++i)
    argv[i] = args[i].data();

  return main(argc, argv.data());
}

#undef main
#endif