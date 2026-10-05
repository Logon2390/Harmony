#pragma once
#include <string>

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

    // HSV utility functions
    static const ccHSVValue HSV_IDENTITY;
    static bool isHsvIdentity(const ccHSVValue& hsv);

    // applies the HSV transform to a color
    static ccColor3B applyHsv(ccColor3B color, const ccHSVValue& hsv);
    static std::string applyHsvHex(const std::string& hex, const ccHSVValue& hsv);

    private:
    ColorUtils() {
        m_colorSelectPopup = ColorSelectPopup::create({255, 255, 255});
    }
};