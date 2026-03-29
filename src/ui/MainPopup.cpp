#include <GUI/CCControlExtension/CCScale9Sprite.h>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/ColorAction.hpp>
#include <Geode/binding/ColorSelectPopup.hpp>
#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/cocos/support/CCPointExtension.h>
#include <Geode/loader/Mod.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/ui/Layout.hpp>

#include "../managers/HueMintManager.hpp"
#include "../network/HueMintService.hpp"

#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

class MainPopup : public Popup {
public:
  static MainPopup *create() {
    auto popup = new MainPopup;
    if (popup->init()) {
      popup->autorelease();
      return popup;
    }
    delete popup;
    return nullptr;
  }

protected:
  HueMintManager &manager = HueMintManager::get();
  HueMintService &service = HueMintService::get();
  std::array<ColorChannelSprite *, HueMintManager::MAX_COLORS> m_colorChannels;
  std::array<CCMenuItemSpriteExtra *, HueMintManager::MAX_COLORS> m_colorButtons;
  ButtonSprite* m_generateBtn;
  CCLabelBMFont* m_infoLabel;
  CCLabelBMFont* m_modeLabel;
  CCLabelBMFont* m_presetLabel;
  TextInput* m_colorsInput;
  TextInput* m_temperatureInput;
  TextInput* m_resultsInput;
  CCMenu* m_modesMenu;
  CCMenu* m_navMenu;
  CCMenu* m_colorsMenu;
  CCMenu* m_lockMenu;
  bool m_isLoaded = false;

  bool init() {
    if (!Popup::init(440.f, 260.f)) return false;

    const char *infoIconName = "GJ_infoIcon_001.png";
    const char *backgroundSpriteName = "square02b_001.png";
    const char *bigFontName = "bigFont.fnt";
    const char *goldFontName = "goldFont.fnt";

    m_isLoaded = (HueMintService::m_currentPaletteResult.items != 0);

    // initialize color channels
    for (size_t i = 0; i < HueMintManager::MAX_COLORS; i++) {
      auto colorSprite = ColorChannelSprite::create();
      colorSprite->setScale(0.8f);
      m_colorChannels.at(i) = colorSprite;
    }

    // @geode-ignore(unknown-resource)
    auto resetBtnSprite = CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("geode.loader/reload-gold.png"),
        CircleBaseColor::Green, CircleBaseSize::Tiny);

    auto saveBtnSprite = CircleButtonSprite::create(
        CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png"),
        CircleBaseColor::Green, CircleBaseSize::Tiny);

