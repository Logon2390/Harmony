#include "../network/HueMintService.hpp"
#include "../managers/SimulationManager.hpp"
#include "../managers/SettingsManager.hpp"
#include "../builders/SpriteBuilder.hpp"
#include "../utils/ColorUtils.hpp"

using namespace geode::prelude;

class SimulationSetupPopup : public Popup {
public:
  std::function<void()> onColorSelect = []() {};
  static SimulationSetupPopup *create(int colorID) {
    auto popup = new SimulationSetupPopup();
    if (popup->init(colorID)) {
      popup->autorelease();
      return popup;
    }
    delete popup;
    return nullptr;
  }

protected:
  SimulationManager &simulation = SimulationManager::get();
  SettingsManager &settings = SettingsManager::get();
  HueMintService &service = HueMintService::get();
  ColorUtils &utils = ColorUtils::get();
  Ref<CCArray> m_colorButtons;
  CCMenu* m_navMenu;
  CCMenu* m_colorsMenu;
  CCMenuItemSpriteExtra* m_prev;
  CCMenuItemSpriteExtra* m_next;
  CCMenuItemSpriteExtra* m_reset;
  CCMenuItemSpriteExtra* m_resetAll;
  CCMenuItemSpriteExtra* m_mode;
  const int COLORS_COUNT = simulation.getMaxColorCount();
  const int MAX_COLORS = settings.MAX_COLORS;
  const float width = 440.f;
  const float height = 260.f;
  const float cropWidth = width - 20.f;
  bool m_isSpecialColors = false;
  int selectedColorID = 0;

