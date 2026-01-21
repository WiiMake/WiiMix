# Python script that compiles useful scripts into a prompt
PROMPT = """
I'm currently working on a fork of the Dolphin Emulator WiiMix that allows sharing and loading savestates between machines on different architectures. My goal is to make the wiimix savestate "pure"; that is, no host-specific data is saved and any host specific components are reinitialized accordingly so the state is appropriate and can be run from any machine.

Here's is my current State.cpp file:

==================================State.cpp====================================
{}
===============================================================================

Note that most functions have corresponding "WiiMix" methods as well so I don't overwrite any of the original logic and can reference it as needed. Here are a couple of key methods:

DoWiiMixState: this is where we decide what to save in save states. More on this later

WiiMixSaveFromBuffer: this should handle saving the pure savestate

WiiMixLoadFromBufferEmuThread: this should load the pure savestate and trigger any necessary reinitialization

WiiMixHostReinitialization: this should reinitialize any necessary host components WITHOUT overwriting any pure state. Each component has a dedicated WiiMixReset function that I've implemented to ensure that nothing unnecessary gets reset or cleared and that events get registered properly.

WiiMixDiffTest: this is the testing function; currently testing is being done with the following command

./Binaries/wiimix-nogui --diff-test -e ~/Programming/OpenSource/gamecube-examples/bin/acube.dol --interpreter

Where --diff-test indicates to  run the diff-test and a really basic gamecube program that just displays a cube is started on the nogui version, ran with the interpreter instead of JIT for simplicity right now.

Optionally, --frames x can also be added to run x frames instead of 1 frame in between saving and loading states.

Here's some more complete information for reference. You already have state.cpp, but here's a couple more important files.

=========================PowerPC.cpp===================================
{}
================================VideoBackendBase.cpp=====================
{}
==========================CoreTiming.cpp===============================
{}
=========================HW.cpp=======================================
{}
==============================MainNoGui.cpp==============================
{}
==============================Core.cpp==========================================
{}
==================================SystemTimers.cpp===================================
{}
====================================AudioInterface.cpp=================================
{}
====================================ProcessorInterface.cpp=============================
{}
====================================SI.cpp=================================
{}
====================================DSP.cpp=================================
{}
====================================DVDInterface.cpp=================================
{}
====================================DVDThread.cpp=================================
{}
====================================VideoInterface.cpp=================================
{}
====================================EXI.cpp=============================
{}
====================================EXI_Channel.cpp=============================
{}
====================================Memmap.cpp=================================
{}
====================================HSP.cpp=================================
{}
====================================GPFifo.cpp=================================
{}
====================================System.cpp=================================
{}
===============================================================================

Here's what my current debug output looks like:

============================scan_results===========================================
{}
======================================================================

I can provide more context and references as needed, but that's the primary output that I'm working with right now. 

With all of that in mind, use your c++ knowledge and knowledge of Dolphin Emulator to help me out. I need to resolve this issue so I can work towards deterministic save states across any number of frames and any architecture.

Think logically and with a C++ developer mindset; what might be the possible issues for the non-determinism for multiple steps? And how could it be resolved? If you need any more information, let me know - I want to do whatever I can to make sure that my savestates are completely deterministic and stable. Please make sure to explain your answers by referencing code segments.
"""

