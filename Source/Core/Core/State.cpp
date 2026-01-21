// Copyright 2008 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Core/State.h"

#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <filesystem>
#include <locale>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <lz4.h>
#include <lzo/lzo1x.h>

#include "AudioCommon/AudioCommon.h"

#include "Common/ChunkFile.h"
#include "Common/CommonTypes.h"
#include "Common/Event.h"
#include "Common/FileUtil.h"
#include "Common/IOFile.h"
#include "Common/MsgHandler.h"
#include "Common/Thread.h"
#include "Common/TimeUtil.h"
#include "Common/Timer.h"
#include "Common/Version.h"
#include "Common/WorkQueueThread.h"

#include "Core/AchievementManager.h"
#include "Core/Config/AchievementSettings.h"
#include "Core/Config/MainSettings.h"
#include "Core/ConfigManager.h"
#include "Core/Core.h"
#include "UICommon/UICommon.h"
#include "Core/CoreTiming.h"
#include "Core/GeckoCode.h"
#include "Core/HW/CPU.h"
#include "Core/HW/HW.h"
#include "Core/HW/DVD/DVDInterface.h"
#include "Core/HW/DVD/DVDThread.h"
#include "Core/HW/Memmap.h"
#include "Core/HW/Wiimote.h"
#include "Core/Host.h"
#include "Core/Movie.h"
#include "Core/NetPlayClient.h"
#include "Core/PowerPC/PowerPC.h"
#include "Core/PowerPC/JitInterface.h"
#include "Core/System.h"
#include "Core/HW/DSP.h"
#include "Core/HW/AudioInterface.h"
#include "Core/HW/MemoryInterface.h"
#include "Core/HW/ProcessorInterface.h"
#include "Core/HW/VideoInterface.h"
#include "Core/HW/SI/SI.h"
#include "Core/HW/GPFifo.h"
#include "Core/HW/HSP/HSP.h"
#include "AudioCommon/SoundStream.h"

#include "VideoCommon/Fifo.h"
#include "VideoCommon/FrameDumpFFMpeg.h"
#include "VideoCommon/OnScreenDisplay.h"
#include "VideoCommon/VideoBackendBase.h"
#include "VideoCommon/BPMemory.h" // For bpMem
#include "VideoCommon/PixelEngine.h"
#include "VideoCommon/VertexLoaderManager.h"
#include "VideoCommon/VertexManagerBase.h"
#include "VideoCommon/CPMemory.h"  // For g_main_cp_state, g_preprocess_cp_state
#include "VideoCommon/XFMemory.h"  // For xfmem
#include "VideoCommon/TMEM.h"      // For tmMem / s_tex_mem (depending on your version)
#include "VideoCommon/TextureDecoder.h" // For s_tex_mem (depending on your version)
#include "VideoCommon/VertexShaderManager.h"
#include "VideoCommon/PixelShaderManager.h"
#include "VideoCommon/GeometryShaderManager.h"
#include "VideoCommon/XFStateManager.h"
#include "VideoCommon/CPMemory.h"

