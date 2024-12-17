#include "DolphinQt/WiiMix/RogueWindow.h"
#include "DolphinQt/WiiMix/RogueSettings.h"

WiiMixRogueWindow::WiiMixRogueWindow(const WiiMixRogueSettings &settings)
        : WiiMixRogueSettings(settings)
{}

WiiMixRogueSettings::Event WiiMixRogueWindow::GetEvent()
{
    int num_events = static_cast<int>(WiiMixRogueSettings::Event::END);
    WiiMixRogueSettings::Event event = static_cast<WiiMixRogueSettings::Event>(rand() % num_events);
    return event;
}

WiiMixRogueSettings::Item WiiMixRogueWindow::GetItem()
{
    int num_items = static_cast<int>(WiiMixRogueSettings::Item::END);
    WiiMixRogueSettings::Item item = static_cast<WiiMixRogueSettings::Item>(rand() % num_items);
    return item;
}