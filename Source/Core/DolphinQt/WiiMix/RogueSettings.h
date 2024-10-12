// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/Settings.h"

#pragma once

class WiiMixRogueSettings : public WiiMixSettings 
{
public:
  explicit WiiMixRogueSettings(const WiiMixSettings& settings);
  enum class Items {
    CANCEL_EFFECT, // Can be used before the final challenge; MUST be used BEFORE starting a round
    SWAP_OBJECTIVE, // Used to replace an objective with a different one
    EXTRA_TIME, // Used automatically if you run out of time; gives you 15 extra seconds
    PAUSE, // Grants the ability to pause for a round
    END
  };
  // There's 10 objectives; the 1st, 3rd, and 5th rounds are always items
  // The final stage is a different objective, but always the same Event:
  // A multiobjective guantlet of boss objectives between different games:
  // The first challenge is limited button presses
  // The next challenge has inverted movement every x seconds (random interval)
  // The final challenge is a survival shuffle round between 10 states marked as survival with save state manipulation,
  // pausing and playing, and speedup and slowdown
  // but if you survive for 100 seconds the next load will fail and you'll win
  enum class Events {
    GET_ITEM,
    LIMITED_A_BUTTON,
    LIMITED_B_BUTTON,
    INVERTED_X,
    INVERTED_Y,
    // INDUCED_LATENCY,
    INCREASED_SPEED,
    VARIABLE_SPEED,
    RANDOMIZED_CONTROLS,
    STRICTER_TIME_LIMIT,
    // DVD_SCREENSAVER_WINDOW,
    LIMITED_BUTTONS,
    INVERTED_MOVEMENT,
    SAVE_STATE_MANIPULATION,
    END
  };

private:

};
