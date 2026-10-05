#include "SpriteBuilder.hpp"
#include <Geode/ui/BasedButtonSprite.hpp>

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
    float rotation = isCorner && index == limit - 1 ? 180.f : 0.f;
    const char *spriteName = isCorner ? SpriteBuilder::backgroundSprName: SpriteBuilder::squareSprName;
    CCRect rect = isCorner ? CCRect{0, 0, 50, 80} : CCRect{0, 0, 80, 80};

    colorSpr = NineSlice::create(spriteName, rect);
    colorSpr->setRotation(rotation);

    if (CCNode *old = btn->getChildByID("hsv-color")) old->removeFromParent();
    NineSlice *hsvSpr = NineSlice::create(spriteName, rect);
    hsvSpr->setRotation(rotation);
    hsvSpr->setID("hsv-color");
    hsvSpr->setAnchorPoint({0.5f, 0.f});
    hsvSpr->setZOrder(2);
    hsvSpr->setVisible(false);
    btn->addChild(hsvSpr);
  } else {
    colorSpr = static_cast<NineSlice *>(btn->getNormalImage());
  }
  btn->setUserFlag("corner"_spr, isRightCorner);
  colorSpr->setContentSize({width, height});
  colorSpr->setColor(ccWHITE);

  btn->updateLayout();
  return colorSpr;
}

void SpriteBuilder::setCircleButtonColor(CCMenuItemSpriteExtra *btn, bool active, float topScale) {
  auto old = typeinfo_cast<CircleButtonSprite *>(btn->getNormalImage());
  if (!old) return;

  // keep the top node alive while the old base is replaced
  Ref<CCNode> top = old->getTopNode();

  auto color = active ? CircleBaseColor::Cyan : CircleBaseColor::Green;
  auto base = CircleButtonSprite::create(top, color, CircleBaseSize::Tiny);
  base->setTopRelativeScale(topScale);
  btn->setNormalImage(base);
  if (btn->getSelectedImage()) btn->setSelectedImage(base);
  btn->setContentSize(base->getContentSize());
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

