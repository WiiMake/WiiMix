#include "DolphinQt/WiiMix/BingoSettings.h"

#include "DolphinQt/WiiMix/Enums.h"

WiiMixBingoSettings::WiiMixBingoSettings(const WiiMixSettings& settings, WiiMixEnums::BingoType bingo_type, int card_size)
    : WiiMixSettings(settings), m_bingo_type(bingo_type), m_card_size(card_size) 
{}

WiiMixEnums::BingoType WiiMixBingoSettings::GetBingoType() const
{
    return m_bingo_type;
}

void WiiMixBingoSettings::SetBingoType(WiiMixEnums::BingoType value)
{
    m_bingo_type = value;
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