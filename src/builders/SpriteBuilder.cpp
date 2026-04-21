#include "SpriteBuilder.hpp"

CCSprite *SpriteBuilder::createArrow(ArrowSprite sprite, bool flipped, float scale)
{
    auto spr = CCSprite::createWithSpriteFrameName(formatArrowSpriteName(sprite));
    spr->setScale(scale);
    spr->setFlipX(flipped);
    return spr;
}

/*
    Sprite used for color buttons in the main popup and simulation setup popup. 
    - If the button is in the first or last position, it will use a 9-slice sprite with rounded corners.
    - If the button is in the middle, it will use a regular square sprite.
    - The function also checks if the sprite needs to be created or if it can reuse the existing one, and updates its size and color accordingly.
*/
NineSlice *SpriteBuilder::createColorSpr(CCMenuItemSpriteExtra *btn, int index, int limit, float width, float height) {
  bool flag = btn->getUserFlag("corner"_spr);
  bool isRightCorner = index == limit - 1;
  bool init = width == 0.f && height == 0.f;
  bool create = init || isRightCorner && !flag || flag && !isRightCorner;
  NineSlice *colorSpr;

  if (create) {
    bool isCorner = index == 0 || index == limit - 1;
    const char *spriteName = isCorner ? SpriteBuilder::backgroundSprName: SpriteBuilder::squareSprName;
    CCRect rect = isCorner ? CCRect{0, 0, 50, 80} : CCRect{0, 0, 80, 80};

    colorSpr = NineSlice::create(spriteName, rect);
    colorSpr->setRotation(isCorner && index == limit - 1 ? 180.f : 0.f);
  } else {
    colorSpr = static_cast<NineSlice *>(btn->getNormalImage());
  }
  btn->setUserFlag("corner"_spr, isRightCorner);
  colorSpr->setContentSize({width, height});
  colorSpr->setColor({255, 255, 255});
  return colorSpr;
}

const char* SpriteBuilder::formatArrowSpriteName(ArrowSprite sprite) {
    switch (sprite) {
        case ArrowSprite::Green:
            return SpriteBuilder::arrow1SprName;
        case ArrowSprite::Cyan:
            return SpriteBuilder::arrow2SprName;
        case ArrowSprite::Pink:
            return SpriteBuilder::arrow3SprName;
        default:
            return SpriteBuilder::arrow1SprName;
    }
}

