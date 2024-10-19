#include <string>
#include "Enums.h"
enum class Color {
    Red,
    Green,
    Blue,
    Unknown
};

namespace WiiMixEnums {
    Color StringToColor(const std::string& color) {
        if (color == WII_MIX_RED) {
            return Color::RED;
        } else if (color == WII_MIX_GREEN) {
            return Color::GREEN;
        } else if (color == WII_MIX_BLUE) {
            return Color::BLUE;
        } else if (color == WII_MIX_PURPLE) {
            return Color::PURPLE;
        }
        else {
            return Color::END;
        }
    }

    std::string ColorToHex(Color color) {
        switch (color) {
            case Color::RED:
                return WII_MIX_RED;
            case Color::GREEN:
                return WII_MIX_GREEN;
            case Color::BLUE:
                return WII_MIX_BLUE;
            case Color::PURPLE:
                return WII_MIX_PURPLE;
            default:
                return "Unknown";
        }
    }
};