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
#include "SettingsPopup.cpp"

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
  HueMintManager &manager = HueMintManager::get();
  HueMintService &service = HueMintService::get();
  std::array<ColorChannelSprite *, HueMintManager::MAX_COLORS> m_colorChannels;
  std::array<CCMenuItemSpriteExtra *, HueMintManager::MAX_COLORS> m_colorButtons;
  ButtonSprite* m_generateBtn;
  CCLabelBMFont* m_infoLabel;
  
 

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

  }

  void onSave(CCObject *) {
    m_colorChannels.at(0)->getParent();
  }

  void onSettings(CCObject *) {
    SettingsPopup::create()->show();
  }

  void onGeneratePalette(CCObject *) {
    m_generateBtn->setString("Loading...");
    service.request([weak = geode::WeakRef(this)](Palette result) {
      if (auto self = weak.lock()) {
        if (!result.colors.empty()) {
          self->m_isLoaded = true;
          self->updateColorChannels(result);
          self->updateInfoLabel();
          self->updateNavigationButtons();
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



  void onLockColorChannel(CCObject *sender) {
    auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
    int index = static_cast<CCInteger *>(item->getUserObject())->getValue();
    auto color = m_colorChannels.at(index)->getColor();

    manager.toggleColorLock(index, ColorSelectPopup::colorToHex(color));
    updateLockButton(index, manager.isColorLocked(index));
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



  void handleReset() {
    m_isLoaded = false;
    manager.reset();
    service.resetPalette();
    updateNavigationButtons();
    //updateFields();

    for (int i = 0; i < HueMintManager::MAX_COLORS; i++) {
      m_colorChannels.at(i)->setColor({255, 255, 255});
      updateLockButton(i, false);
      updateColorsButton(i, i < manager.getRequest().num_colors);
    }
    m_infoLabel->setString("");

    m_colorsMenu->updateLayout();
    m_lockMenu->updateLayout();
  }
};
