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


Let's break down the DoWiiMixState function and provide a bit more context. Note that these may be slightly out of date; if the corresponding method in the file pasted later is different, make sure to refer to that instead; these are just here so you can get an idea.

First, the header is saved; this is deterministic, I've never had a problem saving this:
{}

Next the movie is saved; I haven't really had problems with this either:
{}

Then there's the video_backend. This is currently commented out, but it is something that I've had problems with, although it's difficult to determine because the problems overlap with CoreTiming as well.
{}
{}

Note the use of the WIIMIX_STATE constant; this defaults to true, so it essentially ignores saving these values for my fork of the emulator; this is my current approach to ignoring what I think are host specific values, but again, it's really difficult to tell what is host specific and what isn't.

Next is CoreTiming; as you can tell I've been logging a lot of information for this one because the timing system is hard to make deterministic. In particular, ppc_state.downcount (discussed later) and the slice_length variables have resulted in a negative global_timer before which just can't be right
{}

Next is HW; I haven't had time to test much of this since I've mostly been stuck on CoreTiming, VideoBackend and PowerPC
{}

Then there's PowerPC, the main emulated CPU state; logically I feel like this should all be emulated, but I don't know a whole lot about it so there's definitely stuff I could be missing and I'm sure I could probably benefit from logging more information
{}

Finally there's wiimote stuff, gecko codes and retroachievements; I've been largely ignoring these for now since I'm only testing Gamecube for now, I'm not planning on supporting Gecko codes and although retroachievements is supported, it's something completely different from the rest of the state.

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
====================================VideoInterface.cpp=================================
{}
====================================EXI.cpp=============================
{}
====================================memmap.cpp=================================
{}
===============================================================================
There's one more important file and it's my python testing system; it essentially loops over a bunch of simple gamecube examples, i.e.

acube.dol           consoletest.dol  gxSprites.dol  lesson03.dol  lesson06.dol  lesson09.dol  lesson12.dol     modplay.dol    readsector.dol   triangle.dol
application.dol     directory.dol    lesson01.dol   lesson04.dol  lesson07.dol  lesson10.dol  lesson19.dol     mp3player.dol  sockettest.dol
arena1override.dol  gdbstub.dol      lesson02.dol   lesson05.dol  lesson08.dol  lesson11.dol  MemCardDemo.dol  pageflip.dol   texturetest.dol

And runs WiiMixDiffTest on each one. The file looks like
======================================test_savestates.py============================
{}
===============================================================================

Here's what my current output looks like:

============================testoutput===========================================
...
--- RUNNING: texturetest.dol ---
--- STDOUT ---
Initializing HW...
Initializing DSP...
Starting WiiMix Diff Test...
Save A to buffer (at Frame 0)
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
Step Frame 1 (for State B)
Save B to buffer (at Frame 1)
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 15436, global_timer = 386108
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 15436, global_timer = 386108
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 15436, global_timer = 386108
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 15436, global_timer = 386108
Host reinit
TIMING_LOG (Before Host Reinit): slice_length = 15436, global_timer = 386108
TIMING_LOG (After Host Reinit): slice_length = 20000, global_timer = 0
Load from EmuThread (to Frame 0)
Current frame in EmuThread before DoWiiMixState: 1
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
Current frame at end of EmuThread: 0
TIMING_LOG (After WiiMixLoadFromBufferEmuThread for Diff Test): slice_length = 20000, global_timer = 0
Step Frame 1 (for State C)
Save C to Buffer (at Frame 1)
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 15436, global_timer = 386108
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 15436, global_timer = 386108
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 15436, global_timer = 386108
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 15436, global_timer = 386108
Diff Test PASSED! State is deterministic.
Dumping B and C to files
Stopping System
s_platform.reset() in main
IniFile::Save: /home/xanmankey/.config/dolphin-emu/Dolphin.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Dolphin.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCPadNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCPadNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GFX.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GFX.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/Logger.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Logger.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/DSUClient.ini.xxx -> /home/xanmankey/.config/dolphin-emu/DSUClient.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/FreeLook.ini.xxx -> /home/xanmankey/.config/dolphin-emu/FreeLook.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini.xxx -> /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini

--- RESULT: PASS ---

