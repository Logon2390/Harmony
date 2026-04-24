#include "../managers/SimulationManager.hpp"
#include "../managers/SettingsManager.hpp"
#include "../ui/SimulationSetupPopup.cpp"
#include "../network/HueMintService.hpp"
#include "../builders/SpriteBuilder.hpp"


using namespace geode::prelude;

class SimulationPopup : public Popup {
public:
  static SimulationPopup *create() {
    auto popup = new SimulationPopup();
    if (popup->init()) {
      popup->autorelease();
      return popup;
    }
    delete popup;
    return nullptr;
  }

protected:
  SimulationManager &simulation = SimulationManager::get();
  HueMintService &service = HueMintService::get();
  CCMenu* m_infoMenu;
  TextInput *m_colors;
  const float width = 300.f;
  const float height = 180.f;
  int ColorsCount = simulation.getMaxColorCount();

  bool init() {
    if (!Popup::init(width, height)) return false;

    this->setTitle("Simulation Settings");
    m_infoMenu = static_cast<CCMenu*>(this->m_closeBtn->getParent());

    RowLayout *layout = RowLayout::create();
    layout->setGap(0.5f);
    layout->setAxisAlignment(AxisAlignment::Between);
    layout->setCrossAxisLineAlignment(AxisAlignment::Center);
    layout->setCrossAxisOverflow(false);
    layout->setAutoScale(false);

    RowLayout *buttonLayout = RowLayout::create();
    buttonLayout->setGap(0.5f);
    buttonLayout->setAxisAlignment(AxisAlignment::Center);
    buttonLayout->setCrossAxisLineAlignment(AxisAlignment::Center);
    buttonLayout->setCrossAxisOverflow(false);
    buttonLayout->setAutoScale(false);

    m_colors = TextInput::create(100.f, "6");
    m_colors->setString(geode::utils::numToString(simulation.getMaxColorCount()));
    m_colors->setCommonFilter(CommonFilter::Int);
    m_colors->setMaxCharCount(2);
    m_colors->setScale(0.7f);
    m_colors->setCallback(
        [this](gd::string input) { this->onColorsInput(input); });

    CCMenu *colorsMenu = createMenu(layout);
    colorsMenu->addChild(CCMenuItemSpriteExtra::create(SpriteBuilder::createArrow(ArrowSprite::Green), this, menu_selector(SimulationPopup::onDecreaseColors)));
    colorsMenu->addChild(m_colors);
    colorsMenu->addChild(CCMenuItemSpriteExtra::create(SpriteBuilder::createArrow(ArrowSprite::Green, true), this, menu_selector(SimulationPopup::onIncreaseColors)));
    CCMenuItemSpriteExtra* colorsInfoBtn = CCMenuItemSpriteExtra::create(CCSprite::create(),this, menu_selector(SimulationPopup::onColorsInfo));
    createSelectorRow("Colors", colorsMenu, colorsInfoBtn, 40.f);

    ButtonSprite *specialColorsBtn = ButtonSprite::create("Special Colors");
    specialColorsBtn->setScale(0.7f);

    CCMenu *specialMenu = createMenu(buttonLayout);
    specialMenu->addChild(CCMenuItemSpriteExtra::create(specialColorsBtn, this, menu_selector(SimulationPopup::onSpecialColors)));
    CCMenuItemSpriteExtra* specialInfoBtn = CCMenuItemSpriteExtra::create(CCSprite::create(),this, menu_selector(SimulationPopup::onSpecialColorsInfo));
    createSelectorRow("Special Colors", specialMenu, specialInfoBtn, 10.f);

    ButtonSprite *customColorsBtn = ButtonSprite::create("Custom Colors");
    customColorsBtn->setScale(0.7f);

    CCMenu *customMenu = createMenu(buttonLayout);
    customMenu->addChild(CCMenuItemSpriteExtra::create(customColorsBtn, this, menu_selector(SimulationPopup::onCustomColors)));
    CCMenuItemSpriteExtra* customInfoBtn = CCMenuItemSpriteExtra::create(CCSprite::create(),this, menu_selector(SimulationPopup::onCustomColorsInfo));
    createSelectorRow("Custom Colors", customMenu, customInfoBtn, -20.f);

    ButtonSprite *resetAllBtn = ButtonSprite::create("Reset All");
    resetAllBtn->setScale(0.7f);

    CCMenu *resetAllMenu = createMenu(buttonLayout);
    resetAllMenu->addChild(CCMenuItemSpriteExtra::create(resetAllBtn, this, menu_selector(SimulationPopup::onResetAll)));
    CCMenuItemSpriteExtra* resetAllInfoBtn = CCMenuItemSpriteExtra::create(CCSprite::create(),this, menu_selector(SimulationPopup::onResetAllInfo));
    createSelectorRow("Reset All", resetAllMenu, resetAllInfoBtn, -50.f);

    m_infoMenu->updateLayout();
    return true;
  }

