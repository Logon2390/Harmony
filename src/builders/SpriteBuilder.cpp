#include "SpriteBuilder.hpp"

CCSprite *SpriteBuilder::createArrow(ArrowSprite sprite, bool flipped, float scale)
{
    auto spr = CCSprite::createWithSpriteFrameName(formatArrowSpriteName(sprite));
    spr->setScale(scale);
    spr->setFlipX(flipped);
    return spr;
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