--- RUNNING: triangle.dol ---
--- STDOUT ---
Initializing HW...
Initializing DSP...
Starting WiiMix Diff Test...
Save A to buffer (at Frame 0)
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
Step Frame 1 (for State B)
Save B to buffer (at Frame 1)
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 15439, global_timer = 386105
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 15439, global_timer = 386105
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 15439, global_timer = 386105
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 15439, global_timer = 386105
Host reinit
TIMING_LOG (Before Host Reinit): slice_length = 15439, global_timer = 386105
TIMING_LOG (After Host Reinit): slice_length = 20000, global_timer = 0
Load from EmuThread (to Frame 0)
Current frame in EmuThread before DoWiiMixState: 1
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
Current frame at end of EmuThread: 0
TIMING_LOG (After WiiMixLoadFromBufferEmuThread for Diff Test): slice_length = 20000, global_timer = 0
Step Frame 1 (for State C)
Save C to Buffer (at Frame 1)
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 15439, global_timer = 386105
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 15439, global_timer = 386105
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 15439, global_timer = 386105
TIMING_LOG (Event Queue): event_count = 7
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 15439, global_timer = 386105
Diff Test PASSED! State is deterministic.
Dumping B and C to files
Stopping System
s_platform.reset() in main
IniFile::Save: /home/xanmankey/.config/dolphin-emu/Dolphin.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Dolphin.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCPadNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCPadNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GFX.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GFX.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/Logger.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Logger.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/DSUClient.ini.xxx -> /home/xanmankey/.config/dolphin-emu/DSUClient.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/FreeLook.ini.xxx -> /home/xanmankey/.config/dolphin-emu/FreeLook.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini.xxx -> /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini

--- RESULT: PASS ---
100%|███████████████████████████████████████████████████████████████████████████████████████████████████████████████████████| 28/28

TEST RUN SUMMARY
All 28 tests passed!
======================================================================

Note that I do currently pass all of the SIMPLE examples as seen in the test output above.

I can provide more context and references as needed, but those are the primary files that I'm working with right now. I told you the end goal, but right now my focus is on:

- Understanding: how do these systems work together and what is each system responsible for?

- Effective debugging: how can we effectively identify host specific components as opposed to pure ones? If there's an error, how can we effectively log to find out the root cause

This is a complex multithreaded program, so that's why those are my 2 main focuses; I don't want to end up going in loops changing things and then having to revert. I want to constantly be making some kind of forward progress. With that in mind, if I start a message with STOP: WE ARE GOING IN A CYCLE then I want you to stop getting stuck and repeating the train of thought that you've been repeating and try to consider something new.

With all of that in mind, use your c++ knowledge and knowledge of Dolphin Emulator to help me out. Note that I won't be providing full files as context again unless appropriate, so MAKE SURE TO REFERENCE THESE FILES, and always assume that if you suggest a chance I have made it if I provide you new output unless I specify otherwise.

My current problem is that although I pass all the simple examples for my simple test, I don't pass more complex examples like F-zero Gx. Specifically, I've been stuck on a CoreTiming related issue; the global_timer always seems to be off after loading for F-Zero GX. I've been struggling to figure out the cause of this issue as well as how to resolve it. Note that I've tried the following:
- Ignoring slice_length and downcount when saving/loading states and then recalculating
- Keeping slice_length and downcount in the savestate and commenting out recalculation

BUT NEITHER OF THESE APPROACHES SEEM TO WORK SO I THINK IT'S SOMETHING ELSE.

Here is my current output:

{}
"""

OUTPUT = """
Initializing HW...
Initializing DSP...
Starting WiiMix Diff Test...
Save A to buffer (at Frame 0)
WII-MIX-TEST-LOG: Save A START (Current Frame: 1)
WII-MIX-MAP: Offset after Movie: 0x36
WII-MIX-MAP: Offset after video_backend: 0x30487D
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 1953666, Fifo Order: 287, Userdata: 0
    Event: GPUSleeper, Time: 2430000, Fifo Order: 285, Userdata: 0
    Event: AudioDMACallback, Time: 2063664, Fifo Order: 284, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: PatchEngine, Time: 4054050, Fifo Order: 6, Userdata: 0
    Event: FinishExecutingCommand, Time: 3398976, Fifo Order: 250, Userdata: 12884901889
    Event: DSPCallback, Time: 2430000, Fifo Order: 286, Userdata: 0