    // @geode-ignore(unknown-resource)
    auto settingsBtnSprite = CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("geode.loader/settings.png"),
        CircleBaseColor::Green, CircleBaseSize::Tiny);

    CCMenu *optionsMenu = CCMenu::create(
        CCMenuItemSpriteExtra::create(resetBtnSprite, this, menu_selector(MainPopup::onReset)),
        CCMenuItemSpriteExtra::create(saveBtnSprite, this,menu_selector(MainPopup::onSave)),
        CCMenuItemSpriteExtra::create(settingsBtnSprite, this,menu_selector(MainPopup::onSettings)),
        nullptr);

    m_mainLayer->addChildAtPosition(optionsMenu, Anchor::TopRight,ccp(-20.f, -20.f));
    optionsMenu->setAnchorPoint(ccp(1.f, 0.5f));
    optionsMenu->setContentSize({70.f, 50.f});

    optionsMenu->setLayout(
        RowLayout::create()
            ->setGap(0.5f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setCrossAxisOverflow(false)
            ->setAutoScale(false));

    CCScale9Sprite *colorsBG = cocos2d::extension::CCScale9Sprite::create(
        backgroundSpriteName, {0.0f, 0.0f, 80.0f, 80.0f});
    m_mainLayer->addChildAtPosition(colorsBG, Anchor::Center, ccp(0.f, 50.f));
    colorsBG->setContentSize({400.f, 50.f});
    colorsBG->setColor({130, 64, 33});
    colorsBG->setZOrder(1);
    colorsBG->setID("colorsBG"_spr);

    CCScale9Sprite *settingsBG = cocos2d::extension::CCScale9Sprite::create(
        backgroundSpriteName, {0.0f, 0.0f, 80.0f, 80.0f});
    m_mainLayer->addChildAtPosition(settingsBG, Anchor::Center,ccp(0.f, -60.f));
    settingsBG->setContentSize({400.f, 100.f});
    settingsBG->setColor({130, 64, 33});
    settingsBG->setZOrder(1);
    settingsBG->setID("settingsBG"_spr);

    CCLabelBMFont *settingsLabel = CCLabelBMFont::create("Settings", goldFontName);
    settingsBG->addChildAtPosition(settingsLabel, Anchor::TopLeft,ccp(10.f, 7.5f));
    settingsLabel->setScale(0.5f);
    settingsLabel->setAnchorPoint(ccp(0.f, 1.f));

    m_colorsMenu = CCMenu::create();
    m_colorsMenu->setContentSize(ccp(400.f, 100.f));
    colorsBG->addChildAtPosition(m_colorsMenu, Anchor::Center, ccp(0.f, -8.f));
    m_colorsMenu->setLayout(
        RowLayout::create()
                ->setGap(0.5f)
                ->setGrowCrossAxis(true)
                ->setAxisAlignment(AxisAlignment::Even)
                ->setCrossAxisLineAlignment(AxisAlignment::Center)
                ->setCrossAxisOverflow(false)
                ->setAutoScale(false));

    m_lockMenu = CCMenu::create();
    m_lockMenu->setContentSize(ccp(400.f, 20.f));
    colorsBG->addChildAtPosition(m_lockMenu, Anchor::Center, ccp(0.f,15.f));
    m_lockMenu->setLayout(
        RowLayout::create()
            ->setGap(0.5f)
            ->setGrowCrossAxis(true)
            ->setAxisAlignment(AxisAlignment::Even)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setCrossAxisOverflow(false)
            ->setAutoScale(false));

    for (size_t i = 0; i < HueMintManager::MAX_COLORS; i++) {
      auto label = CCLabelBMFont::create((std::to_string(i + 1)).c_str(), bigFontName);
      auto color = m_colorChannels.at(i);
      auto item = CCMenuItemSpriteExtra::create(
          color, this, menu_selector(MainPopup::onColorChannel));

      auto lockSpr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
      lockSpr->setScale(0.5f);
      auto lockBtn = CCMenuItemSpriteExtra::create(lockSpr, lockSpr, this, menu_selector(MainPopup::onLockColorChannel));
          lockBtn->setUserObject(CCInteger::create(i));
        
      m_colorButtons[i] = item;
      item->addChildAtPosition(label, Anchor::Top, ccp(0.f, -10.f));
      m_lockMenu->addChild(lockBtn);
      m_colorsMenu->addChild(item);
      label->setScale(0.35f);
    }

    // updates color channels and buttons based on the current settings
    for (size_t i = manager.getRequest().num_colors; i < HueMintManager::MAX_COLORS; i++) {
      updateColorsButton(i, false);
    }

    CCMenu* mainControls = CCMenu::create();
    m_navMenu = CCMenu::create();

    mainControls->setContentSize(ccp(400.f, 30.f));
    m_navMenu->setContentSize(ccp(60.f, 30.f));
    colorsBG->addChildAtPosition(mainControls, Anchor::Center, ccp(0.f, -40.f));
    colorsBG->addChildAtPosition(m_navMenu, Anchor::Center, ccp(70.f, -40.f));

    m_navMenu->setLayout(
        RowLayout::create()
            ->setGap(5.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setCrossAxisOverflow(false)
            ->setAutoScale(true));

    mainControls->setLayout(
        RowLayout::create()
            ->setGap(20.f)
            ->setAxisAlignment(AxisAlignment::End)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setCrossAxisOverflow(false)
            ->setAutoScale(true));


    m_generateBtn = ButtonSprite::create("Generate");
    m_generateBtn->setScale(0.7f);

    CCSprite* prevSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    CCSprite* nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    CCMenuItemSpriteExtra* m_prev = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(MainPopup::onPrevPalette));
    CCMenuItemSpriteExtra* m_next = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(MainPopup::onNextPalette));

    m_navMenu->setEnabled(false);
    m_navMenu->setVisible(false);

    prevSprite->setScale(0.6f);
    nextSprite->setScale(0.6f);
    nextSprite->setFlipX(true);

    m_navMenu->addChild(CCMenuItemSpriteExtra::create(m_prev, this, menu_selector(MainPopup::onPrevPalette)));
    m_navMenu->addChild(CCMenuItemSpriteExtra::create(m_next, this, menu_selector(MainPopup::onNextPalette)));
    mainControls->addChild(CCMenuItemSpriteExtra::create(m_generateBtn, this, menu_selector(MainPopup::onGeneratePalette)));

    m_infoLabel = CCLabelBMFont::create("",bigFontName);
    m_infoLabel->setScale(0.4f);
    m_infoLabel->setAnchorPoint(ccp(0.f, 0.5f));
    colorsBG->addChildAtPosition(m_infoLabel, Anchor::BottomLeft,ccp(0.f, -10.5f));

    m_colorsInput = TextInput::create(40.f, "6", bigFontName);
    m_colorsInput->setLabel("Colors");
    m_colorsInput->setString(std::to_string(manager.getRequest().num_colors).c_str());
    m_colorsInput->setAnchorPoint(ccp(0.f, 0.5f));
    m_colorsInput->setCommonFilter(CommonFilter::Int);
    m_colorsInput->setMaxCharCount(2);
    m_colorsInput->setCallback([this](gd::string input) {
        this->onColorsInput(input);
    });

    m_temperatureInput = TextInput::create(40.f, "1.3", bigFontName);
    m_temperatureInput->setLabel("Temp");
    m_temperatureInput->setString(std::to_string(manager.getRequest().temperature).erase(3).c_str());
    m_temperatureInput->setAnchorPoint(ccp(0.f, 0.5f));
    m_temperatureInput->setCommonFilter(CommonFilter::Float);
    m_temperatureInput->setMaxCharCount(3);
    m_temperatureInput->setCallback([this](gd::string input) {
        this->onTemperatureInput(input);
    });

    settingsBG->addChildAtPosition(m_colorsInput, Anchor::TopLeft, ccp(10.f, -35.f));
    settingsBG->addChildAtPosition(m_temperatureInput, Anchor::TopLeft, ccp(60.f, -35.f));

    CCLabelBMFont* modeLabel = CCLabelBMFont::create("Mode", goldFontName);
    modeLabel->setScale(0.35f);
    modeLabel->setAnchorPoint(ccp(0.f, 0.5f));

    m_modeLabel = CCLabelBMFont::create(manager.getRequest().mode.c_str(), bigFontName);
    m_modeLabel->setScale(0.5f);
  
    CCSprite* prevModeSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCSprite* nextModeSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");

    prevModeSpr->setScale(0.6f);
    nextModeSpr->setScale(0.6f);
    nextModeSpr->setFlipX(true);

    CCMenu* modeMenu = CCMenu::create(
      CCMenuItemSpriteExtra::create(prevModeSpr, this, menu_selector(MainPopup::onprevMode)),
      m_modeLabel,
      CCMenuItemSpriteExtra::create(nextModeSpr, this, menu_selector(MainPopup::onNextMode)),
      nullptr);

    modeMenu->setAnchorPoint(ccp(0.f, 0.5f));
    modeMenu->setContentSize({160.f, 60.f});
    modeMenu->setLayout(
        RowLayout::create()
            ->setGap(0.5f)
            ->setAxisAlignment(AxisAlignment::Between)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setCrossAxisOverflow(false)
            ->setAutoScale(false));

    settingsBG->addChildAtPosition(modeLabel, Anchor::TopLeft, ccp(135.f, -13.f));
    settingsBG->addChildAtPosition(modeMenu, Anchor::TopLeft, ccp(110.f, -30.f));

    CCLabelBMFont* presetLabel = CCLabelBMFont::create("Preset", goldFontName);
    presetLabel->setScale(0.35f);
    presetLabel->setAnchorPoint(ccp(0.f, 0.5f));

    m_presetLabel = CCLabelBMFont::create(manager.getRequest().preset.c_str(), bigFontName);
    m_presetLabel->setScale(0.4f);
  
    CCSprite* prevPresetSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    CCSprite* nextPresetSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");

    prevPresetSpr->setScale(0.6f);
    nextPresetSpr->setScale(0.6f);
    nextPresetSpr->setFlipX(true);

    CCMenu* presetMenu = CCMenu::create(
      CCMenuItemSpriteExtra::create(prevPresetSpr, this, menu_selector(MainPopup::onprevPreset)),
      m_presetLabel,
      CCMenuItemSpriteExtra::create(nextPresetSpr, this, menu_selector(MainPopup::onNextPreset)),
      nullptr);

    presetMenu->setAnchorPoint(ccp(0.f, 0.5f));
    presetMenu->setContentSize({160.f, 60.f});
    presetMenu->setLayout(
        RowLayout::create()
            ->setGap(0.5f)
            ->setAxisAlignment(AxisAlignment::Between)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setCrossAxisOverflow(false)
            ->setAutoScale(false));

    settingsBG->addChildAtPosition(presetLabel, Anchor::TopLeft, ccp(135.f, -60.f));
    settingsBG->addChildAtPosition(presetMenu, Anchor::TopLeft, ccp(110.f, -75.f));

    m_resultsInput = TextInput::create(40.f, "10", bigFontName);
    m_resultsInput->setLabel("Results");
    m_resultsInput->setString(std::to_string(manager.getRequest().num_results).c_str());
    m_resultsInput->setAnchorPoint(ccp(0.f, 0.5f));
    m_resultsInput->setCommonFilter(CommonFilter::Int);
    m_resultsInput->setMaxCharCount(2);
    m_resultsInput->setCallback([this](gd::string input) {
        this->onResultsInput(input);
    });

    settingsBG->addChildAtPosition(m_resultsInput, Anchor::TopLeft, ccp(10.f, -80.f));

    if (m_isLoaded) loadLastState();

    optionsMenu->updateLayout();
    m_colorsMenu->updateLayout();
    m_navMenu->updateLayout();
    mainControls->updateLayout();
    m_lockMenu->updateLayout();
    return true;
  }

  void loadLastState() {
    updateColorChannels(
        HueMintService::m_currentPaletteResult.response.results.at(
            HueMintService::m_currentPaletteResult.currentItem));

    updateInfoLabel();
    updateNavigationButtons();

    for (int i = 0; i < manager.getRequest().num_colors; i++) {
      updateLockButton(i, manager.isColorLocked(i));
    }
  }

  void onColorChannel(CCObject *sender) {
    auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
    auto colorSprite =
        static_cast<ColorChannelSprite *>(item->getNormalImage());

    auto color = static_cast<ccColor3B>(colorSprite->getColor());
    auto popup = ColorSelectPopup::create({0, 0, 0});
    popup->m_colorPicker->setColorTarget(colorSprite);
    popup->m_colorPicker->setColorValue(color);
    popup->show();
    return;
  }

  void onReset(CCObject *) {
    geode::createQuickPopup(
        "Reset all settings",
        "Are you sure you want to reset all settings?",
        "Cancel", "Reset",
        [this](auto, bool btn2) {
          if (btn2) {
            handleReset();
          }
        });
  }

  void onSave(CCObject *) {
    m_colorChannels.at(0)->getParent();
  }

  void onSettings(CCObject *) {m_colorsMenu->removeAllChildren();}

  void onGeneratePalette(CCObject *) {
    m_generateBtn->setString("Loading...");
    service.request([weak = geode::WeakRef(this)](Palette result) {
      if (auto self = weak.lock()) {
        if (!result.colors.empty()) {
          self->m_isLoaded = true;
          self->updateColorChannels(result);
          self->updateInfoLabel();
          self->updateNavigationButtons();
          self->updateFields();
          self->m_generateBtn->setString("Generate");
        } else {
          FLAlertLayer::create(
              "Error", "Failed to generate palette. Please try again.", "OK")
              ->show();
        }
      }
    });
  }

  void onNextPalette(CCObject *) {
    updateColorChannels(manager.getNextPalette());
    updateInfoLabel();
  }

  void onPrevPalette(CCObject *) {
    updateColorChannels(manager.getPrevPalette());
    updateInfoLabel();
  }

  void onNextMode(CCObject *) {
    m_modeLabel->setString(manager.setMode(true).c_str());
  }

  void onprevMode(CCObject *) {
    m_modeLabel->setString(manager.setMode(false).c_str());
  }

  void onprevPreset(CCObject *) {
    m_presetLabel->setString(manager.setPreset(false).c_str());
  }

  void onNextPreset(CCObject *) {
    m_presetLabel->setString(manager.setPreset(true).c_str());
  }

  void onLockColorChannel(CCObject *sender) {
    auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
    int index = static_cast<CCInteger *>(item->getUserObject())->getValue();
    auto color = m_colorChannels.at(index)->getColor();

    manager.toggleColorLock(index, ColorSelectPopup::colorToHex(color));
    updateLockButton(index, manager.isColorLocked(index));
  }

  void onColorsInput(gd::string input) {
    int value = static_cast<int>(std::strtol(input.c_str(), nullptr, 10));
    if (value < HueMintManager::MIN_COLORS || value > HueMintManager::MAX_COLORS) return;

    for (int i = 0; i < HueMintManager::MAX_COLORS; i++) {
      bool show = i < value; 
      updateColorsButton(i, show);
    }

    manager.setMaxColors(value);
    m_colorsMenu->updateLayout();
    m_lockMenu->updateLayout();
  }

  void onResultsInput(gd::string input) {
    int value = static_cast<int>(std::strtol(input.c_str(), nullptr, 10));
    manager.setNumResults(value);
  }

  void onTemperatureInput(gd::string input) {
    float value = static_cast<float>(std::strtof(input.c_str(), nullptr));
    if (value < 0 || value > 2.4) return;

    manager.setTemperature(value);
   }

  void updateColorChannels(Palette palette) {
    for (size_t i = 0; i < palette.colors.size(); i++) {
      std::string hexColor = palette.colors.at(i);
      hexColor.erase(0, 1); // Removes the '#' character
      m_colorChannels.at(i)->setColor(ColorSelectPopup::hexToColor(hexColor));
    }
  }

  void updateInfoLabel() {
    m_infoLabel->setString(
        fmt::format("Results: {} - {}",
                    HueMintService::m_currentPaletteResult.currentItem + 1,
                    HueMintService::m_currentPaletteResult.items)
            .c_str());
  }

  void updateColorsButton(int index, bool show) {
    CCMenuItemSpriteExtra *btn = m_colorButtons[index];
    CCMenuItemSpriteExtra *lockBtn = static_cast<CCMenuItemSpriteExtra *>(m_lockMenu->getChildByIndex(index));
    btn->setVisible(show);
    lockBtn->setVisible(show);

    if (!show) {
      updateLockButton(index, false);
      m_colorChannels.at(index)->setColor({255, 255, 255});
    }
  }

  void updateNavigationButtons() {
    m_navMenu->setEnabled(m_isLoaded);
    m_navMenu->setVisible(m_isLoaded);
  }

  void updateLockButton(int index, bool locked) {
    auto lockBtn = static_cast<CCMenuItemSpriteExtra *>(m_lockMenu->getChildByIndex(index));
    const char *frame = locked ? "GJ_lock_001.png" : "GJ_lock_open_001.png";
    static_cast<CCSprite *>(lockBtn->getNormalImage())->setDisplayFrame(
        CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame));
  }

  void updateFields(){
    m_colorsInput->setString(std::to_string(manager.getRequest().num_colors).c_str());
    m_temperatureInput->setString(std::to_string(manager.getRequest().temperature).erase(3).c_str());

    int results = (HueMintService::m_currentPaletteResult.items == 0) ? 10 : HueMintService::m_currentPaletteResult.items;
    m_resultsInput->setString(std::to_string(results).c_str());
  }

  void handleReset() {
    m_isLoaded = false;
    manager.reset();
    service.resetPalette();
    updateNavigationButtons();
    updateFields();

    for (int i = 0; i < HueMintManager::MAX_COLORS; i++) {
      m_colorChannels.at(i)->setColor({255, 255, 255});
      updateLockButton(i, false);
      updateColorsButton(i, i < manager.getRequest().num_colors);
    }

    m_presetLabel->setString(manager.getRequest().preset.c_str());
    m_modeLabel->setString(manager.getRequest().mode.c_str());
    m_infoLabel->setString("");

    m_colorsMenu->updateLayout();
    m_lockMenu->updateLayout();
  }
};
