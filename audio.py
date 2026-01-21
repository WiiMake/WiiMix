PROMPT = """==================================AudioInterface.cpp====================================
{}
==================================DSP.cpp====================================
{}
==================================DSPHLE.cpp====================================
{}
==================================State.cpp====================================
{}
==================================AudioCommon.cpp====================================
{}
==================================AudioCommon.h====================================
{}
==================================AudioMixer.cpp====================================
{}
==================================AudioMixer.h====================================
{}
==================================AudioStretcher.cpp====================================
{}
==================================AudioStretcher.h====================================
{}
==================================CubebStream.cpp====================================
{}
==================================CubebStream.h====================================
{}
==================================WASAPIStream.cpp====================================
{}
==================================WASAPIStream.h====================================
{}
==================================PulseAudioStream.cpp====================================
{}
==================================PulseAudioStream.h====================================
{}
==================================ALSAStream.cpp====================================
{}
==================================ALSAStream.h====================================
{}
==================================NullSoundStream.cpp====================================
{}
==================================NullSoundStream.h====================================
{}
"""

if __name__ == "__main__":
    import os

    # List of audio-related files to load
    files = [
        "Source/Core/Core/HW/AudioInterface.cpp",
        "Source/Core/Core/HW/DSP.cpp",
        "Source/Core/Core/HW/DSPHLE.cpp",
        "Source/Core/Core/State.cpp",
        "Source/Core/AudioCommon/AudioCommon.cpp",
        "Source/Core/AudioCommon/AudioCommon.h",
        "Source/Core/AudioCommon/AudioMixer.cpp",
        "Source/Core/AudioCommon/AudioMixer.h",
        "Source/Core/AudioCommon/AudioStretcher.cpp",
        "Source/Core/AudioCommon/AudioStretcher.h",
        "Source/Core/AudioCommon/CubebStream.cpp",
        "Source/Core/AudioCommon/CubebStream.h",
        "Source/Core/AudioCommon/WASAPIStream.cpp",
        "Source/Core/AudioCommon/WASAPIStream.h",
        "Source/Core/AudioCommon/PulseAudioStream.cpp",
        "Source/Core/AudioCommon/PulseAudioStream.h",
        "Source/Core/AudioCommon/ALSAStream.cpp",
        "Source/Core/AudioCommon/ALSAStream.h",
        "Source/Core/AudioCommon/NullSoundStream.cpp",
        "Source/Core/AudioCommon/NullSoundStream.h",
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
        file_contents["Source/Core/Core/HW/AudioInterface.cpp"],
        file_contents["Source/Core/Core/HW/DSP.cpp"],
        file_contents["Source/Core/Core/HW/DSPHLE.cpp"],
        file_contents["Source/Core/Core/State.cpp"],
        file_contents["Source/Core/AudioCommon/AudioCommon.cpp"],
        file_contents["Source/Core/AudioCommon/AudioCommon.h"],
        file_contents["Source/Core/AudioCommon/AudioMixer.cpp"],
        file_contents["Source/Core/AudioCommon/AudioMixer.h"],
        file_contents["Source/Core/AudioCommon/AudioStretcher.cpp"],
        file_contents["Source/Core/AudioCommon/AudioStretcher.h"],
        file_contents["Source/Core/AudioCommon/CubebStream.cpp"],
        file_contents["Source/Core/AudioCommon/CubebStream.h"],
        file_contents["Source/Core/AudioCommon/WASAPIStream.cpp"],
        file_contents["Source/Core/AudioCommon/WASAPIStream.h"],
        file_contents["Source/Core/AudioCommon/PulseAudioStream.cpp"],
        file_contents["Source/Core/AudioCommon/PulseAudioStream.h"],
        file_contents["Source/Core/AudioCommon/ALSAStream.cpp"],
        file_contents["Source/Core/AudioCommon/ALSAStream.h"],
        file_contents["Source/Core/AudioCommon/NullSoundStream.cpp"],
        file_contents["Source/Core/AudioCommon/NullSoundStream.h"],
    )

    # Write the prompt to a file sourcecode.txt
    with open("sourcecode.txt", "w") as f:
        f.write(formatted_prompt)