WII-MIX-MAP: Offset after CoreTiming: 0x3049DC
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
WII-MIX-MAP: Offset after HW: 0x3345103
WII-MIX-MAP: Offset after PowerPC: 0x335992C
WII-MIX-MAP: Offset after AchievementManager: 0x3359930
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 1953666, Fifo Order: 287, Userdata: 0
    Event: GPUSleeper, Time: 2430000, Fifo Order: 285, Userdata: 0
    Event: AudioDMACallback, Time: 2063664, Fifo Order: 284, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: PatchEngine, Time: 4054050, Fifo Order: 6, Userdata: 0
    Event: FinishExecutingCommand, Time: 3398976, Fifo Order: 250, Userdata: 12884901889
    Event: DSPCallback, Time: 2430000, Fifo Order: 286, Userdata: 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
WII-MIX-TEST-LOG: Save A END (Current Frame: 1)
Step Frame 1 (for State B)
Save B to buffer (at Frame 1)
WII-MIX-TEST-LOG: Save B START (Current Frame: 2)
WII-MIX-MAP: Offset after Movie: 0x36
WII-MIX-MAP: Offset after video_backend: 0x30487D
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7713, global_timer = 4092669
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 4100382, Fifo Order: 594, Userdata: 0
    Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
    Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
    Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
    Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
WII-MIX-MAP: Offset after CoreTiming: 0x3049DC
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7713, global_timer = 4092669
WII-MIX-MAP: Offset after HW: 0x3345103
WII-MIX-MAP: Offset after PowerPC: 0x335992C
WII-MIX-MAP: Offset after AchievementManager: 0x3359930
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7713, global_timer = 4092669
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 4100382, Fifo Order: 594, Userdata: 0
    Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
    Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
    Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
    Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7713, global_timer = 4092669
WII-MIX-TEST-LOG: Save B END (Current Frame: 2)
Load from EmuThread (to Frame 0)
WII-MIX-TEST-LOG: Load A START (Current Frame: 2)
TIMING_LOG (Before Host Reinit): slice_length = 7713, global_timer = 4092669
TIMING_LOG (After Host Reinit): slice_length = 20000, global_timer = 0
Current frame in EmuThread before DoWiiMixState: 2
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
    Event: VICallback, Time: 1953666, Fifo Order: 287, Userdata: 0
    Event: AudioDMACallback, Time: 2063664, Fifo Order: 284, Userdata: 0
    Event: GPUSleeper, Time: 2430000, Fifo Order: 285, Userdata: 0
    Event: DSPCallback, Time: 2430000, Fifo Order: 286, Userdata: 0
    Event: FinishExecutingCommand, Time: 3398976, Fifo Order: 250, Userdata: 12884901889
    Event: PatchEngine, Time: 4054050, Fifo Order: 6, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 1946320
Current frame at end of EmuThread: 1
TIMING_LOG (After WiiMixLoadFromBufferEmuThread for Diff Test): slice_length = 7346, global_timer = 1946320
WII-MIX-TEST-LOG: Load A END (Current Frame: 1)
Step Frame 1 (for State C)
Save C to Buffer (at Frame 1)
WII-MIX-TEST-LOG: Save C START (Current Frame: 2)
WII-MIX-MAP: Offset after Movie: 0x36
WII-MIX-MAP: Offset after video_backend: 0x30487D
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7721, global_timer = 4092661
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 4100382, Fifo Order: 594, Userdata: 0
    Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
    Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
    Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
    Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
WII-MIX-MAP: Offset after CoreTiming: 0x3049DC
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7721, global_timer = 4092661
WII-MIX-MAP: Offset after HW: 0x3345103
WII-MIX-MAP: Offset after PowerPC: 0x335992C
WII-MIX-MAP: Offset after AchievementManager: 0x3359930
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7721, global_timer = 4092661
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 4100382, Fifo Order: 594, Userdata: 0
    Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
    Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
    Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
    Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7721, global_timer = 4092661
