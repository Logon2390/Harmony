#include "../managers/SimulationManager.hpp"
#include "../managers/SettingsManager.hpp"
#include "../network/HueMintService.hpp"
#include "../builders/SpriteBuilder.hpp"
#include "../utils/ColorUtils.hpp"
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class SimulationOverlay : public NineSlice {
public:
  static SimulationOverlay *create() {
    auto overlay = new SimulationOverlay();
    if (overlay->init()) {
      overlay->autorelease();
      return overlay;
    }
    delete overlay;
    return nullptr;
  }
  void onToggleVisibility() {
    m_menu->setEnabled(simulation.shouldDisplayOverlay());
    this->setVisible(simulation.shouldDisplayOverlay());
    refresh();
  }
  void refresh() {
    updateUI();
  }

protected:
  SimulationManager &simulation = SimulationManager::get();
  SettingsManager &settings = SettingsManager::get();
  HueMintService &service = HueMintService::get();
  ColorUtils &utils = ColorUtils::get();
  CCLabelBMFont* m_label;
  CCMenu* m_menu;
  CCNode* m_colors;
  Ref<CCArray> m_colorSprites;
  CCMenuItemSpriteExtra* m_prev;
  CCMenuItemSpriteExtra* m_next;
  CCMenuItemSpriteExtra* m_visibilityBtn;
  CCMenuItemSpriteExtra* m_shuffleBtn;
  const float width = 250.f;
  const float height = 15.f;
  bool m_isHidden = false;

  bool init() {
    if (!this->initWithFile(SpriteBuilder::backgroundSmallSprName, {0.0f, 0.0f, 40.0f, 40.0f}, {})) return false;

    this->setContentSize({width, height});
    this->setColor({ 0, 0, 0 });
    this->setOpacity(150);
    this->setVisible(false);
    this->setID("simulation-overlay"_spr);

    m_menu = CCMenu::create();
    m_menu->setContentSize({width, height});
    this->addChildAtPosition(m_menu, Anchor::Center);

    m_next = CCMenuItemSpriteExtra::create(SpriteBuilder::createArrow(ArrowSprite::Pink, true, 0.3f), this, menu_selector(SimulationOverlay::onNext));
    m_menu->addChildAtPosition(m_next, Anchor::Right, ccp(-10.f, 0.f));

    m_prev = CCMenuItemSpriteExtra::create(SpriteBuilder::createArrow(ArrowSprite::Pink, false, 0.3f), this, menu_selector(SimulationOverlay::onPrev));
    m_menu->addChildAtPosition(m_prev, Anchor::Right, ccp(-25.f, 0.f));

    m_label = CCLabelBMFont::create("", SpriteBuilder::bigFontName);
    m_label->setScale(0.3f);
    m_label->setAnchorPoint({ 0.f, 0.5f });
    this->addChildAtPosition(m_label, Anchor::Left, ccp(5.f, 0.f));

    auto shuffleSpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("icon.png"_spr), EditorBaseColor::LightBlue);
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
      CCSprite *colorSpr = CCSprite::create(SpriteBuilder::circleSprName);
      m_colorSprites->addObject(colorSpr);
      colorSpr->setScale(1.2f);
      colorSpr->setVisible(false);
      m_colors->addChild(colorSpr);
    }

    updatePalettePreview();
    updateInfoLabel(); 
    m_menu->updateLayout();
    m_colors->updateLayout();
    return true;
  }

  void onNext(CCObject*) {
    settings.getNextPalette();
    simulation.replace();
    updateUI();
  }

  void onPrev(CCObject*) {
    settings.getPrevPalette();
    simulation.replace();
    updateUI();
  }

  void updateUI() {
    updateNavigationButtons();
    updateInfoLabel();
    updatePalettePreview();
  }

  void onShuffle(CCObject*) {
    settings.shufflePalette();
    simulation.replace();
    updatePalettePreview();
  }

  void onVisibilityToggle(CCObject *) {
    this->setOpacity(m_isHidden ? 150 : 50);
    m_shuffleBtn->setOpacity(m_isHidden ? 255 : 50);
    m_next->setOpacity(m_isHidden ? 255 : 50);
    m_prev->setOpacity(m_isHidden ? 255 : 50);
    m_label->setOpacity(m_isHidden ? 255 : 50);

    m_isHidden = !m_isHidden;
  }

  void updateNavigationButtons() {
    int currentIndex = service.getPalettePool().currentItem;
    int totalItems = service.getPoolSize();
    int prevOpacity = currentIndex > 0 ? 255 : 200;
    int nextOpacity = currentIndex < totalItems - 1 ? 255 : 200;

    m_prev->setOpacity(prevOpacity);
    m_next->setOpacity(nextOpacity);
    m_prev->setEnabled(currentIndex > 0);
    m_next->setEnabled(currentIndex < totalItems - 1);
  }

  void updateInfoLabel() {
    m_label->setString(
        fmt::format("P: {} - {}",
            service.getPalettePool().currentItem + 1,
            service.getPoolSize())
        .c_str()
    );
  }

  void updatePalettePreview() {
    auto colorsSprites = m_colorSprites->asExt<CCSprite*>();
    auto colors = settings.getCurrentPalette().colors;
    int paletteSize = colors.size();

    for (int i = 0; i < settings.MAX_COLORS; i++) {
      if (i < paletteSize) {
        utils.applyColorToSprite(colorsSprites[i], colors[i]);
        colorsSprites[i]->setVisible(true);
      } else {
        colorsSprites[i]->setVisible(false);
      }
    }
    m_colors->updateLayout();
  }
};