#include "DolphinQt/WiiMix/RogueSettings.h"
#include <QList>
#include "Common/Config/Config.h"
#include "Core/Config/MainSettings.h"

WiiMixRogueSettings::WiiMixRogueSettings(WiiMixEnums::Difficulty difficulty, WiiMixEnums::SaveStateBank bank, std::vector<WiiMixObjective> objectives, WiiMixEnums::RogueLength length) : WiiMixCommonSettings(
    difficulty,
    bank,
    objectives
), m_length(length) {
    if (length != DEFAULT_ROGUE_LENGTH) {
        m_length = length;
    }
    else if (Config::Get(Config::WIIMIX_ROGUE_LENGTH) != DEFAULT_ROGUE_LENGTH) {
        m_length = Config::Get(Config::WIIMIX_ROGUE_LENGTH);
    }
    else {
        m_length = DEFAULT_ROGUE_LENGTH;
    }
}

void WiiMixRogueSettings::SetSeed(QString seed) {
    m_seed = seed;
}

QList<WiiMixEnums::RogueEvent> WiiMixRogueSettings::GetEvents()
{

}

void WiiMixRogueSettings::SetDifficulty(WiiMixEnums::Difficulty difficulty) {
    m_difficulty = difficulty;
    Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_DIFFICULTY, difficulty);
    // emit SettingsChanged(difficulty);
}

void WiiMixRogueSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) {
    m_save_state_bank = save_state_bank;
    Config::Set(Config::LayerType::Base, Config::WIIMIX_ROGUE_SAVE_STATE_BANK, save_state_bank);
    // emit SettingsChanged(save_state_bank);
}

QList<WiiMixEnums::RogueItem> WiiMixRogueSettings::GetItemSet()
{
    // Returns m_items
    // Populates m_items using GetLength() if m_items is empty
    if (m_items.isEmpty())
    {
        WiiMixEnums::RogueLength length = GetLength();
        for (int i = 0; i < static_cast<int>(length); i++)
        {
            // Assuming Item has a constructor that takes an int or some other logic to populate items
            m_items.append(WiiMixEnums::RogueItem(i));
        }
    }
    return m_items;
}

void WiiMixRogueSettings::SetEvents(QList<WiiMixEnums::RogueEvent> events)
{
    m_events = events;
}

void WiiMixRogueSettings::SetItems(QList<WiiMixEnums::RogueItem> items)
{
    m_items = items;
}

WiiMixEnums::RogueLength WiiMixRogueSettings::GetLength()
{
    return m_length;
}

void WiiMixRogueSettings::SetLength(WiiMixEnums::RogueLength length)
{
    m_length = length;
}

QString WiiMixRogueSettings::LengthToString(WiiMixEnums::RogueLength length)
{
    switch (length) {
        case WiiMixEnums::RogueLength::SHORT:
            return QStringLiteral("Short");
        case WiiMixEnums::RogueLength::MEDIUM:
            return QStringLiteral("Medium");
        case WiiMixEnums::RogueLength::MARATHON:
            return QStringLiteral("Marathon");
        default:
            return QStringLiteral("");
    }
}

WiiMixEnums::RogueLength WiiMixRogueSettings::StringToLength(QString length)
{
    if (length == QStringLiteral("Short")) {
        return WiiMixEnums::RogueLength::SHORT;
    }
    else if (length == QStringLiteral("Medium")) {
        return WiiMixEnums::RogueLength::MEDIUM;
    }
    else if (length == QStringLiteral("Long") || length == QStringLiteral("MARATHON")) {
        return WiiMixEnums::RogueLength::MARATHON;
    }
    else {
        return WiiMixEnums::RogueLength::END;
    }
}
