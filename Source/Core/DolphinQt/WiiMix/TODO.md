# TODO

> So me and my goldfish memory can remember what I was working on

## FIRST RELEASE

TAKE CARE OF TOMORROW, IN ADDITION TO CS252

- [untested] Fix game updating
- [] Finish implementing handling completed bingo objectives on server
  - [x] Update non-server bingo settings
  - [x] Add a server side method for detecting a completed objective
  - [x] Rework m_completed_objectives to include a value, i.e. score in the case of time attack
- [most implemented, but still reliant on a lot; untested] Implement timer stuff for time attack; all the timer logic should be OUTSIDE of objectives; there should be an objective timer, sure, but it should be handled probably in main window and NOT in the objectives (although the objective should maintain completion_time); m_objective_timer in MainWindow
- [] Figure out how to implement a stop wiimix function
- [x] Track minimum completion time in psql

- [x] Set up an extra event listener for gamepad input that ONLY runs during a wiimix
  - Would need to also implement custom button selectors for the WiiMix hotkey window to make sure the right inputs are used
- [x] Map keyboard shortcuts to controller

- [untested] Checksum to check if the user has a file already so as to not download it

- [] Bingo server crashes

- [] Make sure local directories initialize on all devices (i.e. objectives/ in save states directory)
- [] Generalize ShuffleGame to just be a generic retroachievements completion handler
  - [] Need to connect ShuffleGame to
- [] End Screen?

## NEW YEARS RELEASE

- [] Save State Client/Server Encryption/Decryption Validation System
- [] Sending States to the Purdue Hackers Server
- [] Fix the Toolbar (fullscreen)
- [] Migrate Bingo to Purdue Hackers rack
- [] Seeding logic
- [] Basic Windows Testing/Fixes
- [] Build and host a simple interface plus database for achievements/games supported
- [] Roguelike (may not be possible within time constraints, we'll see)
- [] Streamline keyboard shortcuts to be user friendly
- [] New Trailer
- [] Significantly more testing of starting games with state and saving to previous state
- [] Add Dev Notes to the ReadMe
  - git submodule add --init --recursive

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
