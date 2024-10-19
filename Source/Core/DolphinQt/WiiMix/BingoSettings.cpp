#include "DolphinQt/WiiMix/BingoSettings.h"

WiiMixBingoSettings::WiiMixBingoSettings(const WiiMixSettings& settings, bool is_lockout, int card_size)
    : WiiMixSettings(settings), m_is_lockout(is_lockout), m_card_size(card_size) 
{}

bool WiiMixBingoSettings::GetLockout() const
{
    return m_is_lockout;
}

void WiiMixBingoSettings::SetLockout(bool value)
{
    m_is_lockout = value;
}

int WiiMixBingoSettings::GetCardSize() const
{
    return m_card_size;
}

void WiiMixBingoSettings::SetCardSize(int value)
{
    // Check if the square root of the value is an integer
    // and if the value is odd
    int sqrt_value = static_cast<int>(sqrt(value));
    if (sqrt_value * sqrt_value == value && value % 2 != 0)
    {
        m_card_size = value;
    }
}

bool WiiMixBingoSettings::GetTeams()
{
    return m_teams;
}

void WiiMixBingoSettings::SetTeams(bool value)
{
    m_teams = value;
}