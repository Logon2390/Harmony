#include "SimulationOverlay.hpp"
#include "../builders/SpriteBuilder.hpp"

SimulationOverlay *SimulationOverlay::create(bool isLiveColorsEnabled, float positionY) {
  auto overlay = new SimulationOverlay();
  if (overlay->init(isLiveColorsEnabled, positionY)) {
    overlay->autorelease();
    return overlay;
  }
  delete overlay;
  return nullptr;
}

void SimulationOverlay::onToggleVisibility() {
  m_menu->setEnabled(simulation.shouldDisplayOverlay());
  this->setVisible(simulation.shouldDisplayOverlay());
  refresh();
}

void SimulationOverlay::refresh() { updateUI(); }

bool SimulationOverlay::init(bool isLiveColorsEnabled, float positionY) {
  if (!this->initWithFile(SpriteBuilder::backgroundSmallSprName,{0.0f, 0.0f, 40.0f, 40.0f}, {})) return false;

  auto winSize = CCDirector::sharedDirector()->getWinSize();
  float offsetY = isLiveColorsEnabled ? positionY + 45.f : positionY + 30.f;

  this->setPosition({winSize.width / 2.f, offsetY});
  this->setContentSize({width, height});
  this->setColor({0, 0, 0});
  this->setOpacity(150);
  this->setVisible(false);
  this->setID("simulation-overlay"_spr);

  m_menu = CCMenu::create();
  m_menu->setContentSize({width, height});
  this->addChildAtPosition(m_menu, Anchor::Center);

  m_next = CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Pink, true, 0.3f), this,
      menu_selector(SimulationOverlay::onNext));
  m_next->setVisible(false);
  m_menu->addChildAtPosition(m_next, Anchor::Right, ccp(-10.f, 0.f));

  m_prev = CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Pink, false, 0.3f), this,
      menu_selector(SimulationOverlay::onPrev));
  m_prev->setVisible(false);
  m_menu->addChildAtPosition(m_prev, Anchor::Right, ccp(-25.f, 0.f));

  m_label = CCLabelBMFont::create("", SpriteBuilder::bigFontName);
  m_label->setScale(0.3f);
  m_label->setAnchorPoint({0.f, 0.5f});
  this->addChildAtPosition(m_label, Anchor::Left, ccp(5.f, 0.f));

  auto shuffleSpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("shuffle.png"_spr), EditorBaseColor::LightBlue);
  shuffleSpr->setScale(0.35f);

  auto visibilitySpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName(SpriteBuilder::hideSprName), EditorBaseColor::DarkGray);
  visibilitySpr->setScale(0.35f);

  m_visibilityBtn = CCMenuItemSpriteExtra::create(visibilitySpr, this, menu_selector(SimulationOverlay::onVisibilityToggle));
  m_menu->addChildAtPosition(m_visibilityBtn, Anchor::Left, ccp(85.f, 0.f));

  m_shuffleBtn = CCMenuItemSpriteExtra::create(shuffleSpr, this, menu_selector(SimulationOverlay::onShuffle));
  m_menu->addChildAtPosition(m_shuffleBtn, Anchor::Left, ccp(65.f, 0.f));

  m_colors = CCNode::create();
  m_colors->setAnchorPoint(ccp(0.5f, 0.5f));
  m_colors->setContentSize({80.f, height});
  m_colors->setLayout(RowLayout::create()
                          ->setGap(1.f)
                          ->setAxisAlignment(AxisAlignment::Start)
                          ->setCrossAxisLineAlignment(AxisAlignment::Center)
                          ->setCrossAxisOverflow(false)
                          ->setAutoScale(false));

  this->addChildAtPosition(m_colors, Anchor::Center, ccp(10.f, 0.f));
  m_colorSprites = CCArray::createWithCapacity(settings.MAX_COLORS);

  for (int i = 0; i < settings.MAX_COLORS; i++) {
    CCSprite *colorSpr = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    m_colorSprites->addObject(colorSpr);
    colorSpr->setVisible(false);
    colorSpr->setScale(0.3f);
    m_colors->addChild(colorSpr);
  }

  updateUI();
  m_menu->updateLayout();
  m_colors->updateLayout();
  return true;
}

void SimulationOverlay::onNext(CCObject *) {
  settings.getNextPalette();
  simulation.replace();
  updateUI();
}

void SimulationOverlay::onPrev(CCObject *) {
  settings.getPrevPalette();
  simulation.replace();
  updateUI();
}

void SimulationOverlay::updateUI() {
  updateNavigationButtons();
  updateInfoLabel();
  updatePalettePreview();
}

void SimulationOverlay::onShuffle(CCObject *) {
  settings.shufflePalette();
  simulation.replace();
  updatePalettePreview();
}

void SimulationOverlay::onVisibilityToggle(CCObject *) {
  this->setOpacity(m_isHidden ? 150 : 50);
  m_shuffleBtn->setOpacity(m_isHidden ? 255 : 50);
  m_next->setOpacity(m_isHidden ? 255 : 50);
  m_prev->setOpacity(m_isHidden ? 255 : 50);
  m_label->setOpacity(m_isHidden ? 255 : 50);

  m_isHidden = !m_isHidden;
}

void SimulationOverlay::updateNavigationButtons() {
  int totalItems = service.getPoolSize();
  bool hasItems = totalItems > 1;

  m_prev->setVisible(hasItems);
  m_next->setVisible(hasItems);
  m_prev->setEnabled(hasItems);
  m_next->setEnabled(hasItems);
}

void SimulationOverlay::updateInfoLabel() {
  m_label->setString(fmt::format("P: {} - {}",
                                 service.getPalettePool().currentItem + 1,
                                 service.getPoolSize())
                         .c_str());
}

void SimulationOverlay::updatePalettePreview() {
  auto colorsSprites = m_colorSprites->asExt<CCSprite *>();
  auto colors = settings.getCurrentPalette().colors;
  int paletteSize = colors.size();

  for (int i = 0; i < settings.MAX_COLORS; i++) {
    if (i < paletteSize) {
      colorsSprites[i]->setColor(cc3bFromHexString(colors[i]).unwrapOr(ccWHITE));
      colorsSprites[i]->setVisible(true);
    } else {
      colorsSprites[i]->setVisible(false);
    }
  }
  m_colors->updateLayout();
}