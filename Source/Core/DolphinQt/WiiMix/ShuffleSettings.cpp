#include "DolphinQt/WiiMix/ShuffleSettings.h"


WiiMixShuffleSettings::WiiMixShuffleSettings(const WiiMixSettings& settings, 
    int number_of_switches, 
    int min_time_between_switch, 
    int max_time_between_switch, 
    bool endless)
    : WiiMixSettings(settings),  // Initialize base class with settings
      m_number_of_switches(number_of_switches),
      m_min_time_between_switch(min_time_between_switch),
      m_max_time_between_switch(max_time_between_switch),
      m_endless(endless)
{}

bool WiiMixShuffleSettings::GetNumberOfSwitches() const
{
    return m_number_of_switches;
}

void WiiMixShuffleSettings::SetNumberOfSwitches(int value)
{
    m_number_of_switches = value;
}

int WiiMixShuffleSettings::GetMinTimeBetweenSwitch() const
{
    return m_min_time_between_switch;
}

void WiiMixShuffleSettings::SetMinTimeBetweenSwitch(int value)
{
    m_min_time_between_switch = value;
}

int WiiMixShuffleSettings::GetMaxTimeBetweenSwitch() const
{
    return m_max_time_between_switch;
}

void WiiMixShuffleSettings::SetMaxTimeBetweenSwitch(int value)
{
    m_max_time_between_switch = value;
}

bool WiiMixShuffleSettings::GetEndless() const
{
    return m_endless;
}

void WiiMixShuffleSettings::SetEndless(bool value)
{
    m_endless = value;
}