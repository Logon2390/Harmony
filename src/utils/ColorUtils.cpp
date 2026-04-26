#include "ColorUtils.hpp"

std::string ColorUtils::hsvToHex(HSV hsv)
{
    RGBA rgba = CCControlUtils::RGBfromHSV(hsv);
    uint8_t r = static_cast<uint8_t>(rgba.r * 255.0);
    uint8_t g = static_cast<uint8_t>(rgba.g * 255.0);
    uint8_t b = static_cast<uint8_t>(rgba.b * 255.0);
    return fmt::format("{:02X}{:02X}{:02X}", r, g, b);
}

RGBA ColorUtils::toRGBA(ccColor3B color)
{
    return RGBA{color.r / 255.0, color.g / 255.0, color.b / 255.0, 1.0};
}

void ColorUtils::copyColor(ccColor3B color, CCObject *sender)
{
    m_colorSelectPopup->m_colorPicker->setColorValue(color);
    m_colorSelectPopup->onCopy(sender);

    Notification::create("Color copied!", NotificationIcon::Success)->show();
}