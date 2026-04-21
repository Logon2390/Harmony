#pragma once

using namespace geode::prelude;

class ColorUtils {
    public:
    static ColorUtils& get() {
        static ColorUtils instance;
        return instance;
    }
    ColorSelectPopup* m_colorSelectPopup;
    ccColor3B hexToColor(const std::string& hex);
    std::string colorToHex(ccColor3B color);
    std::string hsvToHex(HSV hsv);
    RGBA toRGBA(ccColor3B color);
    void copyColor(ccColor3B color, CCObject* sender);
    void applyColorToSprite(NineSlice* sprite, std::string hex = "#FFFFFF");
    void applyColorToSprite(CCSprite* sprite, std::string hex = "#FFFFFF");

    private:
    ColorUtils() {
        m_colorSelectPopup = ColorSelectPopup::create({255, 255, 255});
        m_colorSelectPopup->retain();
    }
};