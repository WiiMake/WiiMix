#include "DolphinQt/WiiMix/RogueWindow.h"
#include "DolphinQt/WiiMix/RogueSettings.h"

WiiMixRogueWindow::WiiMixRogueWindow() {}

WiiMixEnums::RogueEvent WiiMixRogueSettings::GetEvent()
{
    int num_events = static_cast<int>(WiiMixEnums::RogueEvent::END);
    WiiMixEnums::RogueEvent event = static_cast<WiiMixEnums::RogueEvent>(rand() % num_events);
    return event;
}

WiiMixEnums::RogueItem WiiMixRogueSettings::GetItem()
{
    int num_items = static_cast<int>(WiiMixEnums::RogueItem::END);
    WiiMixEnums::RogueItem item = static_cast<WiiMixEnums::RogueItem>(rand() % num_items);
    return item;
}