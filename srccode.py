PROMPT = """==================================State.cpp====================================
{}
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
======================================VertexManagerBase.cpp============================
{}
======================================VertexLoader.cpp============================
{}
======================================VertexShaderGen.cpp============================
{}
======================================Savestate.cpp============================
{}
======================================StateUtils.cpp============================
{}
======================================Sram.cpp============================
{}
======================================Memcard.cpp============================
{}
======================================Memory.cpp============================
{}
======================================EXI_DeviceMemoryCard.cpp============================
{}
======================================EXI_DeviceIPL.cpp============================
{}
======================================EXI_Device.cpp============================
{}
======================================EXI_Memory.cpp============================
{}
======================================EXI_DeviceEthernet.cpp============================
{}
======================================EXI_DeviceSP1.cpp============================
{}
======================================EXI_DeviceAD16.cpp============================
{}
======================================EXI_DeviceDummy.cpp============================
{}
======================================ProgramShaderCache.cpp============================
{}
======================================CommandProcessor.cpp============================
{}
======================================PixelEngine.cpp============================
{}
======================================Fifo.cpp============================
{}
======================================MemoryInterface.cpp============================
{}
"""
# ======================================test_savestates.py============================
# {}
# ============================test_output===========================================
# {}

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
        "Source/Core/VideoCommon/VertexManagerBase.cpp",  # Added
        "Source/Core/VideoCommon/VertexLoader.cpp",  # Added
        "Source/Core/VideoCommon/VertexShaderGen.cpp",  # Added
        "Source/Core/VideoCommon/Savestate.cpp",
        "Source/Core/VideoCommon/StateUtils.cpp",
        "Source/Core/Core/HW/Sram.cpp",
        "Source/Core/Core/HW/Memcard.cpp",
        "Source/Core/Core/HW/Memory.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceMemoryCard.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceIPL.cpp",
        "Source/Core/Core/HW/EXI/EXI_Device.cpp",
        "Source/Core/Core/HW/EXI/EXI_Memory.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceEthernet.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceSP1.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceAD16.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceDummy.cpp",
        "Source/Core/VideoBackends/OGL/ProgramShaderCache.cpp",
        "Source/Core/VideoCommon/CommandProcessor.cpp",
        "Source/Core/VideoCommon/PixelEngine.cpp",
        "Source/Core/VideoCommon/Fifo.cpp",
        "Source/Core/Core/HW/MemoryInterface.cpp",
        # "test_savestates.py",
        # "test_output",
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
        file_contents["Source/Core/VideoCommon/VertexManagerBase.cpp"],  # Added
        file_contents["Source/Core/VideoCommon/VertexLoader.cpp"],  # Added
        file_contents["Source/Core/VideoCommon/VertexShaderGen.cpp"],  # Added
        file_contents["Source/Core/VideoCommon/Savestate.cpp"],
        file_contents["Source/Core/VideoCommon/StateUtils.cpp"],
        file_contents["Source/Core/Core/HW/Sram.cpp"],
        file_contents["Source/Core/Core/HW/Memcard.cpp"],
        file_contents["Source/Core/Core/HW/Memory.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI_DeviceMemoryCard.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI_DeviceIPL.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI_Device.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI_Memory.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI_DeviceEthernet.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI_DeviceSP1.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI_DeviceAD16.cpp"],
        file_contents["Source/Core/Core/HW/EXI/EXI_DeviceDummy.cpp"],
        file_contents["Source/Core/VideoBackends/OGL/ProgramShaderCache.cpp"],
        file_contents["Source/Core/VideoCommon/CommandProcessor.cpp"],
        file_contents["Source/Core/VideoCommon/PixelEngine.cpp"],
        file_contents["Source/Core/VideoCommon/Fifo.cpp"],
        file_contents["Source/Core/Core/HW/MemoryInterface.cpp"],
        # file_contents["test_savestates.py"],
        # file_contents["test_output"],
    )

    # Write the prompt to a file sourcecode.txt
    with open("sourcecode.txt", "w") as f:
        f.write(formatted_prompt)
