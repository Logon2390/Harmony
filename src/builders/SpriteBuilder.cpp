#include "SpriteBuilder.hpp"

CCSprite *SpriteBuilder::createArrow(bool flipped, float scale) {
  auto spr = CCSprite::createWithSpriteFrameName(arrowSprName);
  spr->setScale(scale);
  spr->setFlipX(flipped);
  return spr;
}