#include <signal.h>
#define TEST_FAIL(exit_code, ...) \
  fprintf(stderr, "Diff Test FAILED at %s:%d: ", __FILE__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
  fprintf(stderr, "\n"); \
  fflush(stderr);

namespace State
{

  static int s_poison_mask = 0; // Default to 0 (No Poison)

  void SetWiiMixPoisonMask(int mask)
  {
      s_poison_mask = mask;
  }

  bool WIIMIX_JITTER = false;
  PowerPC::CPUCore WIIMIX_DIFF_TEST_CPU_CORE;
  bool WIIMIX_DIFF_TEST = false;
  bool WIIMIX_LOG = false;
#if defined(__LZO_STRICT_16BIT)
static const u32 IN_LEN = 8 * 1024u;
#elif defined(LZO_ARCH_I086) && !defined(LZO_HAVE_MM_HUGE_ARRAY)
static const u32 IN_LEN = 60 * 1024u;
#else
static const u32 IN_LEN = 128 * 1024u;
#endif

bool diff_test = false;

static const u32 OUT_LEN = IN_LEN + (IN_LEN / 16) + 64 + 3;

static unsigned char __LZO_MMODEL out[OUT_LEN];

static AfterLoadCallbackFunc s_on_after_load_callback;

// Temporary undo state buffer
static std::vector<u8> s_undo_load_buffer;
static std::mutex s_undo_load_buffer_mutex;

static std::mutex s_load_or_save_in_progress_mutex;

struct CompressAndDumpState_args
{
  std::vector<u8> buffer_vector;
  std::string filename;
  std::shared_ptr<Common::Event> state_write_done_event;
};

// Protects against simultaneous reads and writes to the final savestate location from multiple
// threads.
static std::mutex s_save_thread_mutex;
// static std::mutex s_wiimix_save_thread_mutex;

// Queue for compressing and writing savestates to disk.
static Common::WorkQueueThread<CompressAndDumpState_args> s_save_thread;
static Common::WorkQueueThread<CompressAndDumpState_args> s_wiimix_save_thread;

// Keeps track of savestate writes that are currently happening, so we don't load a state while
// another one is still saving. This is particularly important so if you save to a slot and then
// immediately load from the same one, you don't accidentally load the state that's still at that
// file path before the write is done.
static std::mutex s_state_writes_in_queue_mutex;
static size_t s_state_writes_in_queue;
static std::condition_variable s_state_write_queue_is_empty;

// Don't forget to increase this after doing changes on the savestate system
constexpr u32 STATE_VERSION = 168;  // Last changed in PR 12639

// Increase this if the StateExtendedHeader definition changes
constexpr u32 EXTENDED_HEADER_VERSION = 1;  // Last changed in PR 12217

// Change this if we ever need to store more data in the extended header
constexpr u32 COMPRESSED_DATA_OFFSET = 0;

constexpr u32 COOKIE_BASE = 0xBAADBABE;

// Maps savestate versions to Dolphin versions.
// Versions after 42 don't need to be added to this list,
// because they save the exact Dolphin version to savestates.
static const std::map<u32, std::pair<std::string, std::string>> s_old_versions = {
    // The 16 -> 17 change modified the size of StateHeader,
    // so versions older than that can't even be decompressed anymore
    {17, {"3.5-1311", "3.5-1364"}}, {18, {"3.5-1366", "3.5-1371"}}, {19, {"3.5-1372", "3.5-1408"}},
    {20, {"3.5-1409", "4.0-704"}},  {21, {"4.0-705", "4.0-889"}},   {22, {"4.0-905", "4.0-1871"}},
    {23, {"4.0-1873", "4.0-1900"}}, {24, {"4.0-1902", "4.0-1919"}}, {25, {"4.0-1921", "4.0-1936"}},
    {26, {"4.0-1939", "4.0-1959"}}, {27, {"4.0-1961", "4.0-2018"}}, {28, {"4.0-2020", "4.0-2291"}},
    {29, {"4.0-2293", "4.0-2360"}}, {30, {"4.0-2362", "4.0-2628"}}, {31, {"4.0-2632", "4.0-3331"}},
    {32, {"4.0-3334", "4.0-3340"}}, {33, {"4.0-3342", "4.0-3373"}}, {34, {"4.0-3376", "4.0-3402"}},
    {35, {"4.0-3409", "4.0-3603"}}, {36, {"4.0-3610", "4.0-4480"}}, {37, {"4.0-4484", "4.0-4943"}},
    {38, {"4.0-4963", "4.0-5267"}}, {39, {"4.0-5279", "4.0-5525"}}, {40, {"4.0-5531", "4.0-5809"}},
    {41, {"4.0-5811", "4.0-5923"}}, {42, {"4.0-5925", "4.0-5946"}}};

enum
{
  STATE_NONE = 0,
  STATE_SAVE = 1,
  STATE_LOAD = 2,
};

static bool s_use_compression = true;

Interpreter* GetInterpreter(Core::System& system)
{
  // 1. Get the JitInterface& manager from the system
  JitInterface& jit_manager = system.GetJitInterface();
  
  // 2. Get the generic CPUCoreBase* pointer from the manager
  CPUCoreBase* cpu_core = jit_manager.GetCore();

  // 3. Now, we can safely dynamic_cast the CPUCoreBase* to an Interpreter*
  return dynamic_cast<Interpreter*>(cpu_core);
}

void EnableCompression(bool compression)
{
  s_use_compression = compression;
}

static void DoState(Core::System& system, PointerWrap& p)
{
  bool is_wii = system.IsWii() || system.IsMIOS();
  const bool is_wii_currently = is_wii;
  p.Do(is_wii);
  if (is_wii != is_wii_currently)
  {
    OSD::AddMessage(fmt::format("Cannot load a savestate created under {} mode in {} mode",
                                is_wii ? "Wii" : "GC", is_wii_currently ? "Wii" : "GC"),
                    OSD::Duration::NORMAL, OSD::Color::RED);
    p.SetMeasureMode();
    return;
  }

  // Check to make sure the emulated memory sizes are the same as the savestate
  auto& memory = system.GetMemory();
  u32 state_mem1_size = memory.GetRamSizeReal();
  u32 state_mem2_size = memory.GetExRamSizeReal();
  p.Do(state_mem1_size);
  p.Do(state_mem2_size);
  if (state_mem1_size != memory.GetRamSizeReal() || state_mem2_size != memory.GetExRamSizeReal())
  {
    OSD::AddMessage(fmt::format("Memory size mismatch!\n"
                                "Current | MEM1 {:08X} ({:3}MB)    MEM2 {:08X} ({:3}MB)\n"
                                "State   | MEM1 {:08X} ({:3}MB)    MEM2 {:08X} ({:3}MB)",
                                memory.GetRamSizeReal(), memory.GetRamSizeReal() / 0x100000U,
                                memory.GetExRamSizeReal(), memory.GetExRamSizeReal() / 0x100000U,
                                state_mem1_size, state_mem1_size / 0x100000U, state_mem2_size,
                                state_mem2_size / 0x100000U));
    p.SetMeasureMode();
    return;
  }

  // Movie must be done before the video backend, because the window is redrawn in the video backend
  // state load, and the frame number must be up-to-date.
  system.GetMovie().DoState(p);
  p.DoMarker("Movie");
  // CoreTiming needs to be restored before restoring Hardware because
  // the controller code might need to schedule an event if the controller has changed.
  system.GetCoreTiming().DoState(p);
  p.DoMarker("CoreTiming");

  // HW needs to be restored before PowerPC because the data cache might need to be flushed.
  HW::DoState(system, p);
  p.DoMarker("HW");

  g_video_backend->DoState(p);
  p.DoMarker("video_backend");

  system.GetPowerPC().DoState(p);
  p.DoMarker("PowerPC");

  if (system.IsWii())
    Wiimote::DoState(p);
  p.DoMarker("Wiimote");
  Gecko::DoState(p);
  p.DoMarker("Gecko");

  #ifdef USE_RETRO_ACHIEVEMENTS
    AchievementManager::GetInstance().DoState(p);
  #endif  // USE_RETRO_ACHIEVEMENTS
}

void LogOffset(const char* name, PointerWrap& p)
{
  if (p.IsMeasureMode()) {
      // In Measure mode, ptr *is* the offset
      printf("WII-MIX-MAP: Offset after %s: 0x%zX\n", name, reinterpret_cast<size_t>(p.GetCurrentPtr()));
  }
}

void LogMap(PointerWrap& p, const char* name) {
    if (p.IsMeasureMode()) {
        size_t offset = reinterpret_cast<size_t>(p.GetCurrentPtr());
        // Format: [MAP],OffsetDec,OffsetHex,VariableName
        printf("[MAP],%zu,%zX,%s\n", offset, offset, name);
    }
}

// Macro to verify/save a variable and log its position
#define DO_MAP(p, var) \
    do { \
        LogMap(p, #var); \
        p.Do(var); \
    } while(0)

// Macro for array elements (Fixing the snprintf cast error you saw earlier)
#define DO_MAP_ARRAY(p, arr, index) \
    do { \
        char name_buf[64]; \
        snprintf(name_buf, sizeof(name_buf), #arr "[%d]", static_cast<int>(index)); \
        LogMap(p, name_buf); \
        p.Do(arr[index]); \
    } while(0)

// DoWiiMixState is a custom savestate function that removes host data from savestates
// Making savestates more portable between different WiiMix configurations
static void DoWiiMixState(Core::System& system, PointerWrap& p)
{
  bool is_wii = system.IsWii() || system.IsMIOS();
  const bool is_wii_currently = is_wii;
  p.Do(is_wii);
  if (is_wii != is_wii_currently)
  {
    OSD::AddMessage(fmt::format("Cannot load a savestate created under {} mode in {} mode",
                                is_wii ? "Wii" : "GC", is_wii_currently ? "Wii" : "GC"),
                    OSD::Duration::NORMAL, OSD::Color::RED);
    p.SetMeasureMode();
    return;
  }

  // Check to make sure the emulated memory sizes are the same as the savestate
  auto& memory = system.GetMemory();
  u32 state_mem1_size = memory.GetRamSizeReal();
  u32 state_mem2_size = memory.GetExRamSizeReal();
  p.Do(state_mem1_size);
  p.Do(state_mem2_size);
  if (state_mem1_size != memory.GetRamSizeReal() || state_mem2_size != memory.GetExRamSizeReal())
  {
    OSD::AddMessage(fmt::format("Memory size mismatch!\n"
                                "Current | MEM1 {:08X} ({:3}MB)    MEM2 {:08X} ({:3}MB)\n"
                                "State   | MEM1 {:08X} ({:3}MB)    MEM2 {:08X} ({:3}MB)",
                                memory.GetRamSizeReal(), memory.GetRamSizeReal() / 0x100000U,
                                memory.GetExRamSizeReal(), memory.GetExRamSizeReal() / 0x100000U,
                                state_mem1_size, state_mem1_size / 0x100000U, state_mem2_size,
                                state_mem2_size / 0x100000U));
    p.SetMeasureMode();
    return;
  }

  // Movie must be done before the video backend, because the window is redrawn in the video backend
  // state load, and the frame number must be up-to-date.
  
  // 100% pure emulated data; can be saved, may be useful in other places other than just Movie
  system.GetMovie().DoState(p);
  p.DoMarker("Movie");
  LogOffset("Movie", p);

  // Begin with video backend, so that it gets a chance to clear its caches and writeback modified
  // things to RAM
  
  // video backend is a MIX of emulated and host-specific data, so we need to modify it to only save emulated data
  // NOTE: video backend is difficult to separate, so for now I'm commenting it
  // g_video_backend->DoState(p);
  // p.DoMarker("video_backend");
  // LogOffset("video_backend", p);

  // 1. Backend Processor Registers
  // Contains ZMode, BlendMode, TevStages, etc.
  p.Do(bpmem); 
  p.DoMarker("bpMem");
  LogOffset("bpMem", p);

  // 2. Transform Unit Memory
  // Contains Light Objects, Matrices, Viewport, etc.
  p.Do(xfmem);
  p.DoMarker("xfMem");
  LogOffset("xfMem", p);

  // 3. Command Processor State
  // Contains Vertex Attributes (VAT), current pipeline state
  if (p.IsMeasureMode()) printf("--- CP STATE BREAKDOWN ---\n");

  DO_MAP(p, g_main_cp_state.matrix_index_a.Hex);
  DO_MAP(p, g_main_cp_state.matrix_index_b.Hex);
  DO_MAP(p, g_main_cp_state.vtx_desc.low.Hex);
  DO_MAP(p, g_main_cp_state.vtx_desc.high.Hex);

  for (int i = 0; i < 8; ++i) {
      // Log the vtx_attr[i].g0.Hex field with its index
      {
        char name_buf[64];
        snprintf(name_buf, sizeof(name_buf), "vtx_attr[%d].g0.Hex", i);
        LogMap(p, name_buf);
        p.Do(g_main_cp_state.vtx_attr[i].g0.Hex);
      }
      // Actually, cleaner to just do manual logging for nested structs:
      // LogMap(p, fmt::format("vtx_attr[{}].g0", i).c_str()); p.Do(...);
      // But for now, let's focus on the arrays causing the crash.
  }

  // The Arrays (The culprit)
  for (int i = 0; i < 16; ++i) {
      CPArray idx = static_cast<CPArray>(i);
      DO_MAP_ARRAY(p, g_main_cp_state.array_bases, idx);
  }

  for (int i = 0; i < 16; ++i) {
      CPArray idx = static_cast<CPArray>(i);
      DO_MAP_ARRAY(p, g_main_cp_state.array_strides, idx);
  }

  p.Do(g_preprocess_cp_state);
  p.DoMarker("cpMem");
  LogOffset("cpMem", p);

  // CoreTiming needs to be restored before restoring Hardware because
  // the controller code might need to schedule an event if the controller has changed.
  // CoreTiming has 2 distinct parts:
  // - Emulated data (timing, scheduled events)
  // - Host data (m_event_types (maps events to function pointers; host-specific))
  system.GetCoreTiming().LogTimingState("Before DoWiiMixState CoreTiming");
  system.GetCoreTiming().DoState(p);
  p.DoMarker("CoreTiming");
  LogOffset("CoreTiming", p);
  system.GetCoreTiming().LogTimingState("After DoWiiMixState CoreTiming");

  // HW needs to be restored before PowerPC because the data cache might need to be flushed.
  // TODOx: HW::DoState may include host-specific data, so we may need to modify it for WiiMix
  
  // NOTE: HW currently only saves mem
  HW::DoState(system, p);
  p.DoMarker("HW");
  LogOffset("HW", p);

  // TODOx: PowerPC::DoState serializes JIT, so we should include a flag to skip that for WiiMix savestates
  // TODOx: bring this back once I can figure it out
  system.GetPowerPC().DoState(p);
  p.DoMarker("PowerPC");
  LogOffset("PowerPC", p);

  // TODOx: Likely has bluetooth host-specific data
  if (system.IsWii()) {
    Wiimote::DoState(p);
    p.DoMarker("Wiimote");
    LogOffset("Wiimote", p);
  }
  // Gecko codes are currently not supported for WiiMix
  if (!WIIMIX_STATE) {
    Gecko::DoState(p);
    p.DoMarker("Gecko");
    LogOffset("Gecko", p);
  }

  // Retroachievements DoState is actually required
  // It only saves emulated data (achievement progress)
  // if (!WIIMIX_STATE) {
  #ifdef USE_RETRO_ACHIEVEMENTS
    AchievementManager::GetInstance().DoState(p);
    p.DoMarker("AchievementManager");
    LogOffset("AchievementManager", p);
  #endif  // USE_RETRO_ACHIEVEMENTS
  // }
}

void LoadFromBuffer(Core::System& system, std::vector<u8>& buffer)
{
  if (NetPlay::IsNetPlayRunning())
  {
    OSD::AddMessage("Loading savestates is disabled in Netplay to prevent desyncs");
    return;
  }

  if (AchievementManager::GetInstance().IsHardcoreModeActive())
  {
    OSD::AddMessage("Loading savestates is disabled in RetroAchievements hardcore mode");
    return;
  }

  Core::RunOnCPUThread(
      system,
      [&] {
        u8* ptr = buffer.data();
        PointerWrap p(&ptr, buffer.size(), PointerWrap::Mode::Read);
        DoState(system, p);
      },
      true);
}

bool WiiMixLoadFromBuffer(Core::System& system, std::vector<u8>& buffer)
{
  if (NetPlay::IsNetPlayRunning())
  {
    OSD::AddMessage("Loading savestates is disabled in Netplay to prevent desyncs");
    return false;
  }

  if (AchievementManager::GetInstance().IsHardcoreModeActive())
  {
    OSD::AddMessage("Loading savestates is disabled in RetroAchievements hardcore mode");
    return false;
  }

  // This should only be called from the CPU thread
  bool success = false;
  u8* ptr = buffer.data();
  PointerWrap p(&ptr, buffer.size(), PointerWrap::Mode::Read);
  DoWiiMixState(system, p);

  if (p.IsReadMode())
  {
    success = WiiMixHostReinitialization(system);
  }
  return success;
}

bool WiiMixLoadFromBufferEmuThread(Core::System& system, std::vector<u8>& buffer)
{
    if (NetPlay::IsNetPlayRunning())
    {
        OSD::AddMessage("Loading savestates is disabled in Netplay to prevent desyncs");
        return false;
    }

    if (AchievementManager::GetInstance().IsHardcoreModeActive())
    {
        OSD::AddMessage("Loading savestates is disabled in RetroAchievements hardcore mode");
        return false;
    }

    // This should only be called from the CPU thread

    bool success = false;
    u8* ptr = buffer.data();
    PointerWrap p(&ptr, buffer.size(), PointerWrap::Mode::Read);

    auto& power_pc = system.GetPowerPC();

    system.GetCoreTiming().LogEventQueues("Start of Load Function");

    // -------------------------------------------------------------------------
    // STEP 1: HOST RE-INITIALIZATION (RESET)
    // We must reset the emulated hardware and host state BEFORE loading data.
    // If we do this after, we wipe out the data we just loaded (causing the cpMem mismatches).
    // -------------------------------------------------------------------------
    if (p.IsReadMode()) {
        // 1. Poison memory/state to ensure we aren't relying on stale data
        system.PoisonState(static_cast<Core::PoisonModule>(State::s_poison_mask));
        
        // 2. Reset Host Environment (Hardware, Timers, Threads)
        // This registers all Event Types but leaves queues empty/default.
        success = WiiMixHostReinitialization(system);
        if (!success)
            return false;

        system.GetCoreTiming().LogEventQueues("After Host Reinit");

        // 3. Clear the Event Queue
        // WiiMixReset might have added default startup events. We don't want those;
        // we want exactly what is in the save file.
        system.GetCoreTiming().MoveEvents();
        system.GetCoreTiming().LogEventQueues("After MoveEvents");
        system.GetCoreTiming().ClearPendingEvents();
        system.GetCoreTiming().LogEventQueues("After ClearPendingEvents");

        // 4. Clear Critical Hardware State Manually
        // Even though HostReinit does this, we double-tap CP state to be safe.
        std::memset(&g_main_cp_state, 0, sizeof(g_main_cp_state));
        std::memset(&g_preprocess_cp_state, 0, sizeof(g_preprocess_cp_state));
    }

    printf("Current frame in EmuThread before DoWiiMixState: %lu\n", system.GetMovie().GetCurrentFrame());

    // -------------------------------------------------------------------------
    // STEP 2: LOAD STATE
    // Now that the machine is "fresh" and all Event Types are registered,
    // we assume the authoritative state from the buffer.
    // -------------------------------------------------------------------------
    DoWiiMixState(system, p);

    // --- DIAGNOSTIC TRAP: IMMEDIATE READBACK ---
    if (g_main_cp_state.matrix_index_a.Hex == 0xFFFFFFFF) {
        printf("CRITICAL FAILURE: CP State corrupted IMMEDIATELY after DoWiiMixState (Load)!\n");
    } else {
        printf("CP State clean after DoWiiMixState: 0x%08X\n", g_main_cp_state.matrix_index_a.Hex);
    }
    // ---------------------------------------------

    system.GetCoreTiming().LogEventQueues("After DoWiiMixState (Pure State Loaded)");

    // -------------------------------------------------------------------------
    // STEP 3: POST-LOAD THREAD RESTART
    // Restart any threads that were stopped during Reset.
    // -------------------------------------------------------------------------
    if (p.IsReadMode()) {
        // DVD Interface (Restarts the DVD Thread)
        system.GetDVDInterface().WiiMixRestart();

        // DSP (Restarts the DSP Thread)
        system.GetDSP().WiiMixRestart(Config::Get<bool>(Config::MAIN_DSP_HLE));
    }

    // Refresh cpu base pointer to point to valid JIT
    power_pc.ApplyMode();

    auto& ppc_state = power_pc.GetPPCState();
    auto& memory = system.GetMemory();

    // CRITICAL: Force the JIT/Code Cache to clear.
    {
        Core::CPUThreadGuard guard(system);
        system.GetJitInterface().ClearCache(guard);
    }

    if (!ppc_state.m_enable_dcache)
    {
        INFO_LOG_FMT(POWERPC, "Flushing data cache");
        ppc_state.dCache.FlushAll(memory);
    }

    PowerPC::RoundingModeUpdated(ppc_state);
    PowerPC::RecalculateAllFeatureFlags(ppc_state);

    auto& mmu = system.GetMMU();
    mmu.IBATUpdated();
    mmu.DBATUpdated();

    // Prevent CoreTiming from running a full advance right after loading the state
    // auto& core_timing = system.GetCoreTiming();
    // if (ppc_state.downcount > 0 && ppc_state.downcount < 20000) {
    // // We are mid-slice. We MUST skip the next Advance() so the CPU 
    // // can finish the remaining downcount cycles before the timer updates.
    //   core_timing.SetSkipNextAdvance(true);
    // } else {
    //   core_timing.SetSkipNextAdvance(false);
    // }

    if (p.IsReadMode()) {
            // A. Invalidate Vertex Loader
            VertexLoaderManager::Init();

            // FIX: Ensure XF Memory is invalidated BEFORE Shader/Vertex Managers
            auto& xf = system.GetXFStateManager();
            xf.InvalidateXFRange(0, 0x1000); // 4KB XF memory forces backend to reload all XF data
            xf.SetProjectionChanged();
            xf.SetViewportChanged();
            xf.SetLightingConfigChanged();

            if (g_vertex_manager) {
                g_vertex_manager->Invalidate();
            }

            // B. Dirty the Shader Managers
            system.GetVertexShaderManager().dirty = true;
            system.GetPixelShaderManager().Dirty();
            system.GetGeometryShaderManager().Dirty();
    }

    printf("Current frame at end of EmuThread: %lu\n", system.GetMovie().GetCurrentFrame());
    system.GetCoreTiming().LogEventQueues("After Cache Refresh");
    
    return p.IsReadMode();
}

void SaveToBuffer(Core::System& system, std::vector<u8>& buffer)
{
  Core::RunOnCPUThread(
      system,
      [&] {
        u8* ptr = nullptr;
        PointerWrap p_measure(&ptr, 0, PointerWrap::Mode::Measure);

        DoState(system, p_measure);
        const size_t buffer_size = reinterpret_cast<size_t>(ptr);
        buffer.resize(buffer_size);

        ptr = buffer.data();
        PointerWrap p(&ptr, buffer_size, PointerWrap::Mode::Write);
        DoState(system, p);
      },
      true);
}

void WiiMixSaveToBuffer(Core::System& system, std::vector<u8>& buffer)
{
    // Should be called from CPU thread already
    u8* ptr = nullptr;
    PointerWrap p_measure(&ptr, 0, PointerWrap::Mode::Measure);

    // Measure
    DoWiiMixState(system, p_measure);
    const size_t buffer_size = reinterpret_cast<size_t>(ptr);
    buffer.resize(buffer_size);

    ptr = buffer.data();
    PointerWrap p(&ptr, buffer_size, PointerWrap::Mode::Write);
    
    // Write
    DoWiiMixState(system, p);
}

namespace
{
struct SlotWithTimestamp
{
  int slot;
  double timestamp;
};
}  // namespace

// returns first slot number not in the vector, or -1 if all are in the vector
static int GetEmptySlot(const std::vector<SlotWithTimestamp>& used_slots)
{
  for (int i = 1; i <= (int)NUM_STATES; i++)
  {
    const auto it = std::find_if(used_slots.begin(), used_slots.end(),
                                 [i](const SlotWithTimestamp& slot) { return slot.slot == i; });
    if (it == used_slots.end())
      return i;
  }
  return -1;
}

// Arbitrarily chosen value (38 years) that is subtracted in GetSystemTimeAsDouble()
// to increase sub-second precision of the resulting double timestamp
static constexpr int DOUBLE_TIME_OFFSET = (38 * 365 * 24 * 60 * 60);

static double GetSystemTimeAsDouble()
{
  const auto since_epoch = std::chrono::system_clock::now().time_since_epoch();

  const auto since_double_time_epoch = since_epoch - std::chrono::seconds(DOUBLE_TIME_OFFSET);
  return std::chrono::duration_cast<std::chrono::duration<double>>(since_double_time_epoch).count();
}

static std::string SystemTimeAsDoubleToString(double time)
{
  // revert adjustments from GetSystemTimeAsDouble() to get a normal Unix timestamp again
  const time_t seconds = static_cast<time_t>(time) + DOUBLE_TIME_OFFSET;
  const auto local_time = Common::Localtime(seconds);
  if (!local_time)
    return "";

  // fmt is locale agnostic by default, so explicitly use current locale.
  return fmt::format(std::locale{""}, "{:%x %X}", *local_time);
}

static std::string MakeStateFilename(int number);

static std::vector<SlotWithTimestamp> GetUsedSlotsWithTimestamp()
{
  std::vector<SlotWithTimestamp> result;
  StateHeader header;
  for (int i = 1; i <= (int)NUM_STATES; i++)
  {
    std::string filename = MakeStateFilename(i);
    if (File::Exists(filename))
    {
      if (ReadHeader(filename, header))
      {
        result.emplace_back(SlotWithTimestamp{.slot = i, .timestamp = header.legacy_header.time});
      }
    }
  }
  return result;
}

static bool CompareTimestamp(const SlotWithTimestamp& lhs, const SlotWithTimestamp& rhs)
{
  return lhs.timestamp < rhs.timestamp;
}

static void CompressBufferToFile(const u8* raw_buffer, u64 size, File::IOFile& f)
{
  u64 total_bytes_compressed = 0;

  while (true)
  {
    u64 bytes_left_to_compress = size - total_bytes_compressed;

    int bytes_to_compress =
        static_cast<int>(std::min(static_cast<u64>(LZ4_MAX_INPUT_SIZE), bytes_left_to_compress));
    int compressed_buffer_size = LZ4_compressBound(bytes_to_compress);
    auto compressed_buffer = std::make_unique<char[]>(compressed_buffer_size);
    s32 compressed_len =
        LZ4_compress_default(reinterpret_cast<const char*>(raw_buffer) + total_bytes_compressed,
                             compressed_buffer.get(), bytes_to_compress, compressed_buffer_size);

    if (compressed_len == 0)
    {
      PanicAlertFmtT("Internal LZ4 Error - compression failed");
      break;
    }

    // The size of the data to write is 'compressed_len'
    f.WriteArray(&compressed_len, 1);
    f.WriteBytes(compressed_buffer.get(), compressed_len);

    total_bytes_compressed += bytes_to_compress;
    if (total_bytes_compressed == size)
      break;
  }
}

static void CreateExtendedHeader(StateExtendedHeader& extended_header, size_t uncompressed_size)
{
  StateExtendedBaseHeader& base_header = extended_header.base_header;
  base_header.header_version = EXTENDED_HEADER_VERSION;
  base_header.compression_type =
      s_use_compression ? CompressionType::LZ4 : CompressionType::Uncompressed;
  base_header.payload_offset = COMPRESSED_DATA_OFFSET;
  base_header.uncompressed_size = uncompressed_size;

  // If more fields are added to StateExtendedHeader, set them here.
}

static void WriteHeadersToFile(size_t uncompressed_size, File::IOFile& f)
{
  StateHeader header{};
  SConfig::GetInstance().GetGameID().copy(header.legacy_header.game_id,
                                          std::size(header.legacy_header.game_id));
  header.legacy_header.time = GetSystemTimeAsDouble();

  header.version_header.version_cookie = WIIMIX_COOKIE_BASE + WIIMIX_STATE_VERSION;
  header.version_string = Common::GetScmRevStr();
  header.version_header.version_string_length = static_cast<u32>(header.version_string.length());

  StateExtendedHeader extended_header{};
  CreateExtendedHeader(extended_header, uncompressed_size);

  f.WriteArray(&header.legacy_header, 1);
  f.WriteArray(&header.version_header, 1);
  f.WriteString(header.version_string);

  f.WriteArray(&extended_header.base_header, 1);
  // If StateExtendedHeader is amended to include more than the base, add WriteBytes() calls here.
}

static void WriteWiiMixHeadersToFile(size_t uncompressed_size, File::IOFile& f)
{
  StateHeader header{};
  SConfig::GetInstance().GetGameID().copy(header.legacy_header.game_id,
                                          std::size(header.legacy_header.game_id));
  header.legacy_header.time = GetSystemTimeAsDouble();

  header.version_header.version_cookie = WIIMIX_COOKIE_BASE + WIIMIX_STATE_VERSION;
  header.version_string = Common::GetScmRevStr();
  header.version_header.version_string_length = static_cast<u32>(header.version_string.length());

  StateExtendedHeader extended_header{};
  CreateExtendedHeader(extended_header, uncompressed_size);

  f.WriteArray(&header.legacy_header, 1);
  f.WriteArray(&header.version_header, 1);
  f.WriteString(header.version_string);

  f.WriteArray(&extended_header.base_header, 1);
  // If StateExtendedHeader is amended to include more than the base, add WriteBytes() calls here.
}

static void CompressAndDumpState(Core::System& system, CompressAndDumpState_args& save_args)
{
  const u8* const buffer_data = save_args.buffer_vector.data();
  const size_t buffer_size = save_args.buffer_vector.size();
  const std::string& filename = save_args.filename;

  // Find free temporary filename.
  // TODO: The file exists check and the actual opening of the file should be atomic, we don't have
  // functions for that.
  std::string temp_filename;
  size_t temp_counter = static_cast<size_t>(Common::CurrentThreadId());
  do
  {
    temp_filename = fmt::format("{}{}.tmp", filename, temp_counter);
    ++temp_counter;
  } while (File::Exists(temp_filename));

  File::IOFile f(temp_filename, "wb");
  if (!f)
  {
    Core::DisplayMessage("Failed to create state file", 2000);
    return;
  }

  WriteHeadersToFile(buffer_size, f);

  if (s_use_compression)
    CompressBufferToFile(buffer_data, buffer_size, f);
  else
    f.WriteBytes(buffer_data, buffer_size);

  if (!f.IsGood())
    Core::DisplayMessage("Failed to write state file", 2000);

  const std::string last_state_filename = File::GetUserPath(D_STATESAVES_IDX) + "lastState.sav";
  const std::string last_state_dtmname = last_state_filename + ".dtm";
  const std::string dtmname = filename + ".dtm";

  {
    std::lock_guard lk(s_save_thread_mutex);

    // Backup existing state (overwriting an existing backup, if any).
    if (File::Exists(filename))
    {
      if (File::Exists(last_state_filename))
        File::Delete((last_state_filename));
      if (File::Exists(last_state_dtmname))
        File::Delete((last_state_dtmname));

      if (!File::Rename(filename, last_state_filename))
      {
        Core::DisplayMessage("Failed to move previous state to state undo backup", 1000);
      }
      else if (File::Exists(dtmname))
      {
        if (!File::Rename(dtmname, last_state_dtmname))
          Core::DisplayMessage("Failed to move previous state's dtm to state undo backup", 1000);
      }
    }

    auto& movie = system.GetMovie();
    if ((movie.IsMovieActive()) && !movie.IsJustStartingRecordingInputFromSaveState())
      movie.SaveRecording(dtmname);
    else if (!movie.IsMovieActive())
      File::Delete(dtmname);

    // Move written state to final location.
    // TODO: This should also be atomic. This is possible on all systems, but needs a special
    // implementation of IOFile on Windows.
    if (!f.Close())
      Core::DisplayMessage("Failed to close state file", 2000);

    if (!File::Rename(temp_filename, filename))
    {
      Core::DisplayMessage("Failed to rename state file", 2000);
    }
    else
    {
      const std::filesystem::path temp_path(filename);
      Core::DisplayMessage(fmt::format("Saved State to {}", temp_path.filename().string()), 2000);
    }
  }

  Host_UpdateMainFrame();
  Core::safe_to_quit = true;
}

static void CompressAndDumpWiiMixState(Core::System& system, CompressAndDumpState_args& save_args)
{
  const u8* const buffer_data = save_args.buffer_vector.data();
  const size_t buffer_size = save_args.buffer_vector.size();
  const std::string& filename = save_args.filename;

  // Find free temporary filename.
  // TODO: The file exists check and the actual opening of the file should be atomic, we don't have
  // functions for that.
  std::string temp_filename;
  size_t temp_counter = static_cast<size_t>(Common::CurrentThreadId());
  do
  {
    temp_filename = fmt::format("{}{}.tmp", filename, temp_counter);
    ++temp_counter;
  } while (File::Exists(temp_filename));

  File::IOFile f(temp_filename, "wb");
  if (!f)
  {
    Core::DisplayMessage("Failed to create state file", 2000);
    return;
  }

  WriteWiiMixHeadersToFile(buffer_size, f);

  if (s_use_compression)
    CompressBufferToFile(buffer_data, buffer_size, f);
  else
    f.WriteBytes(buffer_data, buffer_size);

  if (!f.IsGood())
    Core::DisplayMessage("Failed to write state file", 2000);

  const std::string last_state_filename = File::GetUserPath(D_STATESAVES_IDX) + "lastState." + WIIMIX_STATE_EXTENSION;
  const std::string last_state_dtmname = last_state_filename + ".dtm";
  const std::string dtmname = filename + ".dtm";

  {
    std::lock_guard lk(s_save_thread_mutex);

    // Backup existing state (overwriting an existing backup, if any).
    if (File::Exists(filename))
    {
      if (File::Exists(last_state_filename))
        File::Delete((last_state_filename));
      if (File::Exists(last_state_dtmname))
        File::Delete((last_state_dtmname));

      if (!File::Rename(filename, last_state_filename))
      {
        Core::DisplayMessage("Failed to move previous state to state undo backup", 1000);
      }
      else if (File::Exists(dtmname))
      {
        if (!File::Rename(dtmname, last_state_dtmname))
          Core::DisplayMessage("Failed to move previous state's dtm to state undo backup", 1000);
      }
    }

    auto& movie = system.GetMovie();
    if ((movie.IsMovieActive()) && !movie.IsJustStartingRecordingInputFromSaveState())
      movie.SaveRecording(dtmname);
    else if (!movie.IsMovieActive())
      File::Delete(dtmname);

    // Move written state to final location.
    // TODO: This should also be atomic. This is possible on all systems, but needs a special
    // implementation of IOFile on Windows.
    if (!f.Close())
      Core::DisplayMessage("Failed to close state file", 2000);

    if (!File::Rename(temp_filename, filename))
    {
      Core::DisplayMessage("Failed to rename state file", 2000);
    }
    else
    {
      const std::filesystem::path temp_path(filename);
      Core::DisplayMessage(fmt::format("Saved State to {}", temp_path.filename().string()), 2000);
    }
  }

  Host_UpdateMainFrame();
  Core::safe_to_quit = true;
}

void SaveAs(Core::System& system, const std::string& filename, bool wait)
{
  std::unique_lock lk(s_load_or_save_in_progress_mutex, std::try_to_lock);
  if (!lk)
    return;
  Core::safe_to_quit = false;
  Core::RunOnCPUThread(
      system,
      [&] {
        Core::safe_to_quit = false;
        {
          std::lock_guard lk_(s_state_writes_in_queue_mutex);
          ++s_state_writes_in_queue;
        }

        // Measure the size of the buffer.
        u8* ptr = nullptr;
        PointerWrap p_measure(&ptr, 0, PointerWrap::Mode::Measure);
        DoState(system, p_measure);
        const size_t buffer_size = reinterpret_cast<size_t>(ptr);

        // Then actually do the write.
        std::vector<u8> current_buffer;
        current_buffer.resize(buffer_size);
        ptr = current_buffer.data();
        PointerWrap p(&ptr, buffer_size, PointerWrap::Mode::Write);
        DoState(system, p);

        if (p.IsWriteMode())
        {
          Core::DisplayMessage("Saving State...", 1000);

          std::shared_ptr<Common::Event> sync_event;

          CompressAndDumpState_args save_args;
          save_args.buffer_vector = std::move(current_buffer);
          save_args.filename = filename;
          if (wait)
          {
            sync_event = std::make_shared<Common::Event>();
            save_args.state_write_done_event = sync_event;
          }

          s_save_thread.EmplaceItem(std::move(save_args));

          if (sync_event)
            sync_event->Wait();
        }
        else
        {
          // someone aborted the save by changing the mode?
          {
            // Note: The worker thread takes care of this in the other branch.
            std::lock_guard lk_(s_state_writes_in_queue_mutex);
            if (--s_state_writes_in_queue == 0)
              s_state_write_queue_is_empty.notify_all();
          }
          Core::DisplayMessage("Unable to save: Internal DoState Error", 4000);
        }
        Core::safe_to_quit = true;
      },
      true);
  // safe_to_quit = true;
}

void SaveAsWiiMix(Core::System& system, const std::string& filename, bool wait)
{
  std::unique_lock lk(s_load_or_save_in_progress_mutex, std::try_to_lock);
  if (!lk)
    return;
  Core::safe_to_quit = false;
  Core::RunOnCPUThread(
      system,
      [&] {
        Core::safe_to_quit = false;
        {
          std::lock_guard lk_(s_state_writes_in_queue_mutex);
          ++s_state_writes_in_queue;
        }

        // Measure the size of the buffer.
        u8* ptr = nullptr;
        PointerWrap p_measure(&ptr, 0, PointerWrap::Mode::Measure);
        DoWiiMixState(system, p_measure);
        const size_t buffer_size = reinterpret_cast<size_t>(ptr);

        // Then actually do the write.
        std::vector<u8> current_buffer;
        current_buffer.resize(buffer_size);
        ptr = current_buffer.data();
        PointerWrap p(&ptr, buffer_size, PointerWrap::Mode::Write);
        DoWiiMixState(system, p);

        if (p.IsWriteMode())
        {
          Core::DisplayMessage("Saving State...", 1000);

          std::shared_ptr<Common::Event> sync_event;

          CompressAndDumpState_args save_args;
          save_args.buffer_vector = std::move(current_buffer);
          save_args.filename = filename;
          if (wait)
          {
            sync_event = std::make_shared<Common::Event>();
            save_args.state_write_done_event = sync_event;
          }

          s_wiimix_save_thread.EmplaceItem(std::move(save_args));

          if (sync_event)
            sync_event->Wait();
        }
        else
        {
          // someone aborted the save by changing the mode?
          {
            // Note: The worker thread takes care of this in the other branch.
            std::lock_guard lk_(s_state_writes_in_queue_mutex);
            if (--s_state_writes_in_queue == 0)
              s_state_write_queue_is_empty.notify_all();
          }
          Core::DisplayMessage("Unable to save: Internal DoState Error", 4000);
        }
        Core::safe_to_quit = true;
      },
      true);
  // safe_to_quit = true;
}

static bool GetVersionFromLZO(StateHeader& header, File::IOFile& f)
{
  // Just read the first block, since it will contain the full revision string
  lzo_uint32 cur_len = 0;  // size of compressed bytes
  lzo_uint new_len = 0;    // size of uncompressed bytes
  std::vector<u8> buffer;
  buffer.resize(header.legacy_header.lzo_size);

  if (!f.ReadArray(&cur_len, 1) || !f.ReadBytes(out, cur_len))
    return false;

  const int res = lzo1x_decompress(out, cur_len, buffer.data(), &new_len, nullptr);
  if (res != LZO_E_OK)
  {
    // This doesn't seem to happen anymore.
    PanicAlertFmtT("Internal LZO Error - decompression failed ({0}) ({1}) \n"
                   "Unable to retrieve outdated savestate version info.",
                   res, new_len);
    return false;
  }

  // Read in cookie and string length
  if (buffer.size() >= sizeof(StateHeaderVersion))
  {
    memcpy(&header.version_header, buffer.data(), sizeof(StateHeaderVersion));
  }
  else
  {
    PanicAlertFmtT("Internal LZO Error - failed to parse decompressed version cookie and version "
                   "string length ({0})",
                   buffer.size());
    return false;
  }

  // Read in the string
  if (buffer.size() >= sizeof(StateHeaderVersion) + header.version_header.version_string_length)
  {
    auto version_buffer = std::make_unique<char[]>(header.version_header.version_string_length);
    memcpy(version_buffer.get(), buffer.data() + sizeof(StateHeaderVersion),
           header.version_header.version_string_length);
    header.version_string =
        std::string(version_buffer.get(), header.version_header.version_string_length);
  }
  else
  {
    PanicAlertFmtT("Internal LZO Error - failed to parse decompressed version string ({0} / {1})",
                   header.version_header.version_string_length, buffer.size());
    return false;
  }

  return true;
}

static bool ReadStateHeaderFromFile(StateHeader& header, File::IOFile& f,
                                    bool get_version_header = true)
{
  if (!f.IsOpen())
  {
    Core::DisplayMessage("State not found", 2000);
    return false;
  }

  if (!f.ReadArray(&header.legacy_header, 1))
  {
    Core::DisplayMessage("Failed to read state legacy header", 2000);
    return false;
  }

  // Bail out if we only care for retrieving the legacy header.
  // This is the case with ReadHeader() calls.
  if (!get_version_header)
    return true;

  if (header.legacy_header.lzo_size != 0)
  {
    // Parse out version from legacy LZO compressed states
    if (!GetVersionFromLZO(header, f))
      return false;
  }
  else
  {
    if (!f.ReadArray(&header.version_header, 1))
    {
      Core::DisplayMessage("Failed to read state version header", 2000);
      return false;
    }

    auto version_buffer = std::make_unique<char[]>(header.version_header.version_string_length);
    if (!f.ReadBytes(version_buffer.get(), header.version_header.version_string_length))
    {
      Core::DisplayMessage("Failed to read state version string", 2000);
      return false;
    }

    header.version_string =
        std::string(version_buffer.get(), header.version_header.version_string_length);
  }

  return true;
}

static bool ReadWiiMixStateHeaderFromFile(StateHeader& header, File::IOFile& f,
                                    bool get_version_header = true)
{
  if (!f.IsOpen())
  {
    Core::DisplayMessage("State not found", 2000);
    return false;
  }

  if (!f.ReadArray(&header.legacy_header, 1))
  {
    Core::DisplayMessage("Failed to read state legacy header", 2000);
    return false;
  }

  // Bail out if we only care for retrieving the legacy header.
  // This is the case with ReadHeader() calls.
  if (!get_version_header)
    return true;

  if (header.legacy_header.lzo_size != 0)
  {
    // Parse out version from legacy LZO compressed states
    if (!GetVersionFromLZO(header, f))
      return false;
  }
  else
  {
    if (!f.ReadArray(&header.version_header, 1))
    {
      Core::DisplayMessage("Failed to read state version header", 2000);
      return false;
    }

    auto version_buffer = std::make_unique<char[]>(header.version_header.version_string_length);
    if (!f.ReadBytes(version_buffer.get(), header.version_header.version_string_length))
    {
      Core::DisplayMessage("Failed to read state version string", 2000);
      return false;
    }

    header.version_string =
        std::string(version_buffer.get(), header.version_header.version_string_length);
  }

  return true;
}

bool ReadHeader(const std::string& filename, StateHeader& header)
{
  // ensure that the savestate write thread isn't moving around states while we do this
  std::lock_guard lk(s_save_thread_mutex);

  File::IOFile f(filename, "rb");
  bool get_version_header = false;
  return ReadStateHeaderFromFile(header, f, get_version_header);
}

bool ReadWiiMixHeader(const std::string& filename, StateHeader& header)
{
  // ensure that the savestate write thread isn't moving around states while we do this
  std::lock_guard lk(s_save_thread_mutex);

  File::IOFile f(filename, "rb");
  bool get_version_header = false;
  return ReadWiiMixStateHeaderFromFile(header, f, get_version_header);
}

std::string GetInfoStringOfSlot(int slot, bool translate)
{
  std::string filename = MakeStateFilename(slot);
  if (!File::Exists(filename))
    return translate ? Common::GetStringT("Empty") : "Empty";

  State::StateHeader header;
  if (!ReadHeader(filename, header))
    return translate ? Common::GetStringT("Unknown") : "Unknown";

  return SystemTimeAsDoubleToString(header.legacy_header.time);
}

u64 GetUnixTimeOfSlot(int slot)
{
  State::StateHeader header;
  if (!ReadHeader(MakeStateFilename(slot), header))
    return 0;

  constexpr u64 MS_PER_SEC = 1000;
  return static_cast<u64>(header.legacy_header.time * MS_PER_SEC) +
         (DOUBLE_TIME_OFFSET * MS_PER_SEC);
}

static bool DecompressLZ4(std::vector<u8>& raw_buffer, u64 size, File::IOFile& f)
{
  raw_buffer.resize(size);

  u64 total_bytes_read = 0;
  while (true)
  {
    s32 compressed_data_len;
    if (!f.ReadArray(&compressed_data_len, 1))
    {
      PanicAlertFmt("Could not read state data length");
      return false;
    }

    if (compressed_data_len <= 0)
    {
      PanicAlertFmtT("Internal LZ4 Error - Tried decompressing {0} bytes", compressed_data_len);
      return false;
    }

    auto compressed_data = std::make_unique<char[]>(compressed_data_len);
    if (!f.ReadBytes(compressed_data.get(), compressed_data_len))
    {
      PanicAlertFmt("Could not read state data");
      return false;
    }

    u32 max_decompress_size =
        static_cast<u32>(std::min((u64)LZ4_MAX_INPUT_SIZE, size - total_bytes_read));

    int bytes_read = LZ4_decompress_safe(
        compressed_data.get(), reinterpret_cast<char*>(raw_buffer.data()) + total_bytes_read,
        compressed_data_len, max_decompress_size);

    if (bytes_read < 0)
    {
      PanicAlertFmtT("Internal LZ4 Error - decompression failed ({0}, {1}, {2})", bytes_read,
                     compressed_data_len, max_decompress_size);
      return false;
    }

    total_bytes_read += static_cast<u64>(bytes_read);

    if (total_bytes_read == size)
    {
      return true;
    }
    else if (total_bytes_read > size)
    {
      PanicAlertFmtT("Internal LZ4 Error - payload size mismatch ({0} / {1}))", total_bytes_read,
                     size);
      return false;
    }
  }
}

static bool ValidateHeaders(const StateHeader& header)
{
  bool success = true;

  // Game ID
  if (strncmp(SConfig::GetInstance().GetGameID().c_str(), header.legacy_header.game_id, 6))
  {
    Core::DisplayMessage(fmt::format("State belongs to a different game (ID {})",
                                     std::string_view{header.legacy_header.game_id,
                                                      std::size(header.legacy_header.game_id)}),
                         2000);
    return false;
  }

  // Check both the state version and the revision string
  std::string current_str = Common::GetScmRevStr();
  std::string loaded_str = header.version_string;
  const u32 loaded_version = header.version_header.version_cookie - COOKIE_BASE;

  if (s_old_versions.contains(loaded_version))
  {
    // This is a REALLY old version, before we started writing the version string to file
    success = false;

    std::pair<std::string, std::string> version_range = s_old_versions.find(loaded_version)->second;
    std::string oldest_version = version_range.first;
    std::string newest_version = version_range.second;

    loaded_str = "Dolphin " + oldest_version + " - " + newest_version;
  }
  else if (loaded_version != STATE_VERSION)
  {
    success = false;
  }

  if (!success)
  {
    const std::string message =
        loaded_str.empty() ?
            "This savestate was created using an incompatible version of Dolphin" :
            "This savestate was created using the incompatible version " + loaded_str;
    Core::DisplayMessage(message, OSD::Duration::NORMAL);
  }

  return success;
}

static bool ValidateWiiMixHeaders(const StateHeader& header)
{
  bool success = true;

  // Game ID
  if (strncmp(SConfig::GetInstance().GetGameID().c_str(), header.legacy_header.game_id, 6))
  {
    Core::DisplayMessage(fmt::format("State belongs to a different game (ID {})",
                                     std::string_view{header.legacy_header.game_id,
                                                      std::size(header.legacy_header.game_id)}),
                         2000);
    return false;
  }

  // Check both the state version and the revision string
  std::string current_str = Common::GetScmRevStr();
  std::string loaded_str = header.version_string;
  const u32 loaded_version = header.version_header.version_cookie - WIIMIX_COOKIE_BASE;

  if (s_old_versions.contains(loaded_version))
  {
    // This is a REALLY old version, before we started writing the version string to file
    success = false;

    std::pair<std::string, std::string> version_range = s_old_versions.find(loaded_version)->second;
    std::string oldest_version = version_range.first;
    std::string newest_version = version_range.second;

    loaded_str = "Dolphin " + oldest_version + " - " + newest_version;
  }
  else if (loaded_version != STATE_VERSION)
  {
    success = false;
  }

  if (!success)
  {
    const std::string message =
        loaded_str.empty() ?
            "This savestate was created using an incompatible version of Dolphin" :
            "This savestate was created using the incompatible version " + loaded_str;
    Core::DisplayMessage(message, OSD::Duration::NORMAL);
  }

  return success;
}

static void LoadFileStateData(const std::string& filename, std::vector<u8>& ret_data)
{
  File::IOFile f;

  {
    // If a state is currently saving, wait for that to end or time out.
    std::unique_lock lk(s_state_writes_in_queue_mutex);
    if (s_state_writes_in_queue != 0)
    {
      if (!s_state_write_queue_is_empty.wait_for(lk, std::chrono::seconds(3),
                                                 []() { return s_state_writes_in_queue == 0; }))
      {
        Core::DisplayMessage(
            "A previous state saving operation is still in progress, cancelling load.", 2000);
        return;
      }
    }
    f.Open(filename, "rb");
  }

  StateHeader header;
  if (!ReadStateHeaderFromFile(header, f) || !ValidateHeaders(header))
    return;

  StateExtendedHeader extended_header;
  if (!f.ReadArray(&extended_header.base_header, 1))
  {
    PanicAlertFmt("Unable to read state header");
    return;
  }
  // If StateExtendedHeader is amended to include more than the base, add ReadBytes() calls here.

  if (extended_header.base_header.header_version != EXTENDED_HEADER_VERSION)
  {
    PanicAlertFmt("State header corrupted");
    return;
  }

  std::vector<u8> buffer;

  switch (extended_header.base_header.compression_type)
  {
  case CompressionType::LZ4:
  {
    Core::DisplayMessage("Decompressing State...", 500);
    if (!DecompressLZ4(buffer, extended_header.base_header.uncompressed_size, f))
      return;

    break;
  }
  case CompressionType::Uncompressed:
  {
    u64 header_len = sizeof(StateHeaderLegacy) + sizeof(StateHeaderVersion) +
                     header.version_header.version_string_length + sizeof(StateExtendedBaseHeader) +
                     extended_header.base_header.payload_offset;

    u64 file_size = f.GetSize();
    if (file_size < header_len)
    {
      PanicAlertFmt("State header length corrupted");
      return;
    }

    const auto size = static_cast<size_t>(file_size - header_len);
    buffer.resize(size);

    if (!f.ReadBytes(buffer.data(), size))
    {
      PanicAlertFmt("Error reading bytes: {0}", size);
      return;
    }
    break;
  }
  default:
    PanicAlertFmt("Unknown compression type {0}", extended_header.base_header.compression_type);
    return;
  }

  // all good
  ret_data.swap(buffer);
}

static void LoadWiiMixFileStateData(const std::string& filename, std::vector<u8>& ret_data)
{
  File::IOFile f;

  {
    // If a state is currently saving, wait for that to end or time out.
    std::unique_lock lk(s_state_writes_in_queue_mutex);
    if (s_state_writes_in_queue != 0)
    {
      if (!s_state_write_queue_is_empty.wait_for(lk, std::chrono::seconds(3),
                                                 []() { return s_state_writes_in_queue == 0; }))
      {
        Core::DisplayMessage(
            "A previous state saving operation is still in progress, cancelling load.", 2000);
        return;
      }
    }
    f.Open(filename, "rb");
  }

  StateHeader header;
  if (!ReadWiiMixStateHeaderFromFile(header, f) || !ValidateWiiMixHeaders(header))
    return;

  StateExtendedHeader extended_header;
  if (!f.ReadArray(&extended_header.base_header, 1))
  {
    PanicAlertFmt("Unable to read state header");
    return;
  }
  // If StateExtendedHeader is amended to include more than the base, add ReadBytes() calls here.

  if (extended_header.base_header.header_version != EXTENDED_HEADER_VERSION)
  {
    PanicAlertFmt("State header corrupted");
    return;
  }

  std::vector<u8> buffer;

  switch (extended_header.base_header.compression_type)
  {
  case CompressionType::LZ4:
  {
    Core::DisplayMessage("Decompressing State...", 500);
    if (!DecompressLZ4(buffer, extended_header.base_header.uncompressed_size, f))
      return;

    break;
  }
  case CompressionType::Uncompressed:
  {
    u64 header_len = sizeof(StateHeaderLegacy) + sizeof(StateHeaderVersion) +
                     header.version_header.version_string_length + sizeof(StateExtendedBaseHeader) +
                     extended_header.base_header.payload_offset;

    u64 file_size = f.GetSize();
    if (file_size < header_len)
    {
      PanicAlertFmt("State header length corrupted");
      return;
    }

    const auto size = static_cast<size_t>(file_size - header_len);
    buffer.resize(size);

    if (!f.ReadBytes(buffer.data(), size))
    {
      PanicAlertFmt("Error reading bytes: {0}", size);
      return;
    }
    break;
  }
  default:
    PanicAlertFmt("Unknown compression type {0}", extended_header.base_header.compression_type);
    return;
  }

  // all good
  ret_data.swap(buffer);
}

void SetWiiMixJitterHostEnvironment(bool state)
{
  WIIMIX_JITTER = state;
}

// TODOx
// Reinitializes host components
// Minimum Viable Save State
// - Header
// - PowerPC::DoState
// - Memory::DoState
// - CoreTiming::DoState
bool WiiMixHostReinitialization(Core::System& system) {
  if (WIIMIX_JITTER)
    system.JitterHostEnvironment();

  // [FIX] MOVED DOWN: Do not reset CoreTiming here.
  // If we clear queues now, threads stopping below can still push new events 
  // (Ghost Events) into m_ts_queue before they fully exit.
  
  /* MOVED: system.GetCoreTiming().WiiMixReset(); */

  // 1. Stop host threads (ENSURE THEY ARE DEAD FIRST)
  system.GetDSP().Shutdown();
  system.GetDVDInterface().Shutdown();
  
  // FIX: DO NOT SHUTDOWN SOUND STREAM. 
  // AudioCommon::WiiMixShutdownSoundStream(system); 
  
  system.GetPowerPC().Shutdown(); 

  // [FIX] NEW LOCATION: Reset CoreTiming NOW.
  // Now that threads are stopped, no new events can be scheduled.
  // This clears any events generated during the shutdown of the threads above.
  system.GetCoreTiming().LogTimingState("Before Host Reinit");
  system.GetCoreTiming().WiiMixReset(); 
  system.GetCoreTiming().LogTimingState("After Host Reinit");

  // 2. Register All Event Handlers AND Re-init Hardware
  system.GetSystemTimers().WiiMixReset();
  system.GetProcessorInterface().WiiMixReset();
  system.GetMemoryInterface().WiiMixReset();
  system.GetSerialInterface().WiiMixReset();
  system.GetGPFifo().WiiMixReset();
  system.GetHSP().WiiMixReset();
  system.GetVideoInterface().Init();
  g_video_backend->WiiMixReset();
  
  // 3. Reset Host-Side Configuration
  system.GetExpansionInterface().WiiMixReset();
  system.GetMemory().WiiMixReset();

  // 4. FULLY Re-initialize threaded components
  system.GetDSP().WiiMixReset();
  
  // FIX: DO NOT RESET AUDIO BACKEND HERE
  system.GetAudioInterface().WiiMixReset();
  
  system.GetDVDInterface().WiiMixReset();

  // 5. Reset Host-Side Caches (CPU JIT)
  auto& power_pc = system.GetPowerPC();
  power_pc.Init(State::WIIMIX_DIFF_TEST_CPU_CORE);

  if (system.IsWii()) {
    Wiimote::ResetAllWiimotes();
  }

  return true;
}

void LoadAs(Core::System& system, const std::string& filename)
{
  if (!Core::IsRunningOrStarting(system))
    return;

  if (NetPlay::IsNetPlayRunning())
  {
    OSD::AddMessage("Loading savestates is disabled in Netplay to prevent desyncs");
    return;
  }

  if (AchievementManager::GetInstance().IsHardcoreModeActive())
  {
    OSD::AddMessage("Loading savestates is disabled in RetroAchievements hardcore mode");
    return;
  }

  std::unique_lock lk(s_load_or_save_in_progress_mutex, std::try_to_lock);
  if (!lk)
    return;

  Core::RunOnCPUThread(
      system,
      [&] {
        // Save temp buffer for undo load state
        auto& movie = system.GetMovie();
        if (!movie.IsJustStartingRecordingInputFromSaveState())
        {
          std::lock_guard lk2(s_undo_load_buffer_mutex);
          SaveToBuffer(system, s_undo_load_buffer);
          const std::string dtmpath = File::GetUserPath(D_STATESAVES_IDX) + "undo.dtm";
          if (movie.IsMovieActive())
            movie.SaveRecording(dtmpath);
          else if (File::Exists(dtmpath))
            File::Delete(dtmpath);
        }

        bool loaded = false;
        bool loadedSuccessfully = false;

        // brackets here are so buffer gets freed ASAP
        {
          std::vector<u8> buffer;
          LoadFileStateData(filename, buffer);

          if (!buffer.empty())
          {
            u8* ptr = buffer.data();
            PointerWrap p(&ptr, buffer.size(), PointerWrap::Mode::Read);
            DoState(system, p);
            loaded = true;
            loadedSuccessfully = p.IsReadMode();
          }
        }

        if (loaded)
        {
          if (loadedSuccessfully)
          {
            std::filesystem::path tempfilename(filename);
            Core::DisplayMessage(
                fmt::format("Loaded State from {}", tempfilename.filename().string()), 2000);
            if (File::Exists(filename + ".dtm"))
              movie.LoadInput(filename + ".dtm");
            else if (!movie.IsJustStartingRecordingInputFromSaveState() &&
                     !movie.IsJustStartingPlayingInputFromSaveState())
              movie.EndPlayInput(false);
          }
          else
          {
            Core::DisplayMessage("The savestate could not be loaded", OSD::Duration::NORMAL);

            // since we could be in an inconsistent state now (and might crash or whatever), undo.
            UndoLoadState(system);
          }
        }

        if (s_on_after_load_callback)
          s_on_after_load_callback();
      },
      true);
}

void LoadAsWiiMix(Core::System& system, const std::string& filename)
{
  if (!Core::IsRunningOrStarting(system))
    return;

  if (NetPlay::IsNetPlayRunning())
  {
    OSD::AddMessage("Loading savestates is disabled in Netplay to prevent desyncs");
    return;
  }

  if (AchievementManager::GetInstance().IsHardcoreModeActive())
  {
    OSD::AddMessage("Loading savestates is disabled in RetroAchievements hardcore mode");
    return;
  }

  std::unique_lock lk(s_load_or_save_in_progress_mutex, std::try_to_lock);
  if (!lk)
    return;

  Core::RunOnCPUThread(
      system,
      [&] {
        // Save temp buffer for undo load state
        auto& movie = system.GetMovie();
        if (!movie.IsJustStartingRecordingInputFromSaveState())
        {
          std::lock_guard lk2(s_undo_load_buffer_mutex);
          WiiMixSaveToBuffer(system, s_undo_load_buffer);
          const std::string dtmpath = File::GetUserPath(D_STATESAVES_IDX) + "undo.dtm";
          if (movie.IsMovieActive())
            movie.SaveRecording(dtmpath);
          else if (File::Exists(dtmpath))
            File::Delete(dtmpath);
        }

        bool loaded = false;
        bool loadedSuccessfully = false;

        // brackets here are so buffer gets freed ASAP
        {
          std::vector<u8> buffer;
          LoadWiiMixFileStateData(filename, buffer);

          if (!buffer.empty())
          {
            loadedSuccessfully = WiiMixLoadFromBuffer(system, buffer);
            loaded = true;
          }
        }

        if (loaded)
        {
          if (loadedSuccessfully)
          {
            std::filesystem::path tempfilename(filename);
            Core::DisplayMessage(
                fmt::format("Loaded State from {}", tempfilename.filename().string()), 2000);
            if (File::Exists(filename + ".dtm"))
              movie.LoadInput(filename + ".dtm");
            else if (!movie.IsJustStartingRecordingInputFromSaveState() &&
                     !movie.IsJustStartingPlayingInputFromSaveState())
              movie.EndPlayInput(false);
          }
          else
          {
            Core::DisplayMessage("The savestate could not be loaded", OSD::Duration::NORMAL);

            // since we could be in an inconsistent state now (and might crash or whatever), undo.
            UndoWiiMixLoadState(system);
          }
        }

        if (s_on_after_load_callback)
          s_on_after_load_callback();
      },
      true);
}

void SetOnAfterLoadCallback(AfterLoadCallbackFunc callback)
{
  s_on_after_load_callback = std::move(callback);
}

void Init(Core::System& system)
{
  s_save_thread.Reset("Savestate Worker", [&system](CompressAndDumpState_args args) {
    CompressAndDumpState(system, args);

    {
      std::lock_guard lk(s_state_writes_in_queue_mutex);
      if (--s_state_writes_in_queue == 0)
        s_state_write_queue_is_empty.notify_all();
    }

    if (args.state_write_done_event)
      args.state_write_done_event->Set();
  });

  s_wiimix_save_thread.Reset("WiiMix Savestate Worker", [&system](CompressAndDumpState_args args) {
    // Tell it to call your new dump function
    CompressAndDumpWiiMixState(system, args);

    {
      std::lock_guard lk(s_state_writes_in_queue_mutex);
      if (--s_state_writes_in_queue == 0)
        s_state_write_queue_is_empty.notify_all();
    }

    if (args.state_write_done_event)
      args.state_write_done_event->Set();
  });
}

void Shutdown()
{
  s_save_thread.Shutdown();
  s_wiimix_save_thread.Shutdown();

  // swapping with an empty vector, rather than clear()ing
  // this gives a better guarantee to free the allocated memory right NOW (as opposed to, actually,
  // never)
  {
    std::lock_guard lk(s_undo_load_buffer_mutex);
    std::vector<u8>().swap(s_undo_load_buffer);
  }
}

static std::string MakeStateFilename(int number)
{
  return fmt::format("{}{}.s{:02d}", File::GetUserPath(D_STATESAVES_IDX),
                     SConfig::GetInstance().GetGameID(), number);
}

void Save(Core::System& system, int slot, bool wait)
{
  SaveAs(system, MakeStateFilename(slot), wait);
}

void SaveWiiMix(Core::System& system, int slot, bool wait)
{
  SaveAsWiiMix(system, MakeStateFilename(slot), wait);
}

void Load(Core::System& system, int slot)
{
  LoadAs(system, MakeStateFilename(slot));
}

void LoadWiiMix(Core::System& system, int slot)
{
  LoadAsWiiMix(system, MakeStateFilename(slot));
}

void LoadLastSaved(Core::System& system, int i)
{
  if (i <= 0)
  {
    Core::DisplayMessage("State doesn't exist", 2000);
    return;
  }

  std::vector<SlotWithTimestamp> used_slots = GetUsedSlotsWithTimestamp();
  if (static_cast<size_t>(i) > used_slots.size())
  {
    Core::DisplayMessage("State doesn't exist", 2000);
    return;
  }

  std::stable_sort(used_slots.begin(), used_slots.end(), CompareTimestamp);
  Load(system, (used_slots.end() - i)->slot);
}

void LoadLastSavedWiiMix(Core::System& system, int i)
{
  if (i <= 0)
  {
    Core::DisplayMessage("State doesn't exist", 2000);
    return;
  }

  std::vector<SlotWithTimestamp> used_slots = GetUsedSlotsWithTimestamp();
  if (static_cast<size_t>(i) > used_slots.size())
  {
    Core::DisplayMessage("State doesn't exist", 2000);
    return;
  }

  std::stable_sort(used_slots.begin(), used_slots.end(), CompareTimestamp);
  LoadWiiMix(system, (used_slots.end() - i)->slot);
}

// must wait for state to be written because it must know if all slots are taken
void SaveFirstSaved(Core::System& system)
{
  std::vector<SlotWithTimestamp> used_slots = GetUsedSlotsWithTimestamp();
  if (used_slots.size() < NUM_STATES)
  {
    // save to an empty slot
    Save(system, GetEmptySlot(used_slots), true);
    return;
  }

  // overwrite the oldest state
  std::stable_sort(used_slots.begin(), used_slots.end(), CompareTimestamp);
  Save(system, used_slots.front().slot, true);
}

void SaveFirstSavedWiiMix(Core::System& system)
{
  std::vector<SlotWithTimestamp> used_slots = GetUsedSlotsWithTimestamp();
  if (used_slots.size() < NUM_STATES)
  {
    // save to an empty slot
    SaveWiiMix(system, GetEmptySlot(used_slots), true);
    return;
  }

  // overwrite the oldest state
  std::stable_sort(used_slots.begin(), used_slots.end(), CompareTimestamp);
  SaveWiiMix(system, used_slots.front().slot, true);
}

// Load the last state before loading the state
void UndoLoadState(Core::System& system)
{
  std::lock_guard lk(s_undo_load_buffer_mutex);
  if (!s_undo_load_buffer.empty())
  {
    auto& movie = system.GetMovie();
    if (movie.IsMovieActive())
    {
      const std::string dtmpath = File::GetUserPath(D_STATESAVES_IDX) + "undo.dtm";
      if (File::Exists(dtmpath))
      {
        LoadFromBuffer(system, s_undo_load_buffer);
        movie.LoadInput(dtmpath);
      }
      else
      {
        PanicAlertFmtT("No undo.dtm found, aborting undo load state to prevent movie desyncs");
      }
    }
    else
    {
      LoadFromBuffer(system, s_undo_load_buffer);
    }
  }
  else
  {
    PanicAlertFmtT("There is nothing to undo!");
  }
}

void UndoWiiMixLoadState(Core::System& system)
{
  std::lock_guard lk(s_undo_load_buffer_mutex);
  if (!s_undo_load_buffer.empty())
  {
    auto& movie = system.GetMovie();
    if (movie.IsMovieActive())
    {
      const std::string dtmpath = File::GetUserPath(D_STATESAVES_IDX) + "undo.dtm";
      if (File::Exists(dtmpath))
      {
        WiiMixLoadFromBuffer(system, s_undo_load_buffer);
        movie.LoadInput(dtmpath);
      }
      else
      {
        PanicAlertFmtT("No undo.dtm found, aborting undo load state to prevent movie desyncs");
      }
    }
    else
    {
      WiiMixLoadFromBuffer(system, s_undo_load_buffer);
    }
  }
  else
  {
    PanicAlertFmtT("There is nothing to undo!");
  }
}

// Load the state that the last save state overwritten on
void UndoSaveState(Core::System& system)
{
  LoadAs(system, File::GetUserPath(D_STATESAVES_IDX) + "lastState.sav");
}

void UndoWiiMixSaveState(Core::System& system)
{
  LoadAsWiiMix(system, File::GetUserPath(D_STATESAVES_IDX) + "lastState." + WIIMIX_STATE_EXTENSION);
}

// -----------------------------------------------------------------
// WiiMix Determinism Diff Test
// -----------------------------------------------------------------
// This is your primary debugging tool.
// It proves that your save/load functions are working correctly.
// 1. Saves a "portable" state to buffer (A)
// 2. Steps the emulator one frame, saves the result (B)
// 3. Loads state (A) back (which must trigger host re-init)
// 4. Steps the emulator one frame, saves the result (C)
// 5. Compares B and C. If they match, your functions are deterministic.

// ORIGINAL
// void WiiMixStepNPauses(Core::System& system, int n)
// {
//   for (int i = 0; i < n; ++i)
//   {
//     // 1. Trigger the internal frame step logic.
//     // This sets s_frame_step = true and unpauses the system.
//     Core::DoFrameStep(system);

//     // 2. FORCE the "Frame Presented" signal.
//     // In Headless/NoGUI mode, the video backend often skips presentation or
//     // doesn't trigger the callback. This manually sets s_stop_frame_step = true.
//     // We do this immediately. The Emulator thread (Interpreter) is slow enough
//     // that this flag will definitely be set before the CPU reaches the next 
//     // VICallback (Field boundary), ensuring we stop at exactly the next Field.
//     Core::Callback_FramePresented(1.0);

//     // 3. Wait for the EmuThread to pause itself at the VI boundary.
//     // We use a small sleep to yield the Host thread while waiting.
//     // We also add a timeout just in case.
//     int timeout_ms = 5000;
//     while (Core::GetState(system) != Core::State::Paused)
//     {
//       std::this_thread::sleep_for(std::chrono::milliseconds(1));
//       timeout_ms--;
//       if (timeout_ms <= 0) {
//           printf("WII-MIX-ERROR: Timed out waiting for frame step to finish!\n");
//           // Force break to unstick the thread, though the test will likely fail
//           system.GetCPU().Break(); 
//           break;
//       }
//     }
//   }
// }

void WiiMixStepNPauses(Core::System& system, int n)
{
  for (int i = 0; i < n; ++i)
  {
    Core::DoFrameStep(system);
    Core::Callback_FramePresented(1.0);

    // CHANGE: Increase timeout from 2000 to 10000ms for slow Interpreter
    int timeout_ms = 10000; 
    while (Core::GetState(system) != Core::State::Paused)
    {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
      timeout_ms--;
      if (timeout_ms <= 0) {
          printf("WII-MIX-ERROR: Timed out waiting for frame step to finish! Force breaking.\n");
          system.GetCPU().Break(); 
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          return; // CRITICAL: Return immediately so we don't save a partial state
      }
    }
  }
}

int WiiMixIdentityTest(Core::System& system)
{
  printf("Starting WiiMix Identity Test...\n");

  // 1. Warm-up Phase
  // Run 10 frames to ensure the emulator is in a steady state (past boot ROM, etc).
  // Initial boot state often has unique flags that are cleared after first use.
  Core::SetState(system, Core::State::Paused, false);
  Core::RunOnCPUThread(system, [&]() {}, true); // Sync
  
  printf("Warming up (10 frames)...\n");
  WiiMixStepNPauses(system, 10);

  int exit_code = 0;
  std::vector<u8> buffer_A;
  std::vector<u8> buffer_B;

  Core::RunOnCPUThread(system, [&] {
    // 2. Save State A
    printf("Saving State A...\n");
    WiiMixSaveToBuffer(system, buffer_A);
    if (buffer_A.empty()) {
        printf("IDENTITY FAIL: Buffer A is empty.\n");
        exit_code = 2;
        return;
    }

    // 3. Load State A
    // Note: WiiMixLoadFromBufferEmuThread handles Poisoning internally if configured via CLI
    printf("Loading State A (Triggering Re-Init/Poison)...\n");
    bool load_ok = WiiMixLoadFromBufferEmuThread(system, buffer_A);
    if (!load_ok) {
        printf("IDENTITY FAIL: Load returned false.\n");
        exit_code = 3;
        return;
    }

    // 4. Save State B (Immediately after load)
    // Theoretically, State B should be bit-for-bit identical to State A
    printf("Saving State B...\n");
    WiiMixSaveToBuffer(system, buffer_B);

    // 5. Compare
    if (buffer_A.size() != buffer_B.size()) {
        printf("IDENTITY FAIL: Size mismatch. A: %zu, B: %zu\n", buffer_A.size(), buffer_B.size());
        exit_code = 10;
    } 
    else if (buffer_A != buffer_B) {
        // Find the first mismatch offset for debugging
        size_t mismatch_idx = 0;
        for (; mismatch_idx < buffer_A.size(); ++mismatch_idx) {
            if (buffer_A[mismatch_idx] != buffer_B[mismatch_idx]) break;
        }
        printf("IDENTITY FAIL: Content mismatch at offset 0x%zX\n", mismatch_idx);
        exit_code = 10;
    }
    else {
        printf("Identity Test PASSED! Load(Save(S)) == S\n");
        exit_code = 0;
    }

    // Dump files if failed
    if (exit_code != 0) {
        File::IOFile file_a("Identity_A.bin", "wb");
        if(file_a) file_a.WriteBytes(buffer_A.data(), buffer_A.size());
        
        File::IOFile file_b("Identity_B.bin", "wb");
        if(file_b) file_b.WriteBytes(buffer_B.data(), buffer_B.size());
        printf("Dumped Identity_A.bin and Identity_B.bin for analysis.\n");
    }

    Core::Stop(system);
  }, true);

  return exit_code;
}

int WiiMixDiffTest(Core::System& system, int steps)
{
  printf("Starting WiiMix Diff Test (%d frames)...\n", steps);
  int exit_code = 1;
  std::vector<u8> buffer_A, buffer_B, buffer_C;
  bool emu_thread_ok = true;

  // Pause the EmuThread
  Core::SetState(system, Core::State::Paused, false);
  Core::RunOnCPUThread(system, [&]() {}, true);

  // --- Save A (The "Anchor" State) ---
  Core::RunOnCPUThread(system, [&] {
    printf("WII-MIX-TEST-LOG: Save A START (Frame: %ld)\n", system.GetMovie().GetCurrentFrame());
    WiiMixSaveToBuffer(system, buffer_A);
    if (buffer_A.empty()) { emu_thread_ok = false; return; }
  }, true);

  if (!emu_thread_ok) {
    TEST_FAIL(2, "Diff Test FAILED: State A was empty.");
    return 2;
  }

  // --- Step N Frames (Path 1) ---
  printf("Stepping %d frames (Continuous)...\n", steps);
  
  // NOTE: Tracing disabled for speed. Uncomment only for deep debugging single frames.
  // Core::RunOnCPUThread(system, [&] { GetInterpreter(system)->StartPCTrace("trace_B.txt"); }, true);
  WiiMixStepNPauses(system, steps);
  // Core::RunOnCPUThread(system, [&] { GetInterpreter(system)->StopPCTrace(); }, true);

  // --- Save B (Continuous Result) ---
  Core::RunOnCPUThread(system, [&] {
    printf("WII-MIX-TEST-LOG: Save B START (Frame: %ld)\n", system.GetMovie().GetCurrentFrame());
    WiiMixSaveToBuffer(system, buffer_B);
    if (buffer_B.empty()) { emu_thread_ok = false; return; }
  }, true);

  if (!emu_thread_ok) {
    TEST_FAIL(4, "Diff Test FAILED: State B was empty.");
    return 4;
  }

  // --- Load A (Reset) ---
  printf("Loading State A...\n");
  Core::RunOnCPUThread(system, [&] {
    // Poisoning is handled inside WiiMixLoadFromBufferEmuThread based on Config::WIIMIX_POISON_MASK
    if (!WiiMixLoadFromBufferEmuThread(system, buffer_A)) {
      emu_thread_ok = false;
    }
  }, true);

  if (!emu_thread_ok) {
    TEST_FAIL(6, "Diff Test FAILED: Loading Emulated State failed.");
    return 6;
  }
   
  // --- Step N Frames (Path 2) ---
  printf("Stepping %d frames (Loaded)...\n", steps);

  // NOTE: Tracing disabled for speed.
  // Core::RunOnCPUThread(system, [&] { GetInterpreter(system)->StartPCTrace("trace_C.txt"); }, true);
  WiiMixStepNPauses(system, steps);
  // Core::RunOnCPUThread(system, [&] { GetInterpreter(system)->StopPCTrace(); }, true);

  // --- Save C (Loaded Result) ---
  Core::RunOnCPUThread(system, [&] {
    printf("WII-MIX-TEST-LOG: Save C START (Frame: %ld)\n", system.GetMovie().GetCurrentFrame());
    WiiMixSaveToBuffer(system, buffer_C);
    
    if (buffer_C.empty()) {
      TEST_FAIL(8, "Diff Test FAILED: State C was empty.");
      exit_code = 8;
    } else if (buffer_B.size() != buffer_C.size()) {
      TEST_FAIL(9, "Diff Test FAILED: Size mismatch! B=%zu, C=%zu.", buffer_B.size(), buffer_C.size());
      exit_code = 9;
    } else if (buffer_B == buffer_C) {
      printf("Diff Test PASSED! State is deterministic.\n");
      exit_code = 0;
    } else {
      auto mismatch = std::mismatch(buffer_B.begin(), buffer_B.end(), buffer_C.begin()).first;
      size_t offset = static_cast<size_t>(mismatch - buffer_B.begin());
      TEST_FAIL(10, "Diff Test FAILED: Mismatch at byte 0x%zX Your save/load logic is not deterministic.", offset);
      exit_code = 10;
    }
    
    if (exit_code != 0) {
      File::IOFile file_b("B.bin", "wb");
      if (file_b) file_b.WriteBytes(buffer_B.data(), buffer_B.size());
      File::IOFile file_c("C.bin", "wb");
      if (file_c) file_c.WriteBytes(buffer_C.data(), buffer_C.size());
    }
    
    Core::Stop(system);
  }, true); 
     
  return exit_code;
}
}  // namespace State