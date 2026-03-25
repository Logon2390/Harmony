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
  bool m_isLoaded = false;
  std::vector<ColorChannelSprite *> m_colorChannels;
  CCLabelBMFont *m_infoLabel;
  CCMenuItemSpriteExtra *m_prev;
  CCMenuItemSpriteExtra *m_next;
  CCMenu* m_navMenu;

  bool init() {
    if (!Popup::init(440.f, 260.f)) return false;

    const char *infoIconName = "GJ_infoIcon_001.png";
    const char *backgroundSpriteName = "square02b_001.png";
    const char *bigFontName = "bigFont.fnt";
    const char *goldFontName = "goldFont.fnt";

    m_isLoaded = (HueMintService::m_currentPaletteResult.items != 0);

    // initialize color channels
    for (size_t i = 0; i < 12; i++) {
      auto colorSprite = ColorChannelSprite::create();
      colorSprite->setScale(0.8f);
      m_colorChannels.push_back(colorSprite);
    }

    auto resetBtnSprite = CircleButtonSprite::create(
        CCSprite::createWithSpriteFrameName("geode.loader/reload-gold.png"),
        CircleBaseColor::Green, CircleBaseSize::Tiny);

    auto saveBtnSprite = CircleButtonSprite::create(
        CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png"),
        CircleBaseColor::Green, CircleBaseSize::Tiny);

    auto settingsBtnSprite = CircleButtonSprite::create(
        CCSprite::createWithSpriteFrameName("geode.loader/settings.png"),
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

    CCMenu *colorsMenu = CCMenu::create();
    colorsMenu->setContentSize(ccp(400.f, 100.f));
    colorsBG->addChildAtPosition(colorsMenu, Anchor::Center);
    colorsMenu->setLayout(
        RowLayout::create()
                ->setGap(0.5f)
                ->setAxisAlignment(AxisAlignment::Even)
                ->setCrossAxisLineAlignment(AxisAlignment::Center)
                ->setCrossAxisOverflow(false)
                ->setAutoScale(false));

    for (size_t i = 0; i < 12; i++) {
      auto colors = m_colorChannels.at(i);
      auto item = CCMenuItemSpriteExtra::create(
          colors, this, menu_selector(MainPopup::onColorChannel));
      colorsMenu->addChild(item);
    }

    CCMenu* mainControls = CCMenu::create();
    m_navMenu = CCMenu::create();

    mainControls->setContentSize(ccp(400.f, 30.f));
    m_navMenu->setContentSize(ccp(30.f, 30.f));
    colorsBG->addChildAtPosition(mainControls, Anchor::Center, ccp(0.f, -40.f));
    colorsBG->addChildAtPosition(m_navMenu, Anchor::Center, ccp(70.f, -40.f));

    m_navMenu->setLayout(
        RowLayout::create()
                ->setGap(5.f)
                ->setAxisAlignment(AxisAlignment::Start)
                ->setCrossAxisLineAlignment(AxisAlignment::Center)
                ->setCrossAxisOverflow(false)
                ->setAutoScale(false));

    mainControls->setLayout(
        RowLayout::create()
            ->setGap(20.f)
            ->setAxisAlignment(AxisAlignment::End)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setCrossAxisOverflow(false)
            ->setAutoScale(true));


    auto generateBtn = ButtonSprite::create("Generate");
    generateBtn->setScale(0.7f);

    CCSprite* prevSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    CCSprite* nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    m_prev = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(MainPopup::onPrevPalette));
    m_next = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(MainPopup::onNextPalette));

    m_navMenu->setEnabled(false);
    m_navMenu->setVisible(false);

    prevSprite->setScale(0.6f);
    nextSprite->setScale(0.6f);
    nextSprite->setFlipX(true);

    m_navMenu->addChild(CCMenuItemSpriteExtra::create(m_prev, this, menu_selector(MainPopup::onPrevPalette)));
    m_navMenu->addChild(CCMenuItemSpriteExtra::create(m_next, this, menu_selector(MainPopup::onNextPalette)));
    mainControls->addChild(CCMenuItemSpriteExtra::create(generateBtn, this, menu_selector(MainPopup::onGeneratePalette)));

    m_infoLabel = CCLabelBMFont::create("",bigFontName);
    m_infoLabel->setScale(0.4f);
    m_infoLabel->setAnchorPoint(ccp(0.f, 0.5f));
    colorsBG->addChildAtPosition(m_infoLabel, Anchor::BottomLeft,ccp(0.f, -10.5f));

    if (m_isLoaded) loadLastState();

    optionsMenu->updateLayout();
    colorsMenu->updateLayout();
    m_navMenu->updateLayout();
    mainControls->updateLayout();


    return true;
  }

  void loadLastState() {
    updateColorChannels(
        HueMintService::m_currentPaletteResult.response.results.at(
            HueMintService::m_currentPaletteResult.currentItem));

    updateInfoLabel();
    updateNavigationButtons();
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
        "Reset Palette",
        "Are you sure you want to reset the palette?",
        "Cancel", "Reset",
        [this](auto, bool btn2) {
          if (btn2) {
            handleReset();
          }
        });
  }

  void onSave(CCObject *) {}

  void onSettings(CCObject *) {}

  void onGeneratePalette(CCObject *) {
    HueMintService::request([this](Palette result) {
      if (!result.colors.empty()) {
        m_isLoaded = true;
        updateColorChannels(result);
        updateInfoLabel();
        updateNavigationButtons();
      } else {
        FLAlertLayer::create(
            "Error", "Failed to generate palette. Please try again.", "OK")
            ->show();
      }
    });
  }

  void onNextPalette(CCObject *) {
    updateColorChannels(HueMintManager::getNextPalette());
    updateInfoLabel();
  }

  void onPrevPalette(CCObject *) {
    updateColorChannels(HueMintManager::getPrevPalette());
    updateInfoLabel();
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

  void updateNavigationButtons() {
    m_navMenu->setEnabled(m_isLoaded);
    m_navMenu->setVisible(m_isLoaded);
  }

  void handleReset() {
    m_isLoaded = false;
    HueMintService::resetPalette();
    updateNavigationButtons();
    for (auto channel : m_colorChannels) {
      channel->setColor({255, 255, 255});
    }
    m_infoLabel->setString("");
  }
};
