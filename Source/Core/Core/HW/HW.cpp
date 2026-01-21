// Copyright 2008 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Core/HW/HW.h"

#include "Common/ChunkFile.h"
#include "Common/CommonTypes.h"

#include "Core/Config/MainSettings.h"
#include "Core/Core.h"
#include "Core/CoreTiming.h"
#include "Core/HW/AddressSpace.h"
#include "Core/HW/AudioInterface.h"
#include "Core/HW/CPU.h"
#include "Core/HW/DSP.h"
#include "Core/HW/DVD/DVDInterface.h"
#include "Core/HW/EXI/EXI.h"
#include "Core/HW/GPFifo.h"
#include "Core/HW/HSP/HSP.h"
#include "Core/HW/Memmap.h"
#include "Core/HW/MemoryInterface.h"
#include "Core/HW/ProcessorInterface.h"
#include "Core/HW/SI/SI.h"
#include "Core/HW/SystemTimers.h"
#include "Core/HW/VideoInterface.h"
#include "Core/HW/WII_IPC.h"
#include "Core/IOS/IOS.h"
#include "Core/State.h"
#include "Core/System.h"

namespace HW
{
void Init(Core::System& system, const Sram* override_sram)
{
  printf("Initializing HW...\n");
  system.GetCoreTiming().Init();
  system.GetSystemTimers().PreInit();

  State::Init(system);

  // Init the whole Hardware
  system.GetAudioInterface().Init();
  system.GetVideoInterface().Init();
  system.GetSerialInterface().Init();
  system.GetProcessorInterface().Init();
  system.GetExpansionInterface().Init(override_sram);  // Needs to be initialized before Memory
  system.GetHSP().Init();
  system.GetMemory().Init();  // Needs to be initialized before AddressSpace
  AddressSpace::Init();
  system.GetMemoryInterface().Init();
  system.GetDSP().Init(Config::Get(Config::MAIN_DSP_HLE));
  system.GetDVDInterface().Init();
  system.GetGPFifo().Init();
  system.GetCPU().Init(Config::Get(Config::MAIN_CPU_CORE));
  system.GetSystemTimers().Init();

  system.GetSystemTimers().ScheduleInitialEvents();

  if (system.IsWii())
  {
    system.GetWiiIPC().Init();
    IOS::HLE::Init(system);  // Depends on Memory
  }
}

// void Init(Core::System& system, const Sram* override_sram)
// {
//   printf("Initializing WiiMix HW...\n");
  
//   // --- 1. "COLD" INIT ---
//   system.GetCoreTiming().Init();
//   system.GetSystemTimers().PreInit();
//   State::Init(system);
//   system.GetVideoInterface().Init();
//   system.GetSerialInterface().Init();
//   system.GetProcessorInterface().Init();
//   system.GetExpansionInterface().Init(override_sram);
//   system.GetHSP().Init();
//   system.GetMemory().Init();
//   AddressSpace::Init();
//   system.GetMemoryInterface().Init();
//   system.GetGPFifo().Init();
//   g_video_backend->Init();
//   system.GetCPU().Init(Config::Get(Config::MAIN_CPU_CORE)); // Init CPU before threads
  
//   // --- 2. "HOT" INIT (Deterministic Order) ---
//   // We call the full Init functions *serially* to force
//   // a deterministic registration and scheduling order.
  
//   system.GetAudioInterface().Init();
//   AudioCommon::PostInitSoundStream(system); // Start stream

//   system.GetDVDInterface().Init(); // Starts thread + schedules event

//   system.GetDSP().Init(Config::Get<bool>(Config::MAIN_DSP_HLE)); // Starts thread

//   system.GetSystemTimers().Init(); // Now "cold"
//   system.GetSystemTimers().ScheduleInitialEvents(); // Manually schedule
  
//   if (system.IsWii())
//   {
//     system.GetWiiIPC().Init();
//     IOS::HLE::Init(system);
//   }
// }

void Shutdown(Core::System& system)
{
  // IOS should always be shut down regardless of IsWii because it can be running in GC mode (MIOS).
  IOS::HLE::Shutdown(system);  // Depends on Memory
  system.GetWiiIPC().Shutdown();

  system.GetSystemTimers().Shutdown();
  system.GetCPU().Shutdown();
  system.GetDVDInterface().Shutdown();
  system.GetDSP().Shutdown();
  system.GetMemoryInterface().Shutdown();
  AddressSpace::Shutdown();
  system.GetMemory().Shutdown();
  system.GetHSP().Shutdown();
  system.GetExpansionInterface().Shutdown();
  system.GetSerialInterface().Shutdown();
  system.GetAudioInterface().Shutdown();

  State::Shutdown();
  system.GetCoreTiming().Shutdown();
}

// void DoState(Core::System& system, PointerWrap& p)
// {
//   // ONLY save memory for testing for now
//   system.GetMemory().DoState(p);
//   p.DoMarker("Memory");
//   system.GetMemoryInterface().DoState(p);
//   p.DoMarker("MemoryInterface");
//   system.GetVideoInterface().DoState(p);
//   p.DoMarker("VideoInterface");
//   // SerialInterface was crashing due to a null ptr; skipping it for now
//   // I'm not sure if it contains data necessary for state saving yet
//   if (!WIIMIX_STATE) {
//     system.GetSerialInterface().DoState(p);
//     p.DoMarker("SerialInterface");
//   }
//   system.GetProcessorInterface().DoState(p);
//   p.DoMarker("ProcessorInterface");
//   // TODO: left off here; DSP nullptr crash on acube.dol
//   system.GetDSP().DoState(p);
//   p.DoMarker("DSP");
//   if (!WIIMIX_STATE) {
//     system.GetDVDInterface().DoState(p);
//     p.DoMarker("DVDInterface");
//     system.GetGPFifo().DoState(p);
//     p.DoMarker("GPFifo");
//     system.GetExpansionInterface().DoState(p);
//     p.DoMarker("ExpansionInterface");
//     if (!WIIMIX_STATE) {
//       system.GetAudioInterface().DoState(p);
//       p.DoMarker("AudioInterface");
//     }
//     system.GetHSP().DoState(p);
//     p.DoMarker("HSP");

//     if (system.IsWii())
//     {
//       system.GetWiiIPC().DoState(p);
//       p.DoMarker("IOS");
//       system.GetIOS()->DoState(p);
//       p.DoMarker("IOS::HLE");
//     }

//     p.DoMarker("WIIHW");
//   }
// }

void DoState(Core::System& system, PointerWrap& p)
{
  // NOTE: all of these are emulated components, however the states being saved
  // MAY contain host-specific data

  system.GetMemory().DoState(p);
  p.DoMarker("Memory");
  State::LogOffset("Memory", p);

  system.GetSystemTimers().DoState(p);
  p.DoMarker("SystemTimers");
  State::LogOffset("SystemTimers", p);

  system.GetMemoryInterface().DoState(p);
  p.DoMarker("MemoryInterface");
  State::LogOffset("MemoryInterface", p);
  system.GetVideoInterface().DoState(p);
  p.DoMarker("VideoInterface");
  State::LogOffset("VideoInterface", p);
  system.GetSerialInterface().DoState(p);
  p.DoMarker("SerialInterface");
  State::LogOffset("SerialInterface", p);
  system.GetProcessorInterface().DoState(p);
  p.DoMarker("ProcessorInterface");
  State::LogOffset("ProcessorInterface", p);
  system.GetDSP().DoState(p);
  p.DoMarker("DSP");
  State::LogOffset("DSP", p);
  system.GetDVDInterface().DoState(p);
  p.DoMarker("DVDInterface");
  State::LogOffset("DVDInterface", p);
  system.GetGPFifo().DoState(p);
  p.DoMarker("GPFifo");
  State::LogOffset("GPFifo", p);
  system.GetExpansionInterface().DoState(p);
  p.DoMarker("ExpansionInterface");
  State::LogOffset("ExpansionInterface", p);
  system.GetAudioInterface().DoState(p);
  p.DoMarker("AudioInterface");
  State::LogOffset("AudioInterface", p);
  system.GetHSP().DoState(p);
  p.DoMarker("HSP");
  State::LogOffset("HSP", p);

  if (system.IsWii())
  {
    system.GetWiiIPC().DoState(p);
    p.DoMarker("IOS");
    system.GetIOS()->DoState(p);
    p.DoMarker("IOS::HLE");
  }

  p.DoMarker("WIIHW");
}

}  // namespace HW
