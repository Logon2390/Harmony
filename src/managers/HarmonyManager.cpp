#include "HarmonyManager.hpp"


Palette HarmonyManager::getComplementary(ccColor3B const& color)
{
    HSV hsv = CCControlUtils::HSVfromRGB(utils.toRGBA(color));

    Palette palette;
    palette.colors.push_back(utils.hsvToHex(hsv));

    hsv.h = wrapHue(hsv.h + 180.f);
    palette.colors.push_back(utils.hsvToHex(hsv));

    return palette;
}

Palette HarmonyManager::getAnalogous(ccColor3B const& color)
{
    HSV hsv = CCControlUtils::HSVfromRGB(utils.toRGBA(color));
    Palette palette;

    HSV c1 = hsv; c1.h = wrapHue(hsv.h - 30.f);
    HSV c2 = hsv;
    HSV c3 = hsv; c3.h = wrapHue(hsv.h + 30.f);

    palette.colors.push_back(utils.hsvToHex(c1));
    palette.colors.push_back(utils.hsvToHex(c2)); // base
    palette.colors.push_back(utils.hsvToHex(c3));

    return palette;
}

Palette HarmonyManager::getTriadic(ccColor3B const& color)
{
    HSV hsv = CCControlUtils::HSVfromRGB(utils.toRGBA(color));
    Palette palette;

    HSV c1 = hsv;
    HSV c2 = hsv; c2.h = wrapHue(hsv.h + 120.f);
    HSV c3 = hsv; c3.h = wrapHue(hsv.h + 240.f);

    palette.colors.push_back(utils.hsvToHex(c1)); // base
    palette.colors.push_back(utils.hsvToHex(c2));
    palette.colors.push_back(utils.hsvToHex(c3));

    return palette;
}

Palette HarmonyManager::getTetradic(ccColor3B const& color)
{
    HSV hsv = CCControlUtils::HSVfromRGB(utils.toRGBA(color));
    Palette palette;

    HSV c1 = hsv;
    HSV c2 = hsv; c2.h = wrapHue(hsv.h + 60.f);
    HSV c3 = hsv; c3.h = wrapHue(hsv.h + 180.f);
    HSV c4 = hsv; c4.h = wrapHue(hsv.h + 240.f);

    palette.colors.push_back(utils.hsvToHex(c1)); // base
    palette.colors.push_back(utils.hsvToHex(c2));
    palette.colors.push_back(utils.hsvToHex(c3));
    palette.colors.push_back(utils.hsvToHex(c4));

    return palette;
}


Palette HarmonyManager::getSplitComplementary(ccColor3B const& color)
{
    HSV hsv = CCControlUtils::HSVfromRGB(utils.toRGBA(color));
    Palette palette;

    HSV c1 = hsv;
    HSV c2 = hsv; c2.h = wrapHue(hsv.h + 150.f);
    HSV c3 = hsv; c3.h = wrapHue(hsv.h + 210.f);

    palette.colors.push_back(utils.hsvToHex(c1));
    palette.colors.push_back(utils.hsvToHex(c2));
    palette.colors.push_back(utils.hsvToHex(c3));

    return palette;
}

Palette HarmonyManager::getSquare(ccColor3B const& color)
{
    HSV hsv = CCControlUtils::HSVfromRGB(utils.toRGBA(color));
    Palette palette;

    HSV c1 = hsv;
    HSV c2 = hsv; c2.h = wrapHue(hsv.h + 90.f);
    HSV c3 = hsv; c3.h = wrapHue(hsv.h + 180.f);
    HSV c4 = hsv; c4.h = wrapHue(hsv.h + 270.f);

    palette.colors.push_back(utils.hsvToHex(c1));
    palette.colors.push_back(utils.hsvToHex(c2));
    palette.colors.push_back(utils.hsvToHex(c3));
    palette.colors.push_back(utils.hsvToHex(c4));

    return palette;
}


Palette HarmonyManager::getShades(ccColor3B const& color)
{
    HSV hsv = CCControlUtils::HSVfromRGB(utils.toRGBA(color));
    Palette palette;

    for (int i = 0; i < 11; i++) {
        float t = static_cast<float>(i) / 10.f;
        HSV c = hsv;
        c.v = hsv.v * (1.f - t * 0.95f);
        palette.colors.push_back(utils.hsvToHex(c));
    }

    return palette;
}

Palette HarmonyManager::getTints(ccColor3B const& color)
{
    HSV hsv = CCControlUtils::HSVfromRGB(utils.toRGBA(color));
    Palette palette;

    for (int i = 0; i < 11; i++) {
        float t = static_cast<float>(i) / 10.f;
        HSV c = hsv;
        c.s = hsv.s * t;
        c.v = 1.f - t * (1.f - hsv.v);
        palette.colors.push_back(utils.hsvToHex(c));
    }

    return palette;
}

Palette HarmonyManager::getColorScale(ccColor3B const& color)
{
    HSV hsv = CCControlUtils::HSVfromRGB(utils.toRGBA(color));
    Palette palette;

    // 0-4: tints
    // 5: base color
    // 6-10: shades
    for (int i = 0; i < 11; i++) {
        HSV c = hsv;

        if (i < 5) {
            // tints
            float t = static_cast<float>(i) / 5.f;
            c.s = hsv.s * t;
            c.v = 1.f - t * (1.f - hsv.v);
        } else if (i == 5) {
            // base color
            c = hsv;
        } else {
            // shades
            float t = static_cast<float>(i - 5) / 5.f;
            c.v = hsv.v * (1.f - t * 0.95f);
            c.s = std::min<float>(hsv.s + t * (1.f - hsv.s) * 0.3f, 1.0f);
        }

        palette.colors.push_back(utils.hsvToHex(c));
    }

    return palette;
}

float HarmonyManager::wrapHue(float h) 
{
  h = fmod(h, 360.f);
  if (h < 0) h += 360.f;
  return h;
}
