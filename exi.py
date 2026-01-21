if __name__ == "__main__":
    import os

    # Only link a bunch of expansion interface files
    exi_files = [
        "Source/Core/Core/HW/EXI/EXI.cpp",
        "Source/Core/Core/HW/EXI/EXI_Channel.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceMemoryCard.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceIPL.cpp",
        "Source/Core/Core/HW/EXI/EXI_Device.cpp",
        "Source/Core/Core/HW/EXI/EXI_Memory.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceEthernet.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceSP1.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceAD16.cpp",
        "Source/Core/Core/HW/EXI/EXI_DeviceDummy.cpp",
    ]
    file_contents = {}
    for fname in exi_files:
        path = os.path.join(os.path.dirname(__file__), fname)
        if os.path.exists(path):
            with open(path, "r") as f:
                file_contents[fname] = f.read()
        else:
            file_contents[fname] = "<file not found>"

    # Write the linked EXI files to exi_sourcecode.txt
    with open("exi_sourcecode.txt", "w") as f:
        for fname in exi_files:
            f.write(f"========== {fname} ==========\n")
            f.write(file_contents[fname])
            f.write("\n\n")
