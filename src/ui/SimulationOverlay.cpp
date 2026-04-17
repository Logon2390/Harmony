#include "../managers/SimulationManager.hpp"
#include "../managers/SettingsManager.hpp"
#include "../network/HueMintService.hpp"
#include "../builders/SpriteBuilder.hpp"

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
    m_isVisible = !m_isVisible;

    m_menu->setEnabled(m_isVisible);
    this->setVisible(m_isVisible);
    updateNavigationButtons();
    updateInfoLabel();
  }

protected:
  SimulationManager &simulation = SimulationManager::get();
  SettingsManager &settings = SettingsManager::get();
  HueMintService &service = HueMintService::get();
  CCLabelBMFont *m_label;
  CCMenu *m_menu;
  CCMenuItemSpriteExtra* m_prev;
  CCMenuItemSpriteExtra* m_next;
  CCMenuItemSpriteExtra* m_visibilityBtn;
  CCMenuItemSpriteExtra* m_shuffleBtn;
  const float width = 300.f;
  const float height = 20.f;
  bool m_isVisible = false;
  bool m_isHidden = false;

  bool init() {
    if (!this->initWithFile(SpriteBuilder::backgroundSpriteName, {0.0f, 0.0f, 80.0f, 80.0f}, {})) return false;

    this->setContentSize({width, height});
    this->setColor({ 0, 0, 0 });
    this->setOpacity(150);
    this->setVisible(m_isVisible);
    this->setID("simulation-overlay"_spr);

    m_menu = CCMenu::create();
    m_menu->setContentSize({width, height});
    this->addChildAtPosition(m_menu, Anchor::Center);

    m_next = CCMenuItemSpriteExtra::create(SpriteBuilder::createArrow(true, 0.4f), this, menu_selector(SimulationOverlay::onNext));
    m_menu->addChildAtPosition(m_next, Anchor::Right, ccp(-20.f, 0.f));

    m_prev = CCMenuItemSpriteExtra::create(SpriteBuilder::createArrow(false, 0.4f), this, menu_selector(SimulationOverlay::onPrev));
    m_menu->addChildAtPosition(m_prev, Anchor::Right, ccp(-50.f, 0.f));

    m_label = CCLabelBMFont::create("", SpriteBuilder::bigFontName);
    m_label->setScale(0.35f);
    m_label->setAnchorPoint({ 0.f, 0.5f });
    this->addChildAtPosition(m_label, Anchor::Left, ccp(10.f, 0.f));

    auto shuffleSpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("icon.png"_spr), EditorBaseColor::LightBlue);
    shuffleSpr->setScale(0.5f);

    auto visibilitySpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName(SpriteBuilder::hideSprName), EditorBaseColor::DarkGray);
    visibilitySpr->setScale(0.5f);

    m_visibilityBtn = CCMenuItemSpriteExtra::create(visibilitySpr, this, menu_selector(SimulationOverlay::onVisibilityToggle));
    m_menu->addChildAtPosition(m_visibilityBtn, Anchor::Left, ccp(100.f, 0.f));

    m_shuffleBtn = CCMenuItemSpriteExtra::create(shuffleSpr, this, menu_selector(SimulationOverlay::onShuffle));
    m_menu->addChildAtPosition(m_shuffleBtn, Anchor::Left, ccp(80.f, 0.f));

    updateInfoLabel(); 
    m_menu->updateLayout();
    return true;
  }

  void onNext(CCObject*) {
    settings.getNextPalette();
    simulation.replace();
    updateInfoLabel();
    updateNavigationButtons();
  }

  void onPrev(CCObject*) {
    settings.getPrevPalette();
    simulation.replace();
    updateInfoLabel();
    updateNavigationButtons();
  }

  void onShuffle(CCObject*) {
    settings.shufflePalette();
    simulation.replace();
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
    int totalItems = service.getPalettePool().palettes.size();
    int prevOpacity = currentIndex > 0 ? 255 : 200;
    int nextOpacity = currentIndex < totalItems - 1 ? 255 : 200;

    m_prev->setOpacity(prevOpacity);
    m_next->setOpacity(nextOpacity);
    m_prev->setEnabled(currentIndex > 0);
    m_next->setEnabled(currentIndex < totalItems - 1);
    m_prev->setVisible(totalItems > 1);
    m_next->setVisible(totalItems > 1);
  }

  void updateInfoLabel() {
    m_label->setString(
        fmt::format("P: {} - {}",
            service.getPalettePool().currentItem + 1,
            service.getPalettePool().palettes.size())
        .c_str()
    );
  }
};