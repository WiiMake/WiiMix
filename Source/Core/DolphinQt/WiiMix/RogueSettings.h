// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Settings.h"

#include <QList>
#include <QString>

#pragma once

class WiiMixRogueSettings : public WiiMixSettings 
{
public:
  explicit WiiMixRogueSettings(const WiiMixSettings& settings);
  enum class Item {
    REROLL_EFFECT, // Rerolls the effect for a round
    SWAP_OBJECTIVE, // Used to replace the current objective with a different one
    EXTRA_TIME, // Used automatically if you run out of time; gives you 15 extra seconds
    PAUSE, // Grants the ability to pause for a round
    SPOTLIGHT, // Grants the ability to view the next 3 objectives/challenges
    DOUBLE_TIME, // Make the current challenge slightly more difficult, but skip the next challenge on completion
    END
  };
  // There's 10 objectives; the 2nd, 4th, and 7th rounds are always items
  // The final stage is a different objective, but always the same Event:
  // A multiobjective guantlet of boss objectives between different games:
  // The first challenge is limited button presses
  // The next challenge has inverted movement every x seconds (random interval)
  // The final challenge is a survival shuffle round between 10 states marked as survival with save state manipulation,
  // pausing and playing, and speedup and slowdown
  // but if you survive for 100 seconds the next load will fail and you'll win
  enum class Event {
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
  
  enum class Length {
    SHORT, // 5
    MEDIUM, // 7
    MARATHON, // 10
    END
  };

  // Seeds will be encoded and decoded using Qt
  QString GetSeed();
  // Example seed: 50402523039501042031012343722364155 (34 digits long)
  // 5: Length; the number of events before the finale
  // 040252: the id of achievement 1
  // 303950: the id of achievement 2
  // 104203: the id of achievement 3
  // 1: the number of boss achievements
  // 012345: the boss achievement 
  // 3: the id of event 1
  // 7: the id of event 2
  // 2: the id of event 3
  // 2: the position of the first item selection
  // 2: the first item offered for item 1
  // 3: the 2nd item offered
  // 6: the 3rd item offered
  // 4: the position of the second item selection
  // 1: 1st item
  // 5: 2nd item
  // 5: 3rd item
  void SetSeed();

  static QString LengthToString(Length length);
  static Length StringToLength(QString length);

  QList<WiiMixRogueSettings::Event> GetEvents();
  void SetEvents(QList<WiiMixRogueSettings::Event> events);
  QList<WiiMixRogueSettings::Item> GetItemPool();
  QList<WiiMixRogueSettings::Item> GetPlayerItems();
  void SetPlayerItems(QList<WiiMixRogueSettings::Item> items);
  void AddPlayerItem(WiiMixRogueSettings::Item item);
  QList<WiiMixRogueSettings::Item> Get3Items();
  WiiMixRogueSettings::Length GetLength();
  void SetLength(WiiMixRogueSettings::Length length);

private:
  QList<WiiMixRogueSettings::Event> m_events;
  QList<WiiMixRogueSettings::Item> m_item_pool;
  QList<WiiMixRogueSettings::Item> m_player_items;
  WiiMixRogueSettings::Length m_length;
  // Endless can potentially come later
  // bool m_endless;
  QString m_seed;
};
