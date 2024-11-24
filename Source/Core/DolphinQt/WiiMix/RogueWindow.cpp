#include "DolphinQt/WiiMix/RogueWindow.h"
#include "DolphinQt/WiiMix/RogueSettings.h"

WiiMixRogueWindow::WiiMixRogueWindow(const WiiMixRogueSettings& settings)
    : WiiMixRogueSettings(settings)
{}

WiiMixRogueSettings::Event WiiMixRogueSettings::GetEvent()
{
    int num_events = static_cast<int>(WiiMixRogueSettings::Events::END);
    WiiMixRogueSettings::Event event = rand() % num_events;
    return event;
}

WiiMixRogueSettings::Item WiiMixRogueSettings::GetItem()
{
    int num_items = static_cast<int>(WiiMixRogueSettings::Items::END);
    WiiMixRogueSettings::Item item = static_cast<WiiMixRogueSettings::Item>(rand() % num_items);
    return item;
}