WIIMIXSTATEFUNCS = """
bool WiiMixHostReinitialization(Core::System& system) {
  system.GetCoreTiming().LogTimingState("Before Host Reinit");
  system.GetCoreTiming().WiiMixReset(); // Resets timer, clears event *types*
  system.GetCoreTiming().LogTimingState("After Host Reinit");

  // 1. Stop all host threads
  system.GetDSP().Shutdown();
  system.GetDVDInterface().Shutdown();
  AudioCommon::WiiMixShutdownSoundStream(system);
  system.GetPowerPC().Shutdown(); 

  // 2. Register All Event Handlers AND Re-init Hardware
  // This re-populates the event map *and* resets hardware to a clean state.
  // This will schedule initial boot-time events (which we want to clear later).
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

  // 4. FULLY Re-initialize threaded components to a clean boot state
  // We are NOT calling the "WiiMixRestart" functions here. We are
  // calling the *full* re-initialization logic.
  system.GetDSP().WiiMixReset();
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

  // 1. Run Re-init. This registers event *types* (good)
  //    but also pollutes the event *queue* with default events (bad).
  if (p.IsReadMode()) {
    success = WiiMixHostReinitialization(system);
    if (!success)
      return false;

    system.GetCoreTiming().LogEventQueues("After Host Reinit");
  }

  // 2. Clear the polluted event queue.
  //    Now CoreTiming has all types registered, but an empty queue.
  if (p.IsReadMode()) {
    // Move any rogue events from host threads (e.g., Audio)
    // into the main queue, so ClearPendingEvents can kill them.
    system.GetCoreTiming().MoveEvents();
    system.GetCoreTiming().LogEventQueues("After MoveEvents");
    system.GetCoreTiming().ClearPendingEvents();
    system.GetCoreTiming().LogEventQueues("After ClearPendingEvents");
  }
  
  // Reset JIT cache before loading state
  // Loading state will force JIT EmuThread JIT cache to reload
  // NOTE: don't call shutdown() as it will result in a use after free error
  // system.GetCoreTiming().LogTimingState("Before EmuThread::DoState CoreTiming");
  // system.GetCoreTiming().DoState(p);
  // p.DoMarker("CoreTiming");
  // system.GetCoreTiming().LogTimingState("After EmuThread::DoState CoreTiming");

  // system.GetCoreTiming().LogTimingState("Before PowerPC init");
  // const PowerPC::CPUCore active_core =
  //     static_cast<PowerPC::CPUCore>(Config::Get(Config::MAIN_CPU_CORE));
  // power_pc.Init(active_core);
  printf("Current frame in EmuThread before DoWiiMixState: %ld\n", system.GetMovie().GetCurrentFrame());
  // system.GetCoreTiming().LogTimingState("After PowerPC init, Before DoWiiMixState");
  DoWiiMixState(system, p);
  system.GetCoreTiming().LogEventQueues("After DoWiiMixState (Pure State Loaded)");

  if (p.IsReadMode()) {
    // 4. Start the host threads. This WILL schedule rogue events.
    system.GetAudioInterface().Init();
    AudioCommon::PostInitSoundStream(system);
    system.GetDVDInterface().Init();
    system.GetDSP().Init(Config::Get<bool>(Config::MAIN_DSP_HLE));
  }

  // Refresh cpu base pointer to point to valid JIT
  power_pc.ApplyMode();

  auto& ppc_state = power_pc.GetPPCState();
  auto& memory = system.GetMemory();

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

  printf("Current frame at end of EmuThread: %ld\n", system.GetMovie().GetCurrentFrame());
  system.GetCoreTiming().LogEventQueues("After Cache Refresh");
  // system.GetCPU().Break();
  return p.IsReadMode();
}
"""