  bool init(int colorID) {
    if (!Popup::init(width, height)) return false;
    this->setTitle(fmt::format("Setup Color channel {}", colorID).c_str());
    selectedColorID = colorID;
    m_colorButtons = CCArray::createWithCapacity(MAX_COLORS);

    RowLayout* mainLayout = RowLayout::create();
    mainLayout->setGap(0.f)
      ->setAxisAlignment(AxisAlignment::Center)
      ->setCrossAxisLineAlignment(AxisAlignment::Center)
      ->setCrossAxisOverflow(false)
      ->setAutoScale(false);

    NineSlice* setupBG = NineSlice::create(SpriteBuilder::backgroundSprName, {0.0f, 0.0f, 80.0f, 80.0f});
    m_mainLayer->addChildAtPosition(setupBG, Anchor::Center, ccp(0.f, -50.f));
    setupBG->setContentSize({cropWidth, 130.f});
    setupBG->setColor({130, 64, 33});
    setupBG->setZOrder(1);

    NineSlice* optsBG = NineSlice::create(SpriteBuilder::backgroundSprName, {0.0f, 0.0f, 80.0f, 80.0f});
    m_mainLayer->addChildAtPosition(optsBG, Anchor::Center, ccp(0.f, 45.f));
    optsBG->setContentSize({cropWidth, 50.f});
    optsBG->setColor({130, 64, 33});
    optsBG->setZOrder(1);

    m_colorsMenu = CCMenu::create();
    m_colorsMenu->setZOrder(2);
    m_colorsMenu->setAnchorPoint(ccp(0.5f, 1.f));
    m_colorsMenu->setContentSize(ccp(cropWidth, 50.f));
    m_mainLayer->addChildAtPosition(m_colorsMenu, Anchor::Top, ccp(0.f, -35.f));
    m_colorsMenu->setLayout(mainLayout);

    m_navMenu = CCMenu::create();
    m_navMenu->setContentSize(ccp(60.f, 30.f));
    m_navMenu->setScale(0.8f);

    optsBG->addChildAtPosition(m_navMenu, Anchor::Center, ccp(85.f, -12.5f));

    for (int i = 0; i < MAX_COLORS; i++) {
      CCLabelBMFont* label = CCLabelBMFont::create((std::to_string(i + 1)).c_str(), SpriteBuilder::bigFontName);
      label->setZOrder(3);
      label->setScale(0.3f);

      CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(CCSprite::create(), this, menu_selector(SimulationSetupPopup::onColorSetup));
      m_colorButtons->addObject(item);

      item->m_scaleMultiplier = 1.f;
      item->setTag(i);
      item->setNormalImage(SpriteBuilder::createColorSpr(item, i, COLORS_COUNT));
      item->setContentSize({cropWidth / COLORS_COUNT, 50.f});
      item->getNormalImage()->setContentSize({cropWidth / COLORS_COUNT, 50.f});
      item->setVisible(i < COLORS_COUNT);
      item->addChildAtPosition(label, Anchor::TopRight, ccp(-10.f, -10.f));
      item->updateSprite();
      m_colorsMenu->addChild(item);
    }

    CCSprite* prevSprite = SpriteBuilder::createArrow(ArrowSprite::Cyan);
    CCSprite* nextSprite = SpriteBuilder::createArrow(ArrowSprite::Cyan, true);
    m_prev = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(SimulationSetupPopup::onPrevPalette));
    m_next = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(SimulationSetupPopup::onNextPalette));
    m_prev->setVisible(service.getPoolSize() > 1);
    m_next->setVisible(service.getPoolSize() > 1);
    m_prev->setEnabled(service.getPoolSize() > 1);
    m_next->setEnabled(service.getPoolSize() > 1);

    m_mode = CCMenuItemSpriteExtra::create(ButtonSprite::create("Special colors"), this, menu_selector(SimulationSetupPopup::onModeChange));
    m_mode->setAnchorPoint(ccp(0.f, 0.5f));
    m_mode->setScale(0.6f);
    m_mode->m_baseScale = 0.6f;
    m_mode->m_scaleMultiplier = 1.1f;

    m_resetAll = CCMenuItemSpriteExtra::create(ButtonSprite::create("Reset all"), this, menu_selector(SimulationSetupPopup::onResetAll));
    m_resetAll->setAnchorPoint(ccp(0.f, 0.5f));
    m_resetAll->setScale(0.6f);
    m_resetAll->setVisible(simulation.getModifiedColors() > 0);
    m_resetAll->setEnabled(simulation.getModifiedColors() > 0);
    m_resetAll->m_baseScale = 0.6f;
    m_resetAll->m_scaleMultiplier = 1.1f;

    m_reset = CCMenuItemSpriteExtra::create(ButtonSprite::create("Reset"), this, menu_selector(SimulationSetupPopup::onReset));
    m_reset->setAnchorPoint(ccp(0.f, 0.5f));
    m_reset->setScale(0.6f);
    m_reset->setVisible(simulation.isColorSetup(selectedColorID));
    m_reset->setEnabled(simulation.isColorSetup(selectedColorID));
    m_reset->m_baseScale = 0.6f;
    m_reset->m_scaleMultiplier = 1.1f;

    m_navMenu->addChildAtPosition(m_prev, Anchor::Center, ccp(110.f, 0.f));
    m_navMenu->addChildAtPosition(m_next, Anchor::Center, ccp(140.f, 0.f));
    m_navMenu->addChildAtPosition(m_mode, Anchor::Center, ccp(-360.f, 0.f));
    m_navMenu->addChildAtPosition(m_resetAll, Anchor::Center, ccp(-220.f, 0.f));
    m_navMenu->addChildAtPosition(m_reset, Anchor::Center, ccp(-120.f, 0.f));

    updateColorSprites(settings.getCurrentPalette().colors);
    m_colorsMenu->updateLayout();
    m_navMenu->updateLayout();
    return true;
  }

  void onResetAll(CCObject*) {
    geode::createQuickPopup(
      "Reset all colors settings",
      "Are you sure you want to reset all colors settings?",
      "Cancel", "Reset", [this](auto, bool btn2) {
        if (btn2) {
          simulation.reset();
        }
      });
  }

  void onReset(CCObject*) {
    geode::createQuickPopup(
      "Reset color channel setup",
      "Are you sure you want to reset this color channel setup?",
      "Cancel", "Reset", [this](auto, bool btn2) {
        if (btn2) {
          simulation.remove(selectedColorID);
          Notification::create(fmt::format("Color channel {} setup removed", selectedColorID).c_str(), NotificationIcon::Info)->show();
        }
      });
  }

  void onModeChange(CCObject*) {
    m_isSpecialColors = !m_isSpecialColors;
    updateModeButton();
    updateControlsVisibility(!m_isSpecialColors);
  }

  void onPrevPalette(CCObject*) {
    updateColorSprites(settings.getPrevPalette().colors);
    updateNavigationButtons();
  }

  void onNextPalette(CCObject*) {
    updateColorSprites(settings.getNextPalette().colors);
    updateNavigationButtons();
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
    m_prev->setVisible(totalItems > 1);
    m_next->setVisible(totalItems > 1);
  }

  void updateModeButton() {
    ButtonSprite* saveSpr = static_cast<ButtonSprite*>(m_mode->getNormalImage());
    saveSpr->setString(m_isSpecialColors ? "Special colors" : "Custom colors");
    saveSpr->updateBGImage(m_isSpecialColors ? "GJ_button_02.png" : "GJ_button_01.png");
  }

  void updateControlsVisibility(bool state) {
    m_prev->setVisible(state);
    m_prev->setEnabled(state);
    m_next->setVisible(state);
    m_next->setEnabled(state);
    m_resetAll->setVisible(state);
    m_resetAll->setEnabled(state);
    m_reset->setVisible(state);
    m_reset->setEnabled(state);
  }

  void updateColorSprites(std::vector<std::string> colors) {
    auto colorButtons =  m_colorButtons->asExt<CCMenuItemSpriteExtra*>();
    NineSlice *colorSpr;
    std::string hex;
    for (size_t i = 0; i < MAX_COLORS; i++) {
      colorSpr = static_cast<NineSlice *>(colorButtons[i]->getNormalImage());
      hex = i < colors.size() ? colors.at(i) : "#FFFFFF";
      utils.applyColorToSprite(colorSpr, hex);
    }
  }

  void updateColorButton(int index, int limit) {
    bool isVisible = index < limit;
    CCMenuItemSpriteExtra *btn = static_cast<CCMenuItemSpriteExtra *>(m_colorButtons->objectAtIndex(index));

    if (isVisible) {
      float width = cropWidth / limit;
      btn->setNormalImage(SpriteBuilder::createColorSpr(btn, index, limit, width, 50.f));
      btn->setContentSize({width, 100.f});
      btn->updateSprite();
      btn->updateLayout();
    }
    btn->setVisible(isVisible);
  }

  void onColorSetup(CCObject* sender) {
    CCMenuItemSpriteExtra* item = static_cast<CCMenuItemSpriteExtra*>(sender);
    int colorIndex = item->getTag();

    if (simulation.isColorSetup(selectedColorID)) {
      int currentColorIndex = simulation.getColorSetup(selectedColorID);
      geode::createQuickPopup(
          "Color already setup",
          fmt::format("This color channel ({}) is already setup with palette color: {}", selectedColorID, currentColorIndex + 1).c_str(),
          "Cancel", "Replace", [this, colorIndex](auto, bool btn2) {
            if (btn2) {
              simulation.setup(selectedColorID, colorIndex);
              Notification::create(fmt::format("Color channel {} setup with color {}", selectedColorID , colorIndex + 1).c_str(), NotificationIcon::Info)->show();
              this->onClose(m_closeBtn);
            }
          }
        );
    } else {
      simulation.setup(selectedColorID, colorIndex);
      Notification::create(fmt::format("Color channel {} setup with color {}", selectedColorID , colorIndex + 1).c_str(), NotificationIcon::Info)->show();
      this->onClose(m_closeBtn);
    }
  }

  void onClose(CCObject* sender) override {
    onColorSelect();
    Popup::onClose(sender);
  }

  std::string formatColorName(int colorID) {
    switch (colorID) {
    case 1000: return "BG";
    case 1001: return "G";
    case 1009: return "G2";
    case 1002: return "LINE";
    case 1013: return "MG";
    case 1014: return "MG2";
    default:
      return utils::numToString(colorID);
    }
  }
};