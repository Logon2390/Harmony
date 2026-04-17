#pragma once

using namespace geode::prelude;

namespace SpriteBuilder {
    constexpr const char *bigFontName = "bigFont.fnt";
    constexpr const char *goldFontName = "goldFont.fnt";
    constexpr const char* arrowSprName = "GJ_arrow_01_001.png";
    constexpr const char *infoIconName = "GJ_infoIcon_001.png";
    constexpr const char *backgroundSpriteName = "square02b_001.png";
    constexpr const char *hideSprName = "hideBtn_001.png";
    CCSprite *createArrow(bool flipped = false, float scale = 0.6f);
}