WII-MIX-TEST-LOG: Save C END (Current Frame: 2)
Diff Test FAILED at /home/xanmankey/Programming/OpenSource/WiiMix/Source/Core/Core/State.cpp:2191: Diff Test FAILED: Mismatch at byte 0x30487D Your save/load logic is not deterministic.
Dumping B and C to files
Stopping System
s_platform.reset() in main
IniFile::Save: /home/xanmankey/.config/dolphin-emu/Dolphin.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Dolphin.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCPadNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCPadNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GFX.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GFX.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/Logger.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Logger.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/DSUClient.ini.xxx -> /home/xanmankey/.config/dolphin-emu/DSUClient.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/FreeLook.ini.xxx -> /home/xanmankey/.config/dolphin-emu/FreeLook.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini.xxx -> /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini
"""

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

OUTPUT = """
./Binaries/wiimix-nogui --diff-test --interpreter -e ~/dolphingames/F-Zero\ GX\ \(USA\).ciso
Initializing HW...
Initializing DSP...
Starting WiiMix Diff Test...
Save A to buffer (at Frame 0)
WII-MIX-MAP: Offset after Movie: 0x36
WII-MIX-MAP: Offset after video_backend: 0x30487D
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 1953666, Fifo Order: 287, Userdata: 0
    Event: GPUSleeper, Time: 2430000, Fifo Order: 285, Userdata: 0
    Event: AudioDMACallback, Time: 2063664, Fifo Order: 284, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: PatchEngine, Time: 4054050, Fifo Order: 6, Userdata: 0
    Event: FinishExecutingCommand, Time: 3398976, Fifo Order: 250, Userdata: 12884901889
    Event: DSPCallback, Time: 2430000, Fifo Order: 286, Userdata: 0
WII-MIX-MAP: Offset after CoreTiming: 0x3049E0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
WII-MIX-MAP: Offset after HW: 0x3345107
WII-MIX-MAP: Offset after PowerPC: 0x3359934
WII-MIX-MAP: Offset after AchievementManager: 0x3359938
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 1953666, Fifo Order: 287, Userdata: 0
    Event: GPUSleeper, Time: 2430000, Fifo Order: 285, Userdata: 0
    Event: AudioDMACallback, Time: 2063664, Fifo Order: 284, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: PatchEngine, Time: 4054050, Fifo Order: 6, Userdata: 0
    Event: FinishExecutingCommand, Time: 3398976, Fifo Order: 250, Userdata: 12884901889
    Event: DSPCallback, Time: 2430000, Fifo Order: 286, Userdata: 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
Step Frame 1 (for State B)
Save B to buffer (at Frame 1)
WII-MIX-MAP: Offset after Movie: 0x36
WII-MIX-MAP: Offset after video_backend: 0x30487D
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7713, global_timer = 4092669
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 4100382, Fifo Order: 594, Userdata: 0
    Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
    Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
    Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
    Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
WII-MIX-MAP: Offset after CoreTiming: 0x3049E0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7713, global_timer = 4092669
WII-MIX-MAP: Offset after HW: 0x3345107
WII-MIX-MAP: Offset after PowerPC: 0x3359934
WII-MIX-MAP: Offset after AchievementManager: 0x3359938
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7713, global_timer = 4092669
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 4100382, Fifo Order: 594, Userdata: 0
    Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
    Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
    Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
    Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7713, global_timer = 4092669
Load from EmuThread (to Frame 0)
TIMING_LOG (Before Host Reinit): slice_length = 20000, global_timer = 0
TIMING_LOG (After Host Reinit): slice_length = 20000, global_timer = 0
Current frame in EmuThread before DoWiiMixState: 2
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 20000, global_timer = 0
    Event: VICallback, Time: 1953666, Fifo Order: 287, Userdata: 0
    Event: AudioDMACallback, Time: 2063664, Fifo Order: 284, Userdata: 0
    Event: GPUSleeper, Time: 2430000, Fifo Order: 285, Userdata: 0
    Event: DSPCallback, Time: 2430000, Fifo Order: 286, Userdata: 0
    Event: FinishExecutingCommand, Time: 3398976, Fifo Order: 250, Userdata: 12884901889
    Event: PatchEngine, Time: 4054050, Fifo Order: 6, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 1, global_timer = 1946320
Current frame at end of EmuThread: 1
TIMING_LOG (After WiiMixLoadFromBufferEmuThread for Diff Test): slice_length = 1, global_timer = 1946320
Step Frame 1 (for State C)
Save C to Buffer (at Frame 1)
WII-MIX-MAP: Offset after Movie: 0x36
WII-MIX-MAP: Offset after video_backend: 0x30487D
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7719, global_timer = 4084941
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 4092660, Fifo Order: 593, Userdata: 0
    Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
    Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
    Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
    Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