# OUTPUT = """
# /Binaries/wiimix-nogui --diff-test --interpreter -e ~/dolphingames/F-Zero\ GX\ \(USA\).ciso
# INTERPRETER IS SET
# Initializing HW...
# Initializing DSP...
# Starting WiiMix Diff Test...
# Save A to buffer (at Frame 0)
# WII-MIX-TEST-LOG: Save A START (Current Frame: 1)
# WII-MIX-MAP: Offset after Movie: 0x36
# WII-MIX-MAP: Offset after video_backend: 0x3048DC
# TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
# TIMING_LOG (Event Queue): event_count = 7
#     Event: VICallback, Time: 1953666, Fifo Order: 287, Userdata: 0
#     Event: GPUSleeper, Time: 2430000, Fifo Order: 285, Userdata: 0
#     Event: AudioDMACallback, Time: 2063664, Fifo Order: 284, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
#     Event: PatchEngine, Time: 4054050, Fifo Order: 6, Userdata: 0
#     Event: FinishExecutingCommand, Time: 3398976, Fifo Order: 250, Userdata: 12884901889
#     Event: DSPCallback, Time: 2430000, Fifo Order: 286, Userdata: 0
# WII-MIX-MAP: Offset after CoreTiming: 0x304A43
# TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
# WII-MIX-MAP: Offset after Memory: 0x2344A65
# WII-MIX-MAP: Offset after MemoryInterface: 0x2344AB1
# WII-MIX-MAP: Offset after VideoInterface: 0x2344B57
# WII-MIX-MAP: Offset after SerialInterface: 0x2344C46
# WII-MIX-MAP: Offset after ProcessorInterface: 0x2344C62
# WII-MIX-MAP: Offset after after main dsp state: 0x3344C8A
# WII-MIX-MAP: Offset after DSP: 0x3344CF5
# WII-MIX-MAP: Offset after DVDInterface error code state: 0x3344D4A
# WII-MIX-MAP: Offset after DVDInterface buffer state: 0x3344D6A
# WII-MIX-MAP: Offset after DVDThread state: 0x3344D77
# WII-MIX-MAP: Offset after DVDInterface: 0x3344D8B
# WII-MIX-MAP: Offset after GPFifo: 0x3344F93
# WII-MIX-MAP: Offset after ExpansionInterface: 0x33450DB
# WII-MIX-MAP: Offset after AudioInterface: 0x3345167
# WII-MIX-MAP: Offset after HSP: 0x334516F
# WII-MIX-MAP: Offset after HW: 0x3345177
# WII-MIX-MAP: Offset after PowerPC: 0x33599A4
# WII-MIX-MAP: Offset after AchievementManager: 0x33599A8
# TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
# TIMING_LOG (Event Queue): event_count = 7
#     Event: VICallback, Time: 1953666, Fifo Order: 287, Userdata: 0
#     Event: GPUSleeper, Time: 2430000, Fifo Order: 285, Userdata: 0
#     Event: AudioDMACallback, Time: 2063664, Fifo Order: 284, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
#     Event: PatchEngine, Time: 4054050, Fifo Order: 6, Userdata: 0
#     Event: FinishExecutingCommand, Time: 3398976, Fifo Order: 250, Userdata: 12884901889
#     Event: DSPCallback, Time: 2430000, Fifo Order: 286, Userdata: 0
# TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
# WII-MIX-TEST-LOG: Save A END (Current Frame: 1)
# Step Frame 1 (for State B)
# WII-MIX-TRACE: Started PC trace to trace_B.txt
# WII-MIX-TRACE: Stopped PC trace.
# Save B to buffer (at Frame 1)
# WII-MIX-TEST-LOG: Save B START (Current Frame: 2)
# WII-MIX-MAP: Offset after Movie: 0x36
# WII-MIX-MAP: Offset after video_backend: 0x3048DC
# TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7717, global_timer = 4061777
# TIMING_LOG (Event Queue): event_count = 7
#     Event: VICallback, Time: 4069494, Fifo Order: 590, Userdata: 0
#     Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
#     Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
#     Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
#     Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
#     Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
# WII-MIX-MAP: Offset after CoreTiming: 0x304A43
# TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7717, global_timer = 4061777
# WII-MIX-MAP: Offset after Memory: 0x2344A65
# WII-MIX-MAP: Offset after MemoryInterface: 0x2344AB1
# WII-MIX-MAP: Offset after VideoInterface: 0x2344B57
# WII-MIX-MAP: Offset after SerialInterface: 0x2344C46
# WII-MIX-MAP: Offset after ProcessorInterface: 0x2344C62
# WII-MIX-MAP: Offset after after main dsp state: 0x3344C8A
# WII-MIX-MAP: Offset after DSP: 0x3344CF5
# WII-MIX-MAP: Offset after DVDInterface error code state: 0x3344D4A
# WII-MIX-MAP: Offset after DVDInterface buffer state: 0x3344D6A
# WII-MIX-MAP: Offset after DVDThread state: 0x3344D77
# WII-MIX-MAP: Offset after DVDInterface: 0x3344D8B
# WII-MIX-MAP: Offset after GPFifo: 0x3344F93
# WII-MIX-MAP: Offset after ExpansionInterface: 0x33450DB
# WII-MIX-MAP: Offset after AudioInterface: 0x3345167
# WII-MIX-MAP: Offset after HSP: 0x334516F
# WII-MIX-MAP: Offset after HW: 0x3345177
# WII-MIX-MAP: Offset after PowerPC: 0x33599A4
# WII-MIX-MAP: Offset after AchievementManager: 0x33599A8
# TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7717, global_timer = 4061777
# TIMING_LOG (Event Queue): event_count = 7
#     Event: VICallback, Time: 4069494, Fifo Order: 590, Userdata: 0
#     Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
#     Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
#     Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
#     Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
#     Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
# TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7717, global_timer = 4061777
# WII-MIX-TEST-LOG: Save B END (Current Frame: 2)
# Load from EmuThread (to Frame 0)
# WII-MIX-TEST-LOG: Load A START (Current Frame: 2)
# --- EVENT QUEUE LOG at: Start of Load Function ---
#   global_timer = 4061777
#   m_event_queue (7 events):
#     Event: VICallback, Time: 4069494, Userdata: 0
#     Event: AudioDMACallback, Time: 4127328, Userdata: 0
#     Event: GPUSleeper, Time: 4374000, Userdata: 0
#     Event: DSPCallback, Time: 4374000, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Userdata: 0
#     Event: FinishExecutingCommand, Time: 5098464, Userdata: 12884901889
#     Event: PatchEngine, Time: 8108100, Userdata: 0
#   m_ts_queue (0 events):
#     [EMPTY]
# ---------------------------------------
# TIMING_LOG (Before Host Reinit): slice_length = 7717, global_timer = 4061777
# TIMING_LOG (After Host Reinit): slice_length = 20000, global_timer = 0
# --- EVENT QUEUE LOG at: After Host Reinit ---
#   global_timer = 0
#   m_event_queue (0 events):
#     [EMPTY]
#   m_ts_queue (0 events):
#     [EMPTY]
# ---------------------------------------
# --- EVENT QUEUE LOG at: After MoveEvents ---
#   global_timer = 0
#   m_event_queue (0 events):
#     [EMPTY]
#   m_ts_queue (0 events):
#     [EMPTY]
# ---------------------------------------
# --- EVENT QUEUE LOG at: After ClearPendingEvents ---
#   global_timer = 0
#   m_event_queue (0 events):
#     [EMPTY]
#   m_ts_queue (0 events):
#     [EMPTY]
# ---------------------------------------
# Current frame in EmuThread before DoWiiMixState: 2
# TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
#     Event: VICallback, Time: 1953666, Fifo Order: 287, Userdata: 0
#     Event: AudioDMACallback, Time: 2063664, Fifo Order: 284, Userdata: 0
#     Event: GPUSleeper, Time: 2430000, Fifo Order: 285, Userdata: 0
#     Event: DSPCallback, Time: 2430000, Fifo Order: 286, Userdata: 0
#     Event: FinishExecutingCommand, Time: 3398976, Fifo Order: 250, Userdata: 12884901889
#     Event: PatchEngine, Time: 4054050, Fifo Order: 6, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
# TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
# --- EVENT QUEUE LOG at: After DoWiiMixState (Pure State Loaded) ---
#   global_timer = 1946320
#   m_event_queue (7 events):
#     Event: VICallback, Time: 1953666, Userdata: 0
#     Event: AudioDMACallback, Time: 2063664, Userdata: 0
#     Event: GPUSleeper, Time: 2430000, Userdata: 0
#     Event: DSPCallback, Time: 2430000, Userdata: 0
#     Event: FinishExecutingCommand, Time: 3398976, Userdata: 12884901889
#     Event: PatchEngine, Time: 4054050, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Userdata: 0
#   m_ts_queue (0 events):
#     [EMPTY]
# ---------------------------------------
# Current frame at end of EmuThread: 1
# --- EVENT QUEUE LOG at: After Cache Refresh ---
#   global_timer = 1946320
#   m_event_queue (7 events):
#     Event: VICallback, Time: 1953666, Userdata: 0
#     Event: AudioDMACallback, Time: 2063664, Userdata: 0
#     Event: GPUSleeper, Time: 2430000, Userdata: 0
#     Event: DSPCallback, Time: 2430000, Userdata: 0
#     Event: FinishExecutingCommand, Time: 3398976, Userdata: 12884901889
#     Event: PatchEngine, Time: 4054050, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Userdata: 0
#   m_ts_queue (0 events):
#     [EMPTY]
# ---------------------------------------
# TIMING_LOG (After WiiMixLoadFromBufferEmuThread for Diff Test): slice_length = 1, global_timer = 1946320
# WII-MIX-TEST-LOG: Load A END (Current Frame: 1)
# Step Frame 1 (for State C)
# WII-MIX-TRACE: Started PC trace to trace_C.txt
# WII-MIX-TRACE: Stopped PC trace.
# Save C to Buffer (at Frame 1)
# WII-MIX-TEST-LOG: Save C START (Current Frame: 2)
# WII-MIX-MAP: Offset after Movie: 0x36
# WII-MIX-MAP: Offset after video_backend: 0x3048DC
# TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7717, global_timer = 4061777
# TIMING_LOG (Event Queue): event_count = 7
#     Event: VICallback, Time: 4069494, Fifo Order: 590, Userdata: 0
#     Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
#     Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
#     Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
#     Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
#     Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
# WII-MIX-MAP: Offset after CoreTiming: 0x304A43
# TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7717, global_timer = 4061777
# WII-MIX-MAP: Offset after Memory: 0x2344A65
# WII-MIX-MAP: Offset after MemoryInterface: 0x2344AB1
# WII-MIX-MAP: Offset after VideoInterface: 0x2344B57
# WII-MIX-MAP: Offset after SerialInterface: 0x2344C46
# WII-MIX-MAP: Offset after ProcessorInterface: 0x2344C62
# WII-MIX-MAP: Offset after after main dsp state: 0x3344C8A
# WII-MIX-MAP: Offset after DSP: 0x3344CF5
# WII-MIX-MAP: Offset after DVDInterface error code state: 0x3344D4A
# WII-MIX-MAP: Offset after DVDInterface buffer state: 0x3344D6A
# WII-MIX-MAP: Offset after DVDThread state: 0x3344D77
# WII-MIX-MAP: Offset after DVDInterface: 0x3344D8B
# WII-MIX-MAP: Offset after GPFifo: 0x3344F93
# WII-MIX-MAP: Offset after ExpansionInterface: 0x33450DB
# WII-MIX-MAP: Offset after AudioInterface: 0x3345167
# WII-MIX-MAP: Offset after HSP: 0x334516F
# WII-MIX-MAP: Offset after HW: 0x3345177
# WII-MIX-MAP: Offset after PowerPC: 0x33599A4
# WII-MIX-MAP: Offset after AchievementManager: 0x33599A8
# TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7717, global_timer = 4061777
# TIMING_LOG (Event Queue): event_count = 7
#     Event: VICallback, Time: 4069494, Fifo Order: 590, Userdata: 0
#     Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
#     Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
#     Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
#     Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
#     Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
#     Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
# TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7717, global_timer = 4061777
# WII-MIX-TEST-LOG: Save C END (Current Frame: 2)
# Diff Test FAILED at /home/xanmankey/Programming/OpenSource/WiiMix/Source/Core/Core/State.cpp:2254: Diff Test FAILED: Mismatch at byte 0x3345103 Your save/load logic is not deterministic.
# Dumping B and C to files
# Stopping System
# s_platform.reset() in main
# IniFile::Save: /home/xanmankey/.config/dolphin-emu/Dolphin.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Dolphin.ini
# IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCPadNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCPadNew.ini
# IniFile::Save: /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini
# IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini
# IniFile::Save: /home/xanmankey/.config/dolphin-emu/GFX.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GFX.ini
# IniFile::Save: /home/xanmankey/.config/dolphin-emu/Logger.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Logger.ini
# IniFile::Save: /home/xanmankey/.config/dolphin-emu/DSUClient.ini.xxx -> /home/xanmankey/.config/dolphin-emu/DSUClient.ini
# IniFile::Save: /home/xanmankey/.config/dolphin-emu/FreeLook.ini.xxx -> /home/xanmankey/.config/dolphin-emu/FreeLook.ini
# IniFile::Save: /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini.xxx -> /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini
# """

