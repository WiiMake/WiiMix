# TODO

> So me and my goldfish memory can remember what I was working on

## NEW YEARS RELEASE

- [] Save State Client/Server Encryption/Decryption Validation System
- [] Sending States to the Purdue Hackers Server
- [] Fix the Toolbar (fullscreen)
- [] Migrate Bingo to Purdue Hackers rack
- [] Seeding logic (that Xander still hasn't finished yet)
- [] Basic Windows Testing/Fixes
- [] Build and host a simple interface plus database for achievements/games supported
- [] Roguelike (may not be possible within time constraints, we'll see)
- [] Streamline keyboard shortcuts to be user friendly
- [] New Trailer
- [] Significantly more testing of starting games with state and saving to previous state

## NOTES

Red: 218, 81, 51 (#da5133)
Blue: 97, 187, 232 (#61bbe8)
Purple: (#a961e8)
Green: (#61e873)

Include files at /usr/include/x86_64-linux-gnu/qt6; however, there should be NO manual editing of include paths;
the cmake tools extension combined with
{
"name": "CMake",
"compileCommands": "${config:cmake.buildDirectory}/compile_commands.json",
"configurationProvider": "ms-vscode.cmake-tools"
}
should take care of that

Dolphin uses ninja, so instead of "make" you should be running "ninja", and make sure to be compiling with cmake .. -G Ninja -DLINUX_LOCAL_DEV=true -DCMAKE_BUILD_TYPE=Debug, otherwise ninja will develop for release and logging to console won't work

SERVER TODOS

- [x] Fix Server.cpp; I think some of the basic logic is there, but I need to build up the servers and settings for lobbies and manage memory correctly
- [] Fix ConfigWidget; this is reliant on Server.cpp
- WEEKEND GOAL: just get the server to compile; I think we can probably make this happen

- [] Should make an enum
- [] Need to edit config widget to send notifications to the server whenever a setting is changed
- [] Need to structure the server, parse the sent data, and respond accordingly (with template responses at first)
- [] Need to link the server responses to the GUI

- [x] Settings.cpp GameIdsToGameFiles and ObjectiveIdsToObjectives
- [x] SettingsWindow.cpp LoadSettings()
- [x] SettingsWindow.cpp SaveSettings()
- [x] Get it to compile (QDir include issue...)
- [x] Make it so config state persists
- [] Bingo lobby
- [] Test some save and load configurations
- [] Finish implementing shuffle
- [] Bingo networking
- [] Bingo board
- [] Roguelike map
- [] Roguelike events
- [] Figure out how to run dolphins test cases/CI CD?