WII-MIX-MAP: Offset after CoreTiming: 0x3049E0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7719, global_timer = 4084941
WII-MIX-MAP: Offset after HW: 0x3345107
WII-MIX-MAP: Offset after PowerPC: 0x3359934
WII-MIX-MAP: Offset after AchievementManager: 0x3359938
TIMING_LOG (Before DoWiiMixState CoreTiming): slice_length = 7719, global_timer = 4084941
TIMING_LOG (Event Queue): event_count = 7
    Event: VICallback, Time: 4092660, Fifo Order: 593, Userdata: 0
    Event: DSPCallback, Time: 4374000, Fifo Order: 565, Userdata: 0
    Event: AudioDMACallback, Time: 4127328, Fifo Order: 581, Userdata: 0
    Event: PerfTracker, Time: 4860000, Fifo Order: 8, Userdata: 0
    Event: FinishExecutingCommand, Time: 5098464, Fifo Order: 493, Userdata: 12884901889
    Event: PatchEngine, Time: 8108100, Fifo Order: 588, Userdata: 0
    Event: GPUSleeper, Time: 4374000, Fifo Order: 564, Userdata: 0
TIMING_LOG (After DoWiiMixState CoreTiming): slice_length = 7719, global_timer = 4084941
Diff Test FAILED at /home/xanmankey/Programming/OpenSource/WiiMix/Source/Core/Core/State.cpp:2158: Diff Test FAILED: Mismatch at byte 0x30487D Your save/load logic is not deterministic.
Dumping B and C to files
Stopping System
s_platform.reset() in main
IniFile::Save: /home/xanmankey/.config/dolphin-emu/Dolphin.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Dolphin.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCPadNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCPadNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/WiimoteNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GCKeyNew.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/GFX.ini.xxx -> /home/xanmankey/.config/dolphin-emu/GFX.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/Logger.ini.xxx -> /home/xanmankey/.config/dolphin-emu/Logger.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/DSUClient.ini.xxx -> /home/xanmankey/.config/dolphin-emu/DSUClient.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/FreeLook.ini.xxx -> /home/xanmankey/.config/dolphin-emu/FreeLook.ini
IniFile::Save: /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini.xxx -> /home/xanmankey/.config/dolphin-emu/RetroAchievements.ini
"""

if __name__ == "__main__":
    import os

    # Read in related files
    files = [
        "Source/Core/Core/State.cpp",
        "Source/Core/Core/PowerPC.cpp",
        "Source/Core/Core/VideoBackendBase.cpp",
        "Source/Core/Core/CoreTiming.cpp",
        "Source/Core/Core/HW.cpp",
        "Source/Core/Core/MainNoGui.cpp",
        "Source/Core/Core/Core.cpp",
        "Source/Core/Core/SystemTimers.cpp",
        "Source/Core/Core/AudioInterface.cpp",
        "Source/Core/Core/ProcessorInterface.cpp",
        "Source/Core/Core/SI.cpp",
        "Source/Core/Core/DSP.cpp",
        "Source/Core/Core/DVDInterface.cpp",
        "Source/Core/Core/VideoInterface.cpp",
        "Source/Core/Core/EXI.cpp",
        "Source/Core/Core/memmap.cpp",
        "test_savestates.py",
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
        state_cpp_do_state,
        movie_manager_do_state,
        video_backend_do_state,
        video_common_do_state,
        core_timing_do_state,
        hw_do_state,
        powerpc_do_state,
        file_contents["Source/Core/Core/State.cpp"],
        file_contents["Source/Core/Core/PowerPC.cpp"],
        file_contents["Source/Core/Core/VideoBackendBase.cpp"],
        file_contents["Source/Core/Core/CoreTiming.cpp"],
        file_contents["Source/Core/Core/HW.cpp"],
        file_contents["Source/Core/Core/MainNoGui.cpp"],
        file_contents["Source/Core/Core/Core.cpp"],
        file_contents["Source/Core/Core/SystemTimers.cpp"],
        file_contents["Source/Core/Core/AudioInterface.cpp"],
        file_contents["Source/Core/Core/ProcessorInterface.cpp"],
        file_contents["Source/Core/Core/SI.cpp"],
        file_contents["Source/Core/Core/DSP.cpp"],
        file_contents["Source/Core/Core/DVDInterface.cpp"],
        file_contents["Source/Core/Core/VideoInterface.cpp"],
        file_contents["Source/Core/Core/EXI.cpp"],
        file_contents["Source/Core/Core/memmap.cpp"],
        file_contents["test_savestates.py"],
        OUTPUT,
    )

    # Write the prompt to a file prompt.txt
    with open("prompt.txt", "w") as f:
        f.write(formatted_prompt)