  void onDecreaseColors(CCObject *) {
    if (ColorsCount > SettingsManager::MIN_COLORS) {
      simulation.setColors(--ColorsCount);
      m_colors->setString(geode::utils::numToString(ColorsCount));
    } else {
      ColorsCount = simulation.getMaxColorCount();
      m_colors->setString(geode::utils::numToString(ColorsCount));
    }
  }

  void onIncreaseColors(CCObject *) {
    if (ColorsCount < simulation.getMaxColorCount()) {
      ColorsCount++;
      simulation.setColors(ColorsCount);
      m_colors->setString(geode::utils::numToString(ColorsCount));
    } else {
      ColorsCount = SettingsManager::MIN_COLORS;
      m_colors->setString(geode::utils::numToString(ColorsCount));
    }
  }

  void onColorsInput(gd::string input) {
    auto result = geode::utils::numFromString<int>(input);

    if (result.isOk()) {
      int value = result.unwrap();
      if (value < SettingsManager::MIN_COLORS || value > simulation.getMaxColorCount()) return;
      ColorsCount = value;
      simulation.setColors(ColorsCount);
      m_colors->setString(geode::utils::numToString(ColorsCount));
    }
  }

  void onSpecialColors(CCObject *) {
    if (service.getPoolSize() == 0) {
      FLAlertLayer::create(
        "No palettes loaded",
        "You need to have at least one palette loaded to start setting up custom colors. You can <cy>load a palette from the saved section </c> or <cg>generate a new one. </c>",
        "OK")->show();
      return;
    }
    SimulationSetupPopup::create(0, true)->show();
  }

  void onCustomColors(CCObject *) {
    if (service.getPoolSize() == 0) {
      FLAlertLayer::create(
        "No palettes loaded",
        "You need to have at least one palette loaded to start setting up custom colors. You can <cy>load a palette from the saved section </c> or <cg>generate a new one. </c>",
        "OK")->show();
      return;
    }
    simulation.m_isSetupStage = true;
    GJColorSetupLayer::create(simulation.m_settings)->show();
  }

  void onResetAll(CCObject *) {
    geode::createQuickPopup(
      "Reset all colors settings",
      "Are you sure you want to reset all colors settings?",
      "Cancel", "Reset", [this](auto, bool btn2) {
        if (btn2) {
          simulation.reset();
          Notification::create("All color channel setups removed", NotificationIcon::Info)->show();
        }
      });
  }

  void onColorsInfo(CCObject *) {
    FLAlertLayer::create("Colors info", "text", "OK")->show();
  }

  void onSpecialColorsInfo(CCObject *) {
    FLAlertLayer::create("Default values info", "text", "OK")->show();
  }

  void onCustomColorsInfo(CCObject *) {
    FLAlertLayer::create("Custom colors info", "text", "OK")->show();
  }

  void onResetAllInfo(CCObject *) {
    FLAlertLayer::create("Reset All info", "text", "OK")->show();
  }

  void onBackupsInfo(CCObject *) {
    FLAlertLayer::create("Backups info", "text", "OK")->show();
  }

  CCNode *createSelectorRow(const char *title, CCNode *node, CCMenuItemSpriteExtra *infoBtn, float height) {
    auto bg = geode::NineSlice::create(SpriteBuilder::backgroundSprName, {0, 0, 80, 80});
    bg->setContentSize({(width - 20.f), 25.f});
    bg->setColor({130, 64, 33});

    auto label = CCLabelBMFont::create(title, SpriteBuilder::goldFontName);
    label->setScale(0.5f);
    label->setAnchorPoint({0.f, 0.5f});
    bg->addChildAtPosition(label, Anchor::Left, ccp(10.f, 0.f));

    bg->addChildAtPosition(node, Anchor::Right, ccp(-10.f, 0.f));
    m_mainLayer->addChildAtPosition(bg, Anchor::Center, ccp(0.f, height));

    auto infoSpr = CCSprite::createWithSpriteFrameName(SpriteBuilder::infoIconName);
    infoSpr->setScale(0.5f);

    infoBtn->setSprite(infoSpr);
    infoBtn->setSelectedImage(infoSpr);
    infoBtn->setPosition(label->getPositionX() + label->getScaledContentWidth() + 20.f, bg->getPositionY());
    m_infoMenu->addChild(infoBtn);

    node->updateLayout();
    return bg;
  }

  CCMenu *createMenu(RowLayout *layout) {
    auto menu = CCMenu::create();
    menu->setLayout(layout);
    menu->setScale(0.8f);
    menu->setAnchorPoint({1.f, 0.5f});
    menu->setContentSize({160.f, 60.f});
    return menu;
  }
};