# Helper to format the DoState methods for each major subsystem
# These are extracted from the PROMPT above for clarity in the prompt

state_cpp_do_state = """
State.cpp::DoWiiMixState:
bool is_wii = system.IsWii() || system.IsMIOS();
const bool is_wii_currently = is_wii;
p.Do(is_wii);
if (is_wii != is_wii_currently) {
OSD::AddMessage(fmt::format("Cannot load a savestate created under {} mode in {} mode",
                            is_wii ? "Wii" : "GC", is_wii_currently ? "Wii" : "GC"),
                OSD::Duration::NORMAL, OSD::Color::RED);
p.SetMeasureMode();
return;
}
auto& memory = system.GetMemory();
u32 state_mem1_size = memory.GetRamSizeReal();
u32 state_mem2_size = memory.GetExRamSizeReal();
p.Do(state_mem1_size);
p.Do(state_mem2_size);
if (state_mem1_size != memory.GetRamSizeReal() || state_mem2_size != memory.GetExRamSizeReal()) {
OSD::AddMessage(fmt::format("Memory size mismatch! ..."), ...);
p.SetMeasureMode();
return;
}
"""

movie_manager_do_state = """
MovieManager::DoState(PointerWrap& p):
p.Do(m_current_frame);
p.Do(m_current_byte);
p.Do(m_current_lag_count);
p.Do(m_current_input_count);
p.Do(m_polled);
p.Do(m_tick_count_at_last_input);
"""

