#include "DolphinQt/WiiMix/ShuffleSettings.h"


WiiMixShuffleSettings::WiiMixShuffleSettings(
    WiiMixEnums::Difficulty difficulty,
      WiiMixEnums::SaveStateBank save_state_bank,
      std::vector<WiiMixObjective> objectives,
    int number_of_switches, 
    int min_time_between_switch, 
    int max_time_between_switch, 
    bool endless)
    : WiiMixCommonSettings(
        difficulty,
        save_state_bank,
        objectives
    ),
      m_number_of_switches(number_of_switches),
      m_min_time_between_switch(min_time_between_switch),
      m_max_time_between_switch(max_time_between_switch),
      m_endless(endless)
{
    if (number_of_switches != DEFAULT_NUMBER_OF_SWITCHES) {
        m_number_of_switches = number_of_switches;
    } else if (Config::Get(Config::WIIMIX_NUMBER_OF_SWITCHES) != DEFAULT_NUMBER_OF_SWITCHES) {
        m_number_of_switches = Config::Get(Config::WIIMIX_NUMBER_OF_SWITCHES);
    } else {
        m_number_of_switches = DEFAULT_NUMBER_OF_SWITCHES;
    }

    if (min_time_between_switch != DEFAULT_MIN_SWITCH_TIME) {
        m_min_time_between_switch = min_time_between_switch;
    } else if (Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH) != DEFAULT_MIN_SWITCH_TIME) {
        m_min_time_between_switch = Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH);
    } else {
        m_min_time_between_switch = DEFAULT_MIN_SWITCH_TIME;
    }
    qDebug() << "min_time_between_switch: " << m_min_time_between_switch;

    if (max_time_between_switch != DEFAULT_MAX_SWITCH_TIME) {
        m_max_time_between_switch = max_time_between_switch;
    } else if (Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH) != DEFAULT_MAX_SWITCH_TIME) {
        m_max_time_between_switch = Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH);
    } else {
        m_max_time_between_switch = DEFAULT_MAX_SWITCH_TIME;
    }
    qDebug() << "max_time_between_switch: " << m_max_time_between_switch;

    if (endless != DEFAULT_IS_ENDLESS) {
        m_endless = endless;
    } else if (Config::Get(Config::WIIMIX_IS_ENDLESS) != DEFAULT_IS_ENDLESS) {
        m_endless = Config::Get(Config::WIIMIX_IS_ENDLESS);
    } else {
        m_endless = DEFAULT_IS_ENDLESS;
    }
}

int WiiMixShuffleSettings::GetNumberOfSwitches() const
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

void WiiMixShuffleSettings::SetDifficulty(WiiMixEnums::Difficulty difficulty) {
    m_difficulty = difficulty;
    Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_DIFFICULTY, difficulty);
    // emit SettingsChanged(difficulty);
}

void WiiMixShuffleSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) {
    m_save_state_bank = save_state_bank;
    Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_SAVE_STATE_BANK, save_state_bank);
    // emit SettingsChanged(save_state_bank);
}