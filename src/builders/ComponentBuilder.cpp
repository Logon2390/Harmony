#include "ComponentBuilder.hpp"
#include "SpriteBuilder.hpp"

CCNode *ComponentBuilder::createOptionSelector(const char *title, float width, SelectorType type, MenuAction action, CCObject *target) {
  auto option = CCNode::create();
  option->setContentSize({width, 25.f});

  auto bg = geode::NineSlice::create(SpriteBuilder::backgroundSprName, {0, 0, 80, 80});
  bg->setContentSize({width, 25.f});
  bg->setColor(ccBLACK);
  bg->setOpacity(35);
  option->addChildAtPosition(bg, Anchor::Center);

  CCMenu *menu = CCMenu::create();
  menu->setContentSize({width, 25.f});
  option->addChildAtPosition(menu, Anchor::Center);

  auto infoSpr = CCSprite::createWithSpriteFrameName(SpriteBuilder::infoIconName);
  infoSpr->setScale(0.5f);

  auto infoBtn = CCMenuItemSpriteExtra::create(infoSpr, infoSpr, target, action.onInfo);
  menu->addChildAtPosition(infoBtn, Anchor::Left, ccp(10.f, 0.f));

  auto label = CCLabelBMFont::create(title, SpriteBuilder::goldFontName);
  label->setScale(0.5f);
  label->setAnchorPoint({0.f, 0.5f});
  bg->addChildAtPosition(label, Anchor::Left, ccp(20.f, 0.f));

  if (type == SelectorType::Button) {
    ButtonSprite *btnSpr = ButtonSprite::create(action.title.c_str());
    btnSpr->setScale(0.6f);

    auto btn = CCMenuItemSpriteExtra::create(btnSpr, btnSpr, target, action.action);
    btn->m_scaleMultiplier = 1.1f;
    btn->setAnchorPoint({1.f, 0.5f});
    menu->addChildAtPosition(btn, Anchor::Right, ccp(-10.f, 0.f));
  }

  if (type == SelectorType::Option) {
    if (!action.node) return option;

    if (action.onNext) {
      auto nextSpr = SpriteBuilder::createArrow(ArrowSprite::Green, true, 0.4f);
      auto nextBtn = CCMenuItemSpriteExtra::create(nextSpr, nextSpr, target, action.onNext);
      menu->addChildAtPosition(nextBtn, Anchor::Right, ccp(-20.f, 0.f));
    }

    menu->addChildAtPosition(action.node, Anchor::Right, ccp(-75.f, 0.f));

    if (action.onPrevious) {
      auto prevSpr = SpriteBuilder::createArrow(ArrowSprite::Green, false, 0.4f);
      auto prevBtn = CCMenuItemSpriteExtra::create(prevSpr, target, action.onPrevious);
      menu->addChildAtPosition(prevBtn, Anchor::Right, ccp(-130.f, 0.f));
    }
    menu->updateLayout();
  }

  return option;
}