video_backend_do_state = """
VideoBackendBase::DoState(PointerWrap& p):
if (!system.IsDualCoreMode()) {
VideoCommon_DoState(p);
return;
}
AsyncRequests::Event ev = {};
ev.do_save_state.p = &p;
ev.type = AsyncRequests::Event::DO_SAVE_STATE;
AsyncRequests::GetInstance()->PushEvent(ev, true);
system.GetFifo().GpuMaySleep();
"""

video_common_do_state = """
void VideoCommon_DoState(PointerWrap& p)
{
  if (!WIIMIX_STATE) {
    bool software = false;
    p.Do(software);

    if (p.IsReadMode() && software == true)
    {
      // change mode to abort load of incompatible save state.
      p.SetVerifyMode();
    }
  }

  // BP Memory
  p.Do(bpmem);
  p.DoMarker("BP Memory");

  // CP Memory
  // We don't save g_preprocess_cp_state separately because the GPU should be
  // synced around state save/load.
  p.Do(g_main_cp_state);
  p.DoMarker("CP Memory");
  if (p.IsReadMode())
    CopyPreprocessCPStateFromMain();

  // XF Memory
  p.Do(xfmem);
  p.DoMarker("XF Memory");

  // Texture decoder
  p.DoArray(s_tex_mem);
  p.DoMarker("texMem");

  // TMEM
  // if (!WIIMIX_STATE) {
    // TMEM is a voltatile cache, so we don't save it in WiiMix states
    TMEM::DoState(p);
    p.DoMarker("TMEM");
  // }

  // FIFO
  auto& system = Core::System::GetInstance();
  system.GetFifo().DoState(p);
  p.DoMarker("Fifo");

  auto& command_processor = system.GetCommandProcessor();
  command_processor.DoState(p);
  p.DoMarker("CommandProcessor");

  system.GetPixelEngine().DoState(p);
  p.DoMarker("PixelEngine");

  // the old way of replaying current bpmem as writes to push side effects to pixel shader manager
  // doesn't really work.
  if (!WIIMIX_STATE) {
    system.GetPixelShaderManager().DoState(p);
    p.DoMarker("PixelShaderManager");

    system.GetVertexShaderManager().DoState(p);
    p.DoMarker("VertexShaderManager");

    system.GetGeometryShaderManager().DoState(p);
    p.DoMarker("GeometryShaderManager");
  }

  // Do not save the host's vertex cache for WiiMix states
  if (!WIIMIX_STATE) {
    g_vertex_manager->DoState(p);
    p.DoMarker("VertexManager");
  }

  // Do not save the host's framebuffer cache for WiiMix states
  if (!WIIMIX_STATE) {
    g_framebuffer_manager->DoState(p);
    p.DoMarker("FramebufferManager");
  }

  // Do not save the host's texture cache for WiiMix states
  if (!WIIMIX_STATE) {
    g_texture_cache->DoState(p);
    p.DoMarker("TextureCache");
  }

  // Do not save host-specific rendering logic for WiiMix states
  if (!WIIMIX_STATE) {
    g_presenter->DoState(p);
    g_frame_dumper->DoState(p);
    p.DoMarker("Presenter");
  }

  // Do not save host's bounding box state (host optimization) for WiiMix states
  if (!WIIMIX_STATE) {
    g_bounding_box->DoState(p);
    p.DoMarker("Bounding Box");
  }

  // Do not save host's widescreen config for WiiMix states
  if (!WIIMIX_STATE) {
    g_widescreen->DoState(p);
    p.DoMarker("Widescreen");
  }

  system.GetXFStateManager().DoState(p);
  p.DoMarker("XFStateManager");

  // Refresh state.
  // Already handled in WiiMixHostReinitialization
  if (p.IsReadMode())
  {
    // Inform backend of new state from registers.
    BPReload();
    VertexLoaderManager::MarkAllDirty();
  }
}
"""

