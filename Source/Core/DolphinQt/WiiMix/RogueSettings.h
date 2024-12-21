// Copyright 2017 Dolphin Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "DolphinQt/WiiMix/Enums.h"
#include "DolphinQt/WiiMix/Settings.h"

#include <QList>
#include <QString>

#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

// CLIENT ROGUE SETTINGS -> singleton
class WiiMixRogueSettings : public WiiMixSettings 
{
public:
  static WiiMixRogueSettings* instance(WiiMixEnums::RogueLength rogue_length = DEFAULT_ROGUE_LENGTH) {
    if (!s_instance) {
        s_instance = new WiiMixRogueSettings(rogue_length);
    }
    return static_cast<WiiMixRogueSettings*>(s_instance);
  }

  explicit WiiMixRogueSettings(WiiMixEnums::RogueLength length);

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
  void SetSeed(QString string);

  static QString LengthToString(WiiMixEnums::RogueLength length);
  static WiiMixEnums::RogueLength StringToLength(QString length);

  QList<WiiMixEnums::RogueEvent> GetEvents();
  WiiMixEnums::RogueEvent GetEvent();
  void SetEvents(QList<WiiMixEnums::RogueEvent> events);
  QList<WiiMixEnums::RogueItem> GetItemSet();
  void SetItems(QList<WiiMixEnums::RogueItem>);
  WiiMixEnums::RogueItem GetItem();
  QList<WiiMixEnums::RogueItem> GetPlayerItems();
  void SetPlayerItems(QList<WiiMixEnums::RogueItem> items);
  void AddPlayerItem(WiiMixEnums::RogueItem item);
  QList<WiiMixEnums::RogueItem> Get3Items();
  WiiMixEnums::RogueLength GetLength();
  void SetLength(WiiMixEnums::RogueLength length);
  void SetDifficulty(WiiMixEnums::Difficulty difficulty) override;
  void SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) override;


private:
  QList<WiiMixEnums::RogueEvent> m_events;
  QList<WiiMixEnums::RogueItem> m_items;
  QList<WiiMixEnums::RogueItem> m_player_items;
  WiiMixEnums::RogueLength m_length;
  // Endless can potentially come later
  // bool m_endless;
  QString m_seed;

};
