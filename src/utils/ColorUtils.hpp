#pragma once

using namespace geode::prelude;

class ColorUtils {
    public:
    static ColorUtils& get() {
        static ColorUtils instance;
        return instance;
    }
    std::string hsvToHex(HSV hsv);
    Ref<ColorSelectPopup> m_colorSelectPopup;
    RGBA toRGBA(ccColor3B color);
    void copyColor(ccColor3B color, CCObject* sender);

    private:
    ColorUtils() {
        m_colorSelectPopup = ColorSelectPopup::create({255, 255, 255});
    }
};