core_timing_do_state = """
void CoreTimingManager::DoState(PointerWrap& p)
{
  std::lock_guard lk(m_ts_write_lock);
  // Treat slice length as volatile for WiiMix states
  // if (!WIIMIX_STATE) {
    // printf("Why is slice length being loaded? THIS IS WRONG\n");
    p.Do(m_globals.slice_length);
  // }
  p.Do(m_globals.global_timer); // 8
  // m_idled_cycles is host-dependent, volatile data.
  // It is not part of the pure emulated state.
  if (!WIIMIX_STATE)
  {
    p.Do(m_idled_cycles);
    p.Do(m_last_oc_factor); // 4
  }
  p.Do(m_fake_dec_start_value); // 4
  p.Do(m_fake_dec_start_ticks); // 8
  p.Do(m_globals.fake_TB_start_value); // 8
  p.Do(m_globals.fake_TB_start_ticks); // 8
  if (p.IsReadMode()) {
    if (WIIMIX_STATE) {
      m_last_oc_factor = m_config_oc_factor;
      m_globals.last_OC_factor_inverted = m_config_oc_inv_factor;
    }
    else {
      m_globals.last_OC_factor_inverted = 1.0f / m_last_oc_factor;
    }
  }

  // if (!WIIMIX_STATE) {
    p.Do(m_event_fifo_id); // 8
  // }

  p.DoMarker("CoreTimingData"); // 4

  // Serialize event queue
  MoveEvents();
  size_t event_count;
  if (!p.IsReadMode())
    event_count = m_event_queue.size();
  
  p.Do(event_count); // Save/load the event count

  if (p.IsReadMode())
  {
    // --- LOAD SIDE ---
    m_event_queue.resize(event_count);
    for (size_t i = 0; i < event_count; ++i)
    {
      Event& ev = m_event_queue[i];
      p.Do(ev.time);
      p.Do(ev.fifo_order);
      p.Do(ev.userdata);

      std::string name;
      p.Do(name);
      auto itr = m_event_types.find(name);
      if (itr != m_event_types.end())
      {
        ev.type = &itr->second;
      }
      else
      {
        if (WIIMIX_STATE) {
          printf("TIMING_LOG (Load Event): *** LOST EVENT: '%s' *** because it's type has not been registered\n", name.c_str());
        }
        WARN_LOG_FMT(POWERPC,
                    "Lost event from savestate because its type, \"{}\", has not been registered.",
                    name);
        ev.type = m_ev_lost;
      }
    }
  }
  else
  {
    // --- SAVE SIDE ---
    // Create a sorted copy to guarantee a deterministic binary file
    printf("TIMING_LOG (Event Queue): event_count = %zu\n", event_count);
    std::vector<Event> sorted_queue = m_event_queue;
    std::sort(sorted_queue.begin(), sorted_queue.end(), CompareEventsForState);

    // Save each event from the *sorted* copy
    for (const Event& ev : sorted_queue)
    {
      // p.Do wants a non-const ref, so we must const_cast
      p.Do(ev.time);
      p.Do(ev.fifo_order);
      p.Do(ev.userdata);

      std::string name = *ev.type->name;
      p.Do(name);
    }
  }

  for (const auto& ev : m_event_queue)
  {
    printf("    Event: %s, Time: %ld, Fifo Order: %ld, Userdata: %ld\n",
           ev.type && ev.type->name ? ev.type->name->c_str() : "(null)",
           ev.time, ev.fifo_order, ev.userdata);
  }

  p.DoMarker("CoreTimingEvents");

  if (p.IsReadMode())
  {
    // When loading from a save state, we must assume the Event order is random and meaningless.
    // The exact layout of the heap in memory is implementation defined, therefore it is platform
    // and library version specific.
    std::make_heap(m_event_queue.begin(), m_event_queue.end(), std::greater<Event>());

    // The stave state has changed the time, so our previous Throttle targets are invalid.
    // Especially when global_time goes down; So we create a fake throttle update.
    ResetThrottle(m_globals.global_timer);
  }
}
"""

