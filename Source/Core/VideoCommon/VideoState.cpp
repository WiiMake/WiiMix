// Copyright 2008 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "VideoCommon/VideoState.h"

#include <cstring>

#include "Common/ChunkFile.h"
#include "Core/System.h"
#include "VideoCommon/BPMemory.h"
#include "VideoCommon/BPStructs.h"
#include "VideoCommon/BoundingBox.h"
#include "VideoCommon/CPMemory.h"
#include "VideoCommon/CommandProcessor.h"
#include "VideoCommon/Fifo.h"
#include "VideoCommon/FrameDumper.h"
#include "VideoCommon/FramebufferManager.h"
#include "VideoCommon/GeometryShaderManager.h"
#include "VideoCommon/PixelEngine.h"
#include "VideoCommon/PixelShaderManager.h"
#include "VideoCommon/Present.h"
#include "VideoCommon/TMEM.h"
#include "VideoCommon/TextureCacheBase.h"
#include "VideoCommon/TextureDecoder.h"
#include "VideoCommon/VertexLoaderManager.h"
#include "VideoCommon/VertexManagerBase.h"
#include "VideoCommon/VertexShaderManager.h"
#include "VideoCommon/Widescreen.h"
#include "VideoCommon/XFMemory.h"
#include "VideoCommon/XFStateManager.h"

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
  // if (!WIIMIX_STATE) {
    system.GetPixelShaderManager().DoState(p);
    p.DoMarker("PixelShaderManager");

    system.GetVertexShaderManager().DoState(p);
    p.DoMarker("VertexShaderManager");

    system.GetGeometryShaderManager().DoState(p);
    p.DoMarker("GeometryShaderManager");
  // }

  // Do not save the host's vertex cache for WiiMix states
  // if (!WIIMIX_STATE) {
    g_vertex_manager->DoState(p);
    p.DoMarker("VertexManager");
  // }

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

  g_presenter->DoState(p);

  if (!WIIMIX_STATE) {
    g_frame_dumper->DoState(p);
  }
  p.DoMarker("Presenter");

  g_bounding_box->DoState(p);
  p.DoMarker("Bounding Box");

  g_widescreen->DoState(p);
  p.DoMarker("Widescreen");

  system.GetXFStateManager().DoState(p);
  p.DoMarker("XFStateManager");

  // Refresh state.
  // Already handled in WiiMixHostReinitialization
  if (p.IsReadMode())
  {
    // Inform backend of new state from registers.
    system.GetPixelShaderManager().Dirty();
    system.GetVertexShaderManager().dirty = true;
    system.GetGeometryShaderManager().Dirty();

    // FIX 2: BPReload applies the registers to the backend.
    BPReload();
    
    // FIX 3: Mark all vertex loaders dirty so they are re-generated.
    VertexLoaderManager::MarkAllDirty();
  }
}
