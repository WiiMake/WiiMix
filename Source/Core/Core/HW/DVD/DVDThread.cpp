// Copyright 2015 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "Core/HW/DVD/DVDThread.h"

#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <utility>
#include <vector>

#include "Common/ChunkFile.h"
#include "Common/CommonTypes.h"
#include "Common/Event.h"
#include "Common/Flag.h"
#include "Common/Logging/Log.h"
#include "Common/MsgHandler.h"
#include "Common/SPSCQueue.h"
#include "Common/Thread.h"
#include "Common/Timer.h"

#include "Core/ConfigManager.h"
#include "Core/Core.h"
#include "Core/CoreTiming.h"
#include "Core/HW/DVD/DVDInterface.h"
#include "Core/HW/DVD/FileMonitor.h"
#include "Core/HW/Memmap.h"
#include "Core/HW/SystemTimers.h"
#include "Core/IOS/ES/Formats.h"
#include "Core/System.h"

#include "DiscIO/Enums.h"
#include "DiscIO/Volume.h"

namespace DVD
{
DVDThread::DVDThread(Core::System& system) : m_system(system)
{
}

DVDThread::~DVDThread() = default;

void DVDThread::Start()
{
  if (WIIMIX_STATE) {
    // FIX: Robustness check.
    // If the thread is already joinable (e.g. restarted by WaitUntilIdle during Load),
    // we must not try to start it again, or the assertion in StartDVDThread will fire.
    // Just verify the flag is set and return.
    if (m_dvd_thread.joinable()) {
      m_is_running = true;
      return;
    }
    
    // if (m_is_running)
    //   return;
  }
  m_finish_read = m_system.GetCoreTiming().RegisterEvent("FinishReadDVDThread", GlobalFinishRead);

  m_request_queue_expanded.Reset();
  m_result_queue_expanded.Reset();
  m_request_queue.Clear();
  m_result_queue.Clear();

  // This is reset on every launch for determinism, but it doesn't matter
  // much, because this will never get exposed to the emulated game.
  if (!WIIMIX_STATE) {
    m_next_id = 0;
  }
  m_is_running = true;
  StartDVDThread();
}

void DVDThread::StartDVDThread()
{
  ASSERT(!m_dvd_thread.joinable());
  m_dvd_thread_exiting.Clear();
  m_dvd_thread = std::thread(&DVDThread::DVDThreadMain, this);
}

void DVDThread::Stop()
{
  if (WIIMIX_STATE) {
    // FIX: Robustness check.
    // If the thread is joinable, we MUST join it, even if m_is_running is false.
    // This cleans up "ghost" threads left by WaitUntilIdle.
    if (!m_is_running && !m_dvd_thread.joinable())
      return;
  }
  StopDVDThread();
  
  // FIX: Do not reset the disc in WiiMix mode. 
  // Disc presence is host configuration and must persist across re-initialization.
  if (!WIIMIX_STATE) {
    m_disc.reset();
  }
  m_is_running = false;
}

void DVDThread::WiiMixReset() {
  // We're already on the CPU thread, and WaitUntilIdle
  // has been called, so the DVD thread is stopped.
  // It is now safe to clear all "dirty" state.
  m_result_map.clear();

  // Drain and clear the "dirty" result queue
  ReadResult result;
  while (m_result_queue.Pop(result))
  {
    // do nothing, just drain it
  }
}

void DVDThread::StopDVDThread()
{
  if (WIIMIX_STATE) {
    // FIX: Allow stopping if the thread object is joinable, regardless of flag.
    if (!m_dvd_thread.joinable())
      return;
  }
  else {
    ASSERT(m_dvd_thread.joinable());
  }

  // By setting dvd_thread_exiting, we ask the DVD thread to cleanly exit.
  // In case the request queue is empty, we need to set request_queue_expanded
  // so that the DVD thread will wake up and check dvd_thread_exiting.
  m_dvd_thread_exiting.Set();
  m_request_queue_expanded.Set();

  m_dvd_thread.join();
}

void DVDThread::DoState(PointerWrap& p)
{
  // m_disc isn't savestated (because it points to files on the
  // local system). Instead, we check that the status of the disc
  // is the same as when the savestate was made. This won't catch
  // cases of having the wrong disc inserted, though.
  // TODO: Check the game ID, disc number, revision?
  bool had_disc = HasDisc();
  p.Do(had_disc);

  // By waiting for the DVD thread to be done working, we ensure
  // that request_queue will be empty and that the DVD thread
  // won't be touching anything while this function runs.
  
  // FIX: Removing (!WIIMIX_STATE) check.
  // m_next_id and m_result_map are PURE EMULATED STATE.
  // Failing to save m_next_id causes read IDs to reset to 0 on load, causing desyncs.
  
  // if (!WIIMIX_STATE) {
    WaitUntilIdle();

    // Don't savestate requests reliant on host

    // Move all results from result_queue to result_map because
    // PointerWrap::Do supports std::map but not Common::SPSCQueue.
    // This won't affect the behavior of FinishRead.
    ReadResult result;
    while (m_result_queue.Pop(result))
      m_result_map.emplace(result.first.id, std::move(result));
  
    // Both queues are now empty, so we don't need to savestate them.
    p.Do(m_result_map);
    p.Do(m_next_id);
  // }

  if (had_disc != HasDisc())
  {
    if (had_disc)
      PanicAlertFmtT("An inserted disc was expected but not found.");
    else
      m_disc.reset();
  }
}

void DVDThread::DoWiiMixState(PointerWrap& p)
{
  // By waiting for the DVD thread to be done working, we ensure
  // that request_queue will be empty and that the DVD thread
  // won't be touching anything while this function runs.
  WaitUntilIdle();

  // Move all results from result_queue to result_map because
  // PointerWrap::Do supports std::map but not Common::SPSCQueue.
  // This won't affect the behavior of FinishRead.
  ReadResult result;
  while (m_result_queue.Pop(result))
    m_result_map.emplace(result.first.id, std::move(result));

  // Both queues are now empty, so we don't need to savestate them.
  p.Do(m_result_map);
  p.Do(m_next_id);

  // m_disc isn't savestated (because it points to files on the
  // local system). Instead, we check that the status of the disc
  // is the same as when the savestate was made. This won't catch
  // cases of having the wrong disc inserted, though.
  // TODO: Check the game ID, disc number, revision?
  bool had_disc = HasDisc();
  p.Do(had_disc);
  if (had_disc != HasDisc())
  {
    if (had_disc)
      PanicAlertFmtT("An inserted disc was expected but not found.");
    else
      m_disc.reset();
  }
}

void DVDThread::SetDisc(std::unique_ptr<DiscIO::Volume> disc)
{
  WaitUntilIdle();
  m_disc = std::move(disc);
}

bool DVDThread::HasDisc() const
{
  return m_disc != nullptr;
}

bool DVDThread::HasWiiHashes() const
{
  // HasWiiHashes is thread-safe, so calling WaitUntilIdle isn't necessary.
  return m_disc->HasWiiHashes();
}

DiscIO::Platform DVDThread::GetDiscType() const
{
  // GetVolumeType is thread-safe, so calling WaitUntilIdle isn't necessary.
  return m_disc->GetVolumeType();
}

u64 DVDThread::PartitionOffsetToRawOffset(u64 offset, const DiscIO::Partition& partition)
{
  // PartitionOffsetToRawOffset is thread-safe, so calling WaitUntilIdle isn't necessary.
  return m_disc->PartitionOffsetToRawOffset(offset, partition);
}

IOS::ES::TMDReader DVDThread::GetTMD(const DiscIO::Partition& partition)
{
  WaitUntilIdle();
  return m_disc->GetTMD(partition);
}

IOS::ES::TicketReader DVDThread::GetTicket(const DiscIO::Partition& partition)
{
  WaitUntilIdle();
  return m_disc->GetTicket(partition);
}

bool DVDThread::IsInsertedDiscRunning()
{
  if (!m_disc)
    return false;

  WaitUntilIdle();

  return SConfig::GetInstance().GetGameID() == m_disc->GetGameID();
}

bool DVDThread::UpdateRunningGameMetadata(const DiscIO::Partition& partition,
                                          std::optional<u64> title_id)
{
  if (!m_disc)
    return false;

  WaitUntilIdle();

  if (title_id)
  {
    const std::optional<u64> volume_title_id = m_disc->GetTitleID(partition);
    if (!volume_title_id || *volume_title_id != *title_id)
      return false;
  }

  SConfig::GetInstance().SetRunningGameMetadata(*m_disc, partition);
  return true;
}

void DVDThread::WaitUntilIdle()
{
  if (!WIIMIX_STATE)
    ASSERT(Core::IsCPUThread());

  while (!m_request_queue.Empty())
    m_result_queue_expanded.Wait();

  StopDVDThread();
  StartDVDThread();
}

void DVDThread::StartRead(u64 dvd_offset, u32 length, const DiscIO::Partition& partition,
                          DVD::ReplyType reply_type, s64 ticks_until_completion)
{
  StartReadInternal(false, 0, dvd_offset, length, partition, reply_type, ticks_until_completion);
}

void DVDThread::StartReadToEmulatedRAM(u32 output_address, u64 dvd_offset, u32 length,
                                       const DiscIO::Partition& partition,
                                       DVD::ReplyType reply_type, s64 ticks_until_completion)
{
  StartReadInternal(true, output_address, dvd_offset, length, partition, reply_type,
                    ticks_until_completion);
}

void DVDThread::StartReadInternal(bool copy_to_ram, u32 output_address, u64 dvd_offset, u32 length,
                                  const DiscIO::Partition& partition, DVD::ReplyType reply_type,
                                  s64 ticks_until_completion)
{
  ASSERT(Core::IsCPUThread());

  auto& core_timing = m_system.GetCoreTiming();

  ReadRequest request;

  request.copy_to_ram = copy_to_ram;
  request.output_address = output_address;
  request.dvd_offset = dvd_offset;
  request.length = length;
  request.partition = partition;
  request.reply_type = reply_type;

  u64 id = m_next_id++;
  request.id = id;

  request.time_started_ticks = core_timing.GetTicks();
  request.realtime_started_us = Common::Timer::NowUs();

  m_request_queue.Push(std::move(request));
  m_request_queue_expanded.Set();

  core_timing.ScheduleEvent(ticks_until_completion, m_finish_read, id);
}

void DVDThread::GlobalFinishRead(Core::System& system, u64 id, s64 cycles_late)
{
  system.GetDVDThread().FinishRead(id, cycles_late);
}

void DVDThread::FinishRead(u64 id, s64 cycles_late)
{
  ReadResult result;
  auto it = m_result_map.find(id);
  if (it != m_result_map.end())
  {
    result = std::move(it->second);
    m_result_map.erase(it);
  }
  else
  {
    while (true)
    {
      while (!m_result_queue.Pop(result))
        m_result_queue_expanded.Wait();

      if (result.first.id == id)
        break;
      else
        m_result_map.emplace(result.first.id, std::move(result));
    }
  }

  const ReadRequest& request = result.first;
  const std::vector<u8>& buffer = result.second;

  DEBUG_LOG_FMT(DVDINTERFACE,
                "Disc has been read. Real time: {} us. "
                "Real time including delay: {} us. "
                "Emulated time including delay: {} us.",
                request.realtime_done_us - request.realtime_started_us,
                Common::Timer::NowUs() - request.realtime_started_us,
                (m_system.GetCoreTiming().GetTicks() - request.time_started_ticks) /
                    (m_system.GetSystemTimers().GetTicksPerSecond() / 1000000));

  auto& dvd_interface = m_system.GetDVDInterface();
  DVD::DIInterruptType interrupt;
  if (buffer.size() != request.length)
  {
    PanicAlertFmtT("The disc could not be read (at {0:#x} - {1:#x}).", request.dvd_offset,
                   request.dvd_offset + request.length);

    dvd_interface.SetDriveError(DVD::DriveError::ReadError);
    interrupt = DVD::DIInterruptType::DEINT;
  }
  else
  {
    if (request.copy_to_ram)
    {
      auto& memory = m_system.GetMemory();
      memory.CopyToEmu(request.output_address, buffer.data(), request.length);
    }

    interrupt = DVD::DIInterruptType::TCINT;
  }

  dvd_interface.FinishExecutingCommand(request.reply_type, interrupt, cycles_late, buffer);
}

void DVDThread::DVDThreadMain()
{
  Common::SetCurrentThreadName("DVD thread");

  while (true)
  {
    m_request_queue_expanded.Wait();

    if (m_dvd_thread_exiting.IsSet())
      return;

    ReadRequest request;
    while (m_request_queue.Pop(request))
    {
      m_file_logger.Log(*m_disc, request.partition, request.dvd_offset);

      std::vector<u8> buffer(request.length);
      if (!m_disc->Read(request.dvd_offset, request.length, buffer.data(), request.partition))
        buffer.resize(0);

      request.realtime_done_us = Common::Timer::NowUs();

      m_result_queue.Push(ReadResult(std::move(request), std::move(buffer)));
      m_result_queue_expanded.Set();

      if (m_dvd_thread_exiting.IsSet())
        return;
    }
  }
}

void DVDThread::PoisonState()
{
    // Trash the Request/Result queues
    // This is hard because they are thread-safe queues.
    // Best we can do is trash the logic variables.
    
    m_next_id = 0xBADDBAD0;
    
    // Ideally clear the map and fill with garbage
    m_result_map.clear();
}

}  // namespace DVD