hw_do_state = """
HW::DoState(Core::System& system, PointerWrap& p):
system.GetMemory().DoState(p);
p.DoMarker("Memory");
system.GetMemoryInterface().DoState(p);
p.DoMarker("MemoryInterface");
system.GetVideoInterface().DoState(p);
p.DoMarker("VideoInterface");
system.GetSerialInterface().DoState(p);
p.DoMarker("SerialInterface");
system.GetProcessorInterface().DoState(p);
p.DoMarker("ProcessorInterface");
system.GetDSP().DoState(p);
p.DoMarker("DSP");
system.GetDVDInterface().DoState(p);
p.DoMarker("DVDInterface");
system.GetGPFifo().DoState(p);
p.DoMarker("GPFifo");
system.GetExpansionInterface().DoState(p);
p.DoMarker("ExpansionInterface");
system.GetAudioInterface().DoState(p);
p.DoMarker("AudioInterface");
system.GetHSP().DoState(p);
p.DoMarker("HSP");
if (system.IsWii()) {
system.GetWiiIPC().DoState(p);
p.DoMarker("IOS");
system.GetIOS()->DoState(p);
p.DoMarker("IOS::HLE");
}
p.DoMarker("WIIHW");
"""

powerpc_do_state = """
void PowerPCManager::DoState(PointerWrap& p)
{
  // some of this code has been disabled, because
  // it changes registers even in Mode::Measure (which is suspicious and seems like it could cause
  // desyncs)
  // and because the values it's changing have been added to CoreTiming::DoState, so it might
  // conflict to mess with them here.

  // m_ppc_state.spr[SPR_DEC] = SystemTimers::GetFakeDecrementer();
  // *((u64 *)&TL(m_ppc_state)) = SystemTimers::GetFakeTimeBase(); //works since we are little
  // endian and TL comes first :)

  p.DoArray(m_ppc_state.gpr);
  p.Do(m_ppc_state.pc);
  p.Do(m_ppc_state.npc);
  p.DoArray(m_ppc_state.cr.fields);
  p.Do(m_ppc_state.msr);
  p.Do(m_ppc_state.fpscr);
  p.Do(m_ppc_state.Exceptions);
  // if (!WIIMIX_STATE) {
    p.Do(m_ppc_state.downcount);
  // }
  p.Do(m_ppc_state.xer_ca);
  p.Do(m_ppc_state.xer_so_ov);
  p.Do(m_ppc_state.xer_stringctrl);
  p.DoArray(m_ppc_state.ps);
  p.DoArray(m_ppc_state.sr);
  p.DoArray(m_ppc_state.spr);
  p.DoArray(m_ppc_state.tlb);
  p.Do(m_ppc_state.pagetable_base);
  p.Do(m_ppc_state.pagetable_hashmask);

  p.Do(m_ppc_state.reserve);
  p.Do(m_ppc_state.reserve_address);

  auto& memory = m_system.GetMemory();

  // if (!WIIMIX_STATE) {
    m_ppc_state.iCache.DoState(memory, p);
    m_ppc_state.dCache.DoState(memory, p);
    // Handle reinitialization in the State::WiiMixLoadFromBufferEmuThread function instead 
    if (p.IsReadMode())
    {
      if (!m_ppc_state.m_enable_dcache)
      {
        INFO_LOG_FMT(POWERPC, "Flushing data cache");
        m_ppc_state.dCache.FlushAll(memory);
      }

      RoundingModeUpdated(m_ppc_state);
      RecalculateAllFeatureFlags(m_ppc_state);

      auto& mmu = m_system.GetMMU();
      mmu.IBATUpdated();
      mmu.DBATUpdated();
    }
  // }

  // SystemTimers::DecrementerSet();
  // SystemTimers::TimeBaseSet();

  // Ignore the entire JIT compiler; meaningless for different architectures/wiimix savestate compatibility
  if (!WIIMIX_STATE) {
    if (Config::Get(Config::MAIN_CPU_CORE) != CPUCore::Interpreter) {
      m_system.GetJitInterface().DoState(p);
    }
  }
}
"""

