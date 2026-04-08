#include "../managers/SettingsManager.hpp"
#include "../managers/DataManager.hpp"
#include "../network/HueMintService.hpp"
#include "../utils/ColorUtils.hpp"
#include "SettingsPopup.cpp"
#include "HarmonyPopup.cpp"
#include "SavedPopup.cpp"

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
  ColorUtils &utils = ColorUtils::get();
  DataManager &data = DataManager::get();
  SettingsManager &manager = SettingsManager::get();
  HueMintService &service = HueMintService::get();
  std::array<CCMenuItemSpriteExtra *, SettingsManager::MAX_COLORS> m_colorButtons;
  TextInput* m_nameInput;
  LoadingSpinner* m_spinner = nullptr;
  CCLabelBMFont* m_infoLabel;
  CircleButtonSprite* m_generateSpr;
  CCMenuItemSpriteExtra* m_generate;
  CCMenuItemSpriteExtra* m_save;
  CCMenu* m_navMenu;
  CCMenu* m_colorsMenu;
  bool m_isLoaded = false;
  int m_colors = 2;
  int m_swapIndex = -1;
  const float width = 440.f;
  const float height = 260.f;
  const float cropWidth = width - 20.f;

  bool init() {
    if (!Popup::init(width, height)) return false;

    const char *infoIconName = "GJ_infoIcon_001.png";
    const char *backgroundSpriteName = "square02b_001.png";
    const char *bigFontName = "bigFont.fnt";
    const char *goldFontName = "goldFont.fnt";

    m_spinner = LoadingSpinner::create(30.f);
    m_isLoaded = (HueMintService::m_currentPaletteResult.items != 0);
    m_colors = manager.getRequest().num_colors;

    auto resetSpr = CircleButtonSprite::create(
      // @geode-ignore(unknown-resource)
      CCSprite::createWithSpriteFrameName("geode.loader/reload-gold.png"),
      CircleBaseColor::Green, CircleBaseSize::Tiny);

    auto folderSpr = CircleButtonSprite::create(
      CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png"),
      CircleBaseColor::Green, CircleBaseSize::Tiny);

    auto hideSpr = CircleButtonSprite::create(
        CCSprite::createWithSpriteFrameName("hideBtn_001.png"),
        CircleBaseColor::Green, CircleBaseSize::Tiny);

    auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png");
    infoSpr->setScale(0.55f);

    auto settingsSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    settingsSpr->setScale(0.55f);

    CCMenu *optionsMenu = CCMenu::create(
      CCMenuItemSpriteExtra::create(settingsSpr, this,menu_selector(MainPopup::onSettings)),
      CCMenuItemSpriteExtra::create(folderSpr, this,menu_selector(MainPopup::onSave)),
      CCMenuItemSpriteExtra::create(resetSpr, this, menu_selector(MainPopup::onReset)),
      CCMenuItemSpriteExtra::create(hideSpr, this, menu_selector(MainPopup::onHide)),
      CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(MainPopup::onInfo)),
      nullptr);

    RowLayout* mainLayout = RowLayout::create();
    mainLayout->setGap(0.f)
      ->setAxisAlignment(AxisAlignment::Center)
      ->setCrossAxisLineAlignment(AxisAlignment::Center)
      ->setCrossAxisOverflow(false)
      ->setAutoScale(false);

    ColumnLayout* optionsLayout = ColumnLayout::create();
    optionsLayout->setGap(1.f)
      ->setAxisAlignment(AxisAlignment::Even)
      ->setCrossAxisLineAlignment(AxisAlignment::Center)
      ->setCrossAxisOverflow(false)
      ->setAutoScale(true);

    m_mainLayer->addChildAtPosition(optionsMenu, Anchor::TopRight, ccp(-10.f, -20.f));
    optionsMenu->setAnchorPoint(ccp(1.f, 0.5f));
    optionsMenu->setContentSize({200.f, 50.f});
    optionsMenu->setLayout(
        RowLayout::create()
            ->setGap(0.5f)
            ->setAxisAlignment(AxisAlignment::End)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setAxisReverse(true)
            ->setCrossAxisOverflow(false)
            ->setAutoScale(false));

    NineSlice* optsBG = NineSlice::create(backgroundSpriteName, {0.0f, 0.0f, 80.0f, 80.0f});
    m_mainLayer->addChildAtPosition(optsBG, Anchor::Center, ccp(0.f, -10.f));
    optsBG->setContentSize({420.f, 50.f});
    optsBG->setColor({130, 64, 33});
    optsBG->setZOrder(1);

    NineSlice* testModeBG = NineSlice::create(backgroundSpriteName, {0.0f, 0.0f, 80.0f, 80.0f});
    m_mainLayer->addChildAtPosition(testModeBG, Anchor::Bottom, ccp(0.f, 40.f));
    testModeBG->setContentSize({cropWidth, 50.f});
    testModeBG->setColor({130, 64, 33});
    testModeBG->setZOrder(1);

    m_colorsMenu = CCMenu::create();
    m_colorsMenu->setZOrder(2);
    m_colorsMenu->setAnchorPoint(ccp(0.5f, 1.f));
    m_colorsMenu->setContentSize(ccp(cropWidth, 100.f));
    m_mainLayer->addChildAtPosition(m_colorsMenu, Anchor::Top, ccp(0.f, -40.f));
    m_colorsMenu->setLayout(mainLayout);

    for (size_t i = 0; i < SettingsManager::MAX_COLORS; i++) {

      CCLabelBMFont* label = CCLabelBMFont::create((std::to_string(i + 1)).c_str(), bigFontName);
      label->setZOrder(3);
      label->setScale(0.3f);

      CCSprite* lockSpr = CCSprite::createWithSpriteFrameName("GJ_lock_open_001.png");
      CCMenuItemSpriteExtra* lockBtn = CCMenuItemSpriteExtra::create(lockSpr, this, menu_selector(MainPopup::onLockColorChannel));
      lockBtn->setID("lock");
      lockBtn->m_scaleMultiplier = 1.1f;

      CCSprite* infoSpr = CCSprite::createWithSpriteFrameName(infoIconName);
      CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(MainPopup::onColorChannelHarmonies));
      infoBtn->setID("info");
      infoBtn->m_scaleMultiplier = 1.1f;

      CCSprite* swapSpr = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
      CCMenuItemSpriteExtra* swapBtn = CCMenuItemSpriteExtra::create(swapSpr, this, menu_selector(MainPopup::onSwapColorChannel));
      swapBtn->setID("swap");
      swapBtn->m_scaleMultiplier = 1.1f;

      CCMenu* colorMenu = CCMenu::create();
      colorMenu->setID(fmt::format("color-menu-{}", i));
      colorMenu->setScale(0.6f);
      colorMenu->setZOrder(3);
      colorMenu->setContentSize(ccp(20.f, 60.f));
      colorMenu->setLayout(optionsLayout);
      colorMenu->addChild(lockBtn);
      colorMenu->addChild(infoBtn);
      colorMenu->addChild(swapBtn);
      colorMenu->updateLayout();

      //init button with a default values, this will be updated in updateUI and loadLastState
      CCMenuItemSpriteExtra* item = CCMenuItemSpriteExtra::create(createColorSpr(i, 0.f, 0.f), this, menu_selector(MainPopup::onColorChannel));
      item->m_scaleMultiplier = 1.f;
      item->setVisible(false);
      item->addChildAtPosition(label, Anchor::TopRight, ccp(-10.f, -10.f));
      item->addChildAtPosition(colorMenu, Anchor::Center);
      m_colorsMenu->addChild(item);

      m_colorButtons[i] = item;
    }

    // updates color channels and buttons based on the current settings
    updateUI();

    m_nameInput = TextInput::create(150.f, "Palette name", goldFontName);
    m_nameInput->setString("Palette name");
    m_nameInput->setCommonFilter(CommonFilter::Name);
    m_nameInput->setMaxCharCount(25);
    m_nameInput->setTextAlign(TextInputAlign::Left);
    m_nameInput->setAnchorPoint({0.f, 0.5f});
    m_nameInput->setScale(0.7f);
    m_nameInput->hideBG();
    m_mainLayer->addChildAtPosition(m_nameInput, Anchor::TopLeft, ccp(10.f, -30.f));

    CCMenu* mainMenu = CCMenu::create();
    mainMenu->setContentSize(ccp(400.f, 25.f));
    mainMenu->setScale(0.8f);

    m_navMenu = CCMenu::create();
    m_navMenu->setContentSize(ccp(60.f, 30.f));
    m_navMenu->setScale(0.8f);

    optsBG->addChildAtPosition(mainMenu, Anchor::Center, ccp(40.f, -12.5f));
    optsBG->addChildAtPosition(m_navMenu, Anchor::Center, ccp(85.f, -12.5f));

    m_generateSpr = CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("icon.png"_spr), CircleBaseColor::Cyan, CircleBaseSize::Tiny);
    m_generate = CCMenuItemSpriteExtra::create(m_generateSpr, this, menu_selector(MainPopup::onGeneratePalette));
  
    ButtonSprite* saveSpr = ButtonSprite::create("Save");
    saveSpr->setScale(0.6f);
    m_save = CCMenuItemSpriteExtra::create(saveSpr, this, menu_selector(MainPopup::onSavePalette));

    CCSprite* prevSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    CCSprite* nextSprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
    CCMenuItemSpriteExtra* m_prev = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(MainPopup::onPrevPalette));
    CCMenuItemSpriteExtra* m_next = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(MainPopup::onNextPalette));

    m_navMenu->setEnabled(false);
    m_navMenu->setVisible(false);

    prevSprite->setScale(0.6f);
    nextSprite->setScale(0.6f);
    nextSprite->setFlipX(true);

    m_navMenu->addChildAtPosition(CCMenuItemSpriteExtra::create(m_prev, this, menu_selector(MainPopup::onPrevPalette)), Anchor::Center, ccp(70.f, 0.f));
    m_navMenu->addChildAtPosition(CCMenuItemSpriteExtra::create(m_next, this, menu_selector(MainPopup::onNextPalette)), Anchor::Center, ccp(100.f, 0.f));
    m_navMenu->addChildAtPosition(m_save, Anchor::Center, ccp(-210.f, 0.f));
    mainMenu->addChildAtPosition(m_generate, Anchor::Center, ccp(197.5f, 0.f));

    m_infoLabel = CCLabelBMFont::create("",bigFontName);
    m_infoLabel->setScale(0.35f);
    m_infoLabel->setAnchorPoint(ccp(0.f, 0.5f));
    optsBG->addChildAtPosition(m_infoLabel, Anchor::Left,ccp(10.f, -10.5f));

    if (m_isLoaded) loadLastState();

    optionsMenu->updateLayout();
    mainMenu->updateLayout();
    m_colorsMenu->updateLayout();
    m_navMenu->updateLayout();
    return true;
  }

  void loadLastState() {
    updateSpritesColor(manager.getCurrentPalette());
    updateInfoLabel();
    updateNavigationButtons();

    for (int i = 0; i < m_colors; i++) {
      updateLockButton(i, manager.isColorLocked(i));
    }
  }

  void onReset(CCObject *) {
    geode::createQuickPopup(
      "Reset all colors",
      "Are you sure you want to reset all colors?",
      "Cancel", "Reset", [this](auto, bool btn2) {
        if (btn2) {
          handleReset();
        }
      });
  }

  void onSave(CCObject *) {
    SavedPopup::create()->show();
  }

  void onHide(CCObject *) {
    handleHide(!isColorMenuVisible());
  }

  void onInfo(CCObject *) {
    FLAlertLayer::create(
        "Info",
        "HI!",
        "OK")
        ->show();
  }

  void onSettings(CCObject *) {
    auto settingsPopup = SettingsPopup::create();
    settingsPopup->show();
    settingsPopup->onColorsChanged = [this]() {
      updateUI();
    };
  }

  void onColorChannel(CCObject *sender) {
    auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
    auto colorSpr = static_cast<ColorChannelSprite *>(item->getNormalImage());

    if (manager.isColorLocked(utils.colorToHex(colorSpr->getColor()))) {
      FLAlertLayer::create("This color is locked", "Unlock this color to edit it.", "OK")->show();
      return;
    }

    auto color = static_cast<ccColor3B>(colorSpr->getColor());
    auto popup = ColorSelectPopup::create({0, 0, 0});
    popup->m_colorPicker->setColorTarget(colorSpr);
    popup->m_colorPicker->setColorValue(color);
    popup->show();
    return;
  }

  void onGeneratePalette(CCObject *) {
    m_spinner = LoadingSpinner::create(15.f);
    m_generateSpr->getTopNode()->setVisible(false);
    m_generate->addChildAtPosition(m_spinner, Anchor::Center);
    m_generate->setEnabled(false);

    service.request([weak = geode::WeakRef(this)](Palette result) {
      if (auto self = weak.lock()) {
        self->m_spinner->removeFromParent();
        self->m_spinner = nullptr;
        self->m_generateSpr->getTopNode()->setVisible(true);
        self->m_generate->setEnabled(true);
        if (!result.colors.empty()) {
          self->m_isLoaded = true;
          self->data.clearSaved();
          self->updateSpritesColor(result);
          self->updateInfoLabel();
          self->updateNavigationButtons();
          self->updateSaveButton();
        } else {
          FLAlertLayer::create("Error", "Failed to generate palette. Please try again.", "OK")->show();
        }
      }
    });
  }

  void onSavePalette(CCObject *) {
    data.create(manager.getCurrentPalette(), m_nameInput->getString());
    data.setSaved(HueMintService::m_currentPaletteResult.currentItem);
    updateSaveButton();

    Notification::create("Palette saved", NotificationIcon::Success)->show();
  }

  void onNextPalette(CCObject *) {
    updateSpritesColor(manager.getNextPalette());
    updateInfoLabel();
    updateSaveButton();
  }

  void onPrevPalette(CCObject *) {
    updateSpritesColor(manager.getPrevPalette());
    updateInfoLabel();
    updateSaveButton();
  }

  void onLockColorChannel(CCObject *sender) {
    auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
    auto menu = static_cast<CCMenu *>(item->getParent());
    auto result = getIndexFromID(menu->getID());

    if (result.ok()) {
      int index = result.unwrap();

      NineSlice * colorSpr = static_cast<NineSlice *>(m_colorButtons[index]->getNormalImage());
      manager.toggleColorLock(index, utils.colorToHex(colorSpr->getColor()));
      updateLockButton(index, manager.isColorLocked(index));
    }
  }

  void onSwapColorChannel(CCObject *sender) {
      auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
      auto menu = static_cast<CCMenu *>(item->getParent());
      auto result = getIndexFromID(menu->getID());

      if (result.ok()) {
        int index = result.unwrap();
        if (manager.isColorLocked(index)) {
          FLAlertLayer::create("This color is locked", "Unlock this color to swap it.", "OK")->show();
          return;
        }

        if (m_swapIndex == -1) {
          //TODO: change the color of the button sprite to indicate that it's selected
          m_swapIndex = index;
          Notification::create("Select a color to swap with", NotificationIcon::Info)->show();
          return;
        }

        manager.swapColors(m_swapIndex, index);
        updateSpritesColor(manager.getCurrentPalette());
        m_swapIndex = -1;
      }
    }

  void onColorChannelHarmonies(CCObject *sender) {
    auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
    auto menu = static_cast<CCMenu *>(item->getParent());
    auto result = getIndexFromID(menu->getID());

    if (result.ok()) {
      int index = result.unwrap();
      auto colorSpr = static_cast<NineSlice *>(m_colorButtons[index]->getNormalImage());
      HarmonyPopup::create(colorSpr->getColor())->show();
    }
  }

  void updateSpritesColor(Palette palette) {
    for (size_t i = 0; i < palette.colors.size(); i++) {
      NineSlice *colorSpr = static_cast<NineSlice *>(m_colorButtons[i]->getNormalImage());
      applyColorToSprite(colorSpr, palette.colors.at(i));
    }
  }

  void updateInfoLabel() {
    m_infoLabel->setString(
        fmt::format("Results: {} - {}",
                    HueMintService::m_currentPaletteResult.currentItem + 1,
                    HueMintService::m_currentPaletteResult.items)
            .c_str());
  }

  void updateColorButton(int index) {
    bool isVisible = index < m_colors;
    CCMenuItemSpriteExtra *btn = m_colorButtons[index];
    CCSize size = {cropWidth / m_colors, 100.f};

    btn->setNormalImage(createColorSpr(index, size.width, size.height));
    btn->setContentSize(size);
    btn->setVisible(isVisible);
    btn->updateSprite();
    btn->updateLayout();

    //sync visibility of the color menu with the color button
    handleHide(isColorMenuVisible());
  }

  void updateNavigationButtons() {
    m_navMenu->setEnabled(m_isLoaded);
    m_navMenu->setVisible(m_isLoaded);
  }

  void updateLockButton(int index, bool locked) {
    CCMenuItemSpriteExtra * color = m_colorButtons[index];
    CCMenuItemSpriteExtra * lockBtn = static_cast<CCMenuItemSpriteExtra *>(color->getChildByIDRecursive("lock"));

    const char *frame = locked ? "GJ_lock_001.png" : "GJ_lock_open_001.png";
    static_cast<CCSprite *>(lockBtn->getNormalImage())->setDisplayFrame(
        CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame));
  }

  void updateSaveButton() {
    bool isSaved = data.isSaved(HueMintService::m_currentPaletteResult.currentItem);
    ButtonSprite* saveSpr = static_cast<ButtonSprite*>(m_save->getNormalImage());
    saveSpr->setString(isSaved ? "Saved" : "Save");
    saveSpr->updateBGImage(isSaved ? "GJ_button_02.png" : "GJ_button_01.png");
    m_save->setEnabled(!isSaved);
  }

  void updateUI() {
    m_colors = manager.getRequest().num_colors;
    for (int i = 0; i < SettingsManager::MAX_COLORS; i++) {
      updateColorButton(i);
      updateLockButton(i, manager.isColorLocked(i));
    }

    updateSpritesColor(manager.getCurrentPalette());
    m_colorsMenu->updateLayout();
  }

  void handleReset() {
    m_isLoaded = false;
    service.resetPalette();
    updateNavigationButtons();

    for (int i = 0; i < SettingsManager::MAX_COLORS; i++) {
      updateLockButton(i, false);
      updateColorButton(i);
    }

    updateSpritesColor(manager.getCurrentPalette());
    m_infoLabel->setString("");
    m_colorsMenu->updateLayout();
  }

  void handleHide(bool show) {
    for (int i = 0; i < m_colors; i++) {
      CCMenu *menu = static_cast<CCMenu *>(m_colorButtons[i]->getChildByID(fmt::format("color-menu-{}", i)));
      if (menu)menu->setVisible(show);
    }
  }

  bool isColorMenuVisible() {
    CCMenu *menu = static_cast<CCMenu *>(m_colorButtons[0]->getChildByID(fmt::format("color-menu-{}", 0)));
    return menu ? menu->isVisible() : false;
  }

  geode::Result<int> getIndexFromID(std::string id) {
    std::string_view numStr = std::string_view(id).substr(id.rfind('-') + 1);
    return geode::utils::numFromString<int>(numStr);
  }

  NineSlice* createColorSpr(int index, float width, float height) {
    bool isCorner = (index == 0 || index == m_colors - 1);
    const char* spriteName = isCorner ? "square02b_001.png" : "square.png";
    CCRect rect = isCorner ? CCRect{0, 0, 50, 80} : CCRect{0, 0, 80, 80};

    NineSlice* colorSpr = NineSlice::create(spriteName, rect);
    colorSpr->setRotation(isCorner && index == m_colors - 1 ? 180.f : 0.f);
    colorSpr->setContentSize({width, height});
    return colorSpr;
  }

  void applyColorToSprite(NineSlice* sprite, std::string hex = "#FFFFFF") {
    hex = hex.length() == 7 ? hex : "#FFFFFF"; // Fallback to white if the hex code is invalid
    hex.erase(0, 1); // Removes the '#' character
    sprite->setColor(utils.hexToColor(hex));
  }
};

