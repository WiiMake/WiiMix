#include "DolphinQt/WiiMix/RogueSettings.h"

#include <QList>

WiiMixRogueSettings::WiiMixRogueSettings(const WiiMixSettings& settings)
    : WiiMixSettings(settings)
{}

WiiMixRogueSettings::WiiMixRogueSettings()
        : WiiMixSettings()
{}

void WiiMixRogueSettings::SetSeed(QString seed) {
    m_seed = seed;
}

QList<WiiMixRogueSettings::Event> WiiMixRogueSettings::GetEvents()
{
  return m_events;
}

QList<WiiMixRogueSettings::Item> WiiMixRogueSettings::GetItems()
{
    // Returns m_items
    // Populates m_items using GetLength() if m_items is empty
    if (m_items.isEmpty())
    {
        WiiMixRogueSettings::Length length = GetLength();
        for (int i = 0; i < static_cast<int>(length); ++i)
        {
            // Assuming Item has a constructor that takes an int or some other logic to populate items
            m_items.append(Item(i));
        }
    }
    return m_items;
}

void WiiMixRogueSettings::SetEvents(QList<WiiMixRogueSettings::Event> events)
{
    m_events = events;
}

void WiiMixRogueSettings::SetItems(QList<WiiMixRogueSettings::Item> items)
{
    m_items = items;
}

WiiMixRogueSettings::Length WiiMixRogueSettings::GetLength()
{
    return m_length;
}

void WiiMixRogueSettings::SetLength(WiiMixRogueSettings::Length length)
{
    m_length = length;
}

QString WiiMixRogueSettings::LengthToString(Length length)
{
    switch (length) {
        case Length::SHORT:
            return QStringLiteral("Short");
        case Length::MEDIUM:
            return QStringLiteral("Medium");
        case Length::MARATHON:
            return QStringLiteral("Marathon");
        default:
            return QStringLiteral("");
    }
}

WiiMixRogueSettings::Length WiiMixRogueSettings::StringToLength(QString length)
{
    if (length == QStringLiteral("Short")) {
        return Length::SHORT;
    }
    else if (length == QStringLiteral("Medium")) {
        return Length::MEDIUM;
    }
    else if (length == QStringLiteral("Long") || length == QStringLiteral("Marathon")) {
        return Length::MARATHON;
    }
    else {
        return Length::END;
    }
}
