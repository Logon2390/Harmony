# pragma once
#include "HueMintManager.hpp"

using namespace geode::prelude;
class HarmonyManager {
    public:
    Palette getComplementary(ccColor3B const& color);
    Palette getAnalogous(ccColor3B const& color);
    Palette getTriadic(ccColor3B const& color);
    Palette getTetradic(ccColor3B const& color);
    Palette getSplitComplementary(ccColor3B const& color);
    Palette getSquare(ccColor3B const& color);
    Palette getShades(ccColor3B const& color);
    Palette getTints(ccColor3B const& color);
    Palette getColorScale(ccColor3B const& color);
    static HarmonyManager& get() {
        static HarmonyManager instance;
        return instance;
    }

    private:
    HarmonyManager() = default;
    RGBA toRGBA(ccColor3B const& color);
    std::string hsvToHex(HSV hsv);
    float wrapHue(float h);
};