if __name__ == "__main__":
    import os

    # Read in related files
    files = [
        "Source/Core/Core/State.cpp",
        "Source/Core/Core/PowerPC/PowerPC.cpp",
        "Source/Core/VideoCommon/VideoBackendBase.cpp",
        "Source/Core/Core/CoreTiming.cpp",
        "Source/Core/Core/HW/HW.cpp",
        "Source/Core/DolphinNoGUI/MainNoGUI.cpp",
        "Source/Core/Core/Core.cpp",
        "Source/Core/Core/HW/SystemTimers.cpp",
        "Source/Core/Core/HW/AudioInterface.cpp",
        "Source/Core/Core/HW/ProcessorInterface.cpp",
        "Source/Core/Core/HW/SI/SI.cpp",
        "Source/Core/Core/HW/DSP.cpp",
        "Source/Core/Core/HW/DVD/DVDInterface.cpp",
        "Source/Core/Core/HW/DVD/DVDThread.cpp",
        "Source/Core/Core/HW/VideoInterface.cpp",
        "Source/Core/Core/HW/EXI/EXI.cpp",
        "Source/Core/Core/HW/EXI/EXI_Channel.cpp",
        "Source/Core/Core/HW/Memmap.cpp",
        "Source/Core/Core/HW/HSP/HSP.cpp",
        "Source/Core/Core/HW/GPFifo.cpp",
        "Source/Core/Core/System.cpp",
        # "debug_determinism.py",
        # "scan_results.txt",
        "scan_results/Mario Kart - Double Dash!! (USA).iso_report.txt",
        "scan_results/Mario Party 6 (USA).ciso_report.txt",
        "scan_results/Mario Party 7 (USA).iso_report.txt",
        "scan_results/Nickelodeon SpongeBob SquarePants - Battle for Bikini Bottom (USA).ciso_report.txt",
        "scan_results/WarioWare, Inc. - Mega Party Game$! (USA).iso_report.txt",
    ]
    file_contents = {}
    for fname in files:
        path = os.path.join(os.path.dirname(__file__), fname)
        if os.path.exists(path):
            with open(path, "r") as f:
                file_contents[fname] = f.read()
        else:
            file_contents[fname] = "<file not found>"

    # Format the prompt
    formatted_prompt = PROMPT.format(
        # state_cpp_do_state,
        # movie_manager_do_state,
        # video_backend_do_state,
        # video_common_do_state,
        # core_timing_do_state,
        # hw_do_state,
        # powerpc_do_state,
        file_contents["Source/Core/Core/State.cpp"],
        file_contents["Source/Core/Core/PowerPC/PowerPC.cpp"],
        file_contents["Source/Core/VideoCommon/VideoBackendBase.cpp"],
        file_contents["Source/Core/Core/CoreTiming.cpp"],
        file_contents["Source/Core/Core/HW/HW.cpp"],
        file_contents["Source/Core/DolphinNoGUI/MainNoGUI.cpp"],
        file_contents["Source/Core/Core/Core.cpp"],
        file_contents["Source/Core/Core/HW/SystemTimers.cpp"],
        file_contents["Source/Core/Core/HW/AudioInterface.cpp"],
        file_contents["Source/Core/Core/HW/ProcessorInterface.cpp"],
        file_contents["Source/Core/Core/HW/SI/SI.cpp"],
        file_contents["Source/Core/Core/HW/DSP.cpp"],
        file_contents["Source/Core/Core/HW/DVD/DVDInterface.cpp"],
        file_contents["Source/Core/Core/HW/DVD/DVDThread.cpp"],
        file_contents["Source/Core/Core/HW/VideoInterface.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI_Channel.cpp"],
        file_contents["Source/Core/Core/HW/Memmap.cpp"],
        file_contents["Source/Core/Core/HW/HSP/HSP.cpp"],
        file_contents["Source/Core/Core/HW/GPFifo.cpp"],
        file_contents["Source/Core/Core/System.cpp"],
        # file_contents["debug_determinism.py"],
        # file_contents["test_output"],
        # file_contents["testoutput"],
        # file_contents["scan_results.txt"],
        # WIIMIXSTATEFUNCS,
        file_contents["scan_results/Mario Kart - Double Dash!! (USA).iso_report.txt"],
        file_contents["scan_results/Mario Party 6 (USA).ciso_report.txt"],
        file_contents["scan_results/Mario Party 7 (USA).iso_report.txt"],
        file_contents[
            "scan_results/Nickelodeon SpongeBob SquarePants - Battle for Bikini Bottom (USA).ciso_report.txt"
        ],
        file_contents[
            "scan_results/WarioWare, Inc. - Mega Party Game$! (USA).iso_report.txt"
        ],
    )

    # Write the prompt to a file prompt.txt
    with open("prompt.txt", "w") as f:
        f.write(formatted_prompt)
