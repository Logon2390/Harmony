#include "../network/HueMintService.hpp"
#include "../managers/SimulationManager.hpp"
#include "../managers/SettingsManager.hpp"
#include "../builders/SpriteBuilder.hpp"
#include "../utils/ColorUtils.hpp"

using namespace geode::prelude;

class SimulationSetupPopup : public Popup {
public:
  std::function<void()> onColorSelect = []() {};
  static SimulationSetupPopup *create(int colorID, bool specialColors = false) {
    auto popup = new SimulationSetupPopup();
    if (popup->init(colorID, specialColors)) {
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
  Ref<CCArray> m_labels;
  Ref<CCArray> m_colorSetups;
  Ref<CCArray> m_paletteColors;
  CCMenu* m_navMenu;
  CCMenu* m_colorsMenu;
  CCMenuItemSpriteExtra* m_prev;
  CCMenuItemSpriteExtra* m_next;
  CCMenuItemSpriteExtra* m_reset;
  CCMenuItemSpriteExtra* m_resetAll;
  CCMenuItemSpriteExtra* m_mode;
  ScrollLayer* m_setups;
  const int COLORS_COUNT = simulation.getColors();
  const int MAX_COLORS = settings.MAX_COLORS;
  const float width = 440.f;
  const float height = 260.f;
  const float cropWidth = width - 20.f;
  bool m_isSpecialColors = false;
  int selectedColorID = 0;

  bool init(int colorID, bool specialColors = false) {
    if (!Popup::init(width, height)) return false;
    std::string title = specialColors ? "Setup Special Colors" : fmt::format("Setup color channel {}", formatColorName(colorID));
    this->setTitle(title.c_str());

    selectedColorID = colorID;
    m_isSpecialColors = specialColors;
    simulation.m_isSetupStage = true;

    m_colorButtons = CCArray::createWithCapacity(MAX_COLORS);
    m_labels = CCArray::createWithCapacity(MAX_COLORS);
    m_paletteColors = CCArray::createWithCapacity(MAX_COLORS);
    m_colorSetups = CCArray::create();  

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
      m_labels->addObject(label);

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
    m_mode->setVisible(!m_isSpecialColors);
    m_mode->setEnabled(!m_isSpecialColors);
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

    m_setups = ScrollLayer::create({cropWidth - 20.f, 120.f}, true, true);
    m_setups->setZOrder(2);
    m_setups->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

    Scrollbar* scrollbar = Scrollbar::create(m_setups);
    setupBG->addChildAtPosition(m_setups, Anchor::BottomLeft, ccp(10.f, 0.f));
    setupBG->addChildAtPosition(scrollbar, Anchor::Right, ccp(-5.f, 0.f));

    initColorsSetup();
    if (m_isSpecialColors) toggleSpecialColors();
    else updateColorSprites(settings.getCurrentPalette().colors);

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
          handleResetAll();
          Notification::create("All color channel setups removed", NotificationIcon::Info)->show();
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
          handleReset(selectedColorID);
          Notification::create(fmt::format("Color channel {} setup removed", formatColorName(selectedColorID)).c_str(), NotificationIcon::Info)->show();
        }
      });
  }

  void onModeChange(CCObject*) {
    m_isSpecialColors = !m_isSpecialColors;
    toggleSpecialColors();
  }

  void onPrevPalette(CCObject*) {
    updateColorSprites(settings.getPrevPalette().colors);
    updateNavigationButtons();
    updatePaletteColors();
  }

  void onNextPalette(CCObject*) {
    updateColorSprites(settings.getNextPalette().colors);
    updateNavigationButtons();
    updatePaletteColors();
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

  void updateColorButton() {
    auto colorButtons = m_colorButtons->asExt<CCMenuItemSpriteExtra*>();
    int limit = m_isSpecialColors ? simulation.getSpecialColors().size() : COLORS_COUNT;
    for (int i = 0; i < MAX_COLORS; i++) {
      bool isVisible = i < limit;
      CCMenuItemSpriteExtra *btn = colorButtons[i];

      if (isVisible) {
        float width = cropWidth / limit;
        btn->setNormalImage(SpriteBuilder::createColorSpr(btn, i, limit, width, 50.f));
        btn->setContentSize({width, 100.f});
        btn->setTag(m_isSpecialColors ? simulation.getSpecialColorIDs().at(i) : i);
        btn->updateSprite();
        btn->updateLayout();
      }
      btn->setVisible(isVisible);
    }
    m_colorsMenu->updateLayout();
  }

  void updateColorLabels() {
    int limit  = m_isSpecialColors ? simulation.getSpecialColors().size() : COLORS_COUNT;
    auto labels = m_labels->asExt<CCLabelBMFont*>();

    for (int i = 0; i < limit; i++) {
      std::string name = m_isSpecialColors ? formatColorName(simulation.getSpecialColorIDs().at(i)) : std::to_string(i + 1);
      labels[i]->setString(name.c_str());
    }
  }

  void updatePaletteColors() {
    auto paletteColors = m_paletteColors->asExt<ColorChannelSprite*>();
    auto colors = settings.getCurrentPalette().colors;
    for (int i = 0; i < MAX_COLORS; i++) {
      if ( i < colors.size()) {
        utils.applyColorToSprite(paletteColors[i], colors.at(i));
      }
    }
  }

  void updateResetControls() {
    bool isSetup = simulation.isColorSetup(selectedColorID);
    m_reset->setVisible(isSetup);
    m_reset->setEnabled(isSetup);
    m_resetAll->setVisible(simulation.getModifiedColors() > 0);
    m_resetAll->setEnabled(simulation.getModifiedColors() > 0);
  }

  void onColorSetup(CCObject* sender) {
    CCMenuItemSpriteExtra* item = static_cast<CCMenuItemSpriteExtra*>(sender);
    int colorIndex = item->getTag();

    if (!m_isSpecialColors) {
      if (simulation.isColorSetup(selectedColorID)) {
        int currentColorIndex = simulation.getColorSetup(selectedColorID);
        geode::createQuickPopup(
            "Color already setup",
            fmt::format("This color channel ({}) is already setup with palette color: {}", formatColorName(selectedColorID), currentColorIndex + 1).c_str(),
            "Cancel", "Replace", [this, colorIndex](auto, bool btn2) {
              if (btn2) {
                simulation.setup(selectedColorID, colorIndex);
                Notification::create(
                  fmt::format("Color channel {} setup with color {}", formatColorName(selectedColorID), colorIndex + 1).c_str(), 
                  NotificationIcon::Info)->show();
                this->onClose(m_closeBtn);
              }
            }
          );
      } else {
        simulation.setup(selectedColorID, colorIndex);
        Notification::create(
          fmt::format("Color channel {} setup with color {}", formatColorName(selectedColorID), colorIndex + 1).c_str(), 
          NotificationIcon::Info)->show();
        this->onClose(m_closeBtn);
      }
    } else {
      selectedColorID = item->getTag();
      Notification::create(
        fmt::format("Selected color channel {} for special colors setup", formatColorName(selectedColorID)).c_str(),
        NotificationIcon::Info
      )->show();
      m_isSpecialColors = false;
      toggleSpecialColors();
    }
  }

  void toggleSpecialColors() {
    updateModeButton();
    updateControlsVisibility(!m_isSpecialColors);
    updateColorButton();
    updateColorLabels();
    updateColorSprites(m_isSpecialColors ? simulation.getSpecialColors() : settings.getCurrentPalette().colors);
  }

  void onClose(CCObject* sender) override {
    if (m_isSpecialColors) simulation.m_isSetupStage = false;
    onColorSelect();
    Popup::onClose(sender);
  }

  void handleReset(int colorID) {
    auto colorSetups = m_colorSetups->asExt<ColorChannelSprite *>();
    updateResetControls();

    for (ColorChannelSprite *setup : colorSetups) {
      if (setup->getTag() == colorID) {
        setup->removeFromParent();
        m_colorSetups->removeObject(setup);
        m_setups->m_contentLayer->updateLayout();
        break;
      }
    }
  }

  void handleResetAll() {
    auto colorSetups = m_colorSetups->asExt<ColorChannelSprite *>();
    updateResetControls();

    for (ColorChannelSprite *setup : colorSetups) {
      setup->removeFromParent();
    }

    m_colorSetups->removeAllObjects();
    m_setups->m_contentLayer->updateLayout();
  }

  void initColorsSetup() {
    int count = 0;
    auto colors = settings.getCurrentPalette().colors;
    float rowHeight = 110.f / MAX_COLORS;

    for (int i = 0; i < MAX_COLORS; i++) {
        CCNode* setup = CCNode::create();
        setup->setContentSize({ cropWidth - 30.f, rowHeight });
        setup->setLayout(
            RowLayout::create()
            ->setAxisAlignment(AxisAlignment::Start)
            ->setCrossAxisAlignment(AxisAlignment::End)
            ->setAutoScale(false));

        ccColor3B indexColor = i < colors.size()
            ? utils.hexToColor(colors.at(i).erase(0, 1)) : ccWHITE;

        ColorChannelSprite* colorSpr = ColorChannelSprite::create();
        colorSpr->setColor(indexColor);
        colorSpr->setScale(0.5f);
        m_paletteColors->addObject(colorSpr);

        CCLabelBMFont* label = CCLabelBMFont::create(
            std::to_string(i + 1).c_str(), SpriteBuilder::goldFontName);
        label->setScale(0.6f);

        CCSprite* ArrowSprite = SpriteBuilder::createArrow(ArrowSprite::Green, true, 0.4f);
        colorSpr->addChildAtPosition(label, Anchor::Center);
        setup->addChild(colorSpr);
        setup->addChild(ArrowSprite);

        bool hasColors = false;
        for (auto& [colorID, color] : simulation.getColorsByIndex(i)) {
            if (count > 14) {
                CCLabelBMFont* moreLabel = CCLabelBMFont::create(
                    fmt::format("+{}", simulation.getColorsByIndex(i).size() - 16).c_str(),
                    SpriteBuilder::goldFontName);
                moreLabel->setScale(0.5f);
                moreLabel->setTag(-10);
                setup->addChildAtPosition(moreLabel, Anchor::Center);

                //when resetting all colors, we need to remove this label as well
                m_colorSetups->addObject(moreLabel);
                break;
            }

            ColorChannelSprite* colorChannel = ColorChannelSprite::create();
            colorChannel->setColor(color);
            colorChannel->setScale(0.5f);
            colorChannel->setTag(colorID);

            CCLabelBMFont* channelLabel = CCLabelBMFont::create(
                formatColorName(colorID).c_str(), SpriteBuilder::bigFontName);
            channelLabel->setScale(0.5f);
            colorChannel->addChildAtPosition(channelLabel, Anchor::Center);
            setup->addChild(colorChannel);
            m_colorSetups->addObject(colorChannel);
            hasColors = true;
            count++;
        }
        count = 0;
        setup->updateLayout();

        if (hasColors) {
            m_setups->m_contentLayer->addChild(setup);
        }
    }

    int visibleRows = m_setups->m_contentLayer->getChildrenCount();
    float totalHeight = std::max(120.f, (float)visibleRows * rowHeight + 4.f);
    m_setups->m_contentLayer->setContentSize({ cropWidth - 30.f, totalHeight });
    m_setups->m_contentLayer->updateLayout();
    m_setups->moveToTop();
}

  std::string formatColorName(int colorID) {
    switch (colorID) {
    case 1000: return "BG";
    case 1001: return "G";
    case 1009: return "G2";
    case 1002: return "L";
    case 1013: return "MG";
    case 1014: return "MG2";
    default:
      return utils::numToString(colorID);
    }
  }
};