#include "MainPopup.hpp"
#include "../builders/SpriteBuilder.hpp"
#include "../ui/HarmonyPopup.hpp"
#include "../ui/SavedPopup.hpp"
#include "../ui/SettingsPopup.hpp"
#include "../ui/SimulationPopup.hpp"

MainPopup *MainPopup::create() {
  auto popup = new MainPopup();
  if (popup->init()) {
    popup->autorelease();
    return popup;
  }
  delete popup;
  return nullptr;
}

bool MainPopup::init() {
  if (!Popup::init(width, height))
    return false;

  m_isLoaded = service.getPoolSize() > 0;
  m_colorButtons = CCArray::createWithCapacity(manager.MAX_COLORS);

  auto resetSpr = CircleButtonSprite::create(
      // @geode-ignore(unknown-resource)
      CCSprite::createWithSpriteFrameName("geode.loader/reload-gold.png"),
      CircleBaseColor::Green, CircleBaseSize::Tiny);

  auto folderSpr = CircleButtonSprite::create(
      CCSprite::createWithSpriteFrameName(SpriteBuilder::folderBtnSprName),
      CircleBaseColor::Green, CircleBaseSize::Tiny);

  auto hideSpr = CircleButtonSprite::create(
      CCSprite::createWithSpriteFrameName(SpriteBuilder::hideSprName),
      CircleBaseColor::Green, CircleBaseSize::Tiny);

  auto settingsSpr = CCSprite::createWithSpriteFrameName(SpriteBuilder::optionsBtnSprName);
  settingsSpr->setScale(0.55f);

  CCMenu *optionsMenu = CCMenu::create(
      CCMenuItemSpriteExtra::create(settingsSpr, this, menu_selector(MainPopup::onSettings)),
      CCMenuItemSpriteExtra::create(folderSpr, this, menu_selector(MainPopup::onSave)),
      CCMenuItemSpriteExtra::create(resetSpr, this, menu_selector(MainPopup::onReset)),
      CCMenuItemSpriteExtra::create(hideSpr, this, menu_selector(MainPopup::onHide)),
      nullptr);

  RowLayout *mainLayout = RowLayout::create();
  mainLayout->setGap(0.f)
      ->setAxisAlignment(AxisAlignment::Center)
      ->setCrossAxisLineAlignment(AxisAlignment::Center)
      ->setCrossAxisOverflow(false)
      ->setAutoScale(false);

  ColumnLayout *optionsLayout = ColumnLayout::create();
  optionsLayout->setGap(2.f)
      ->setAxisAlignment(AxisAlignment::Even)
      ->setCrossAxisLineAlignment(AxisAlignment::Center)
      ->setCrossAxisOverflow(false)
      ->setAutoScale(true);

  m_mainLayer->addChildAtPosition(optionsMenu, Anchor::TopRight,
                                  ccp(-10.f, -20.f));
  optionsMenu->setAnchorPoint(ccp(1.f, 0.5f));
  optionsMenu->setContentSize({200.f, 50.f});
  optionsMenu->setLayout(RowLayout::create()
                             ->setGap(0.5f)
                             ->setAxisAlignment(AxisAlignment::End)
                             ->setCrossAxisLineAlignment(AxisAlignment::Center)
                             ->setAxisReverse(true)
                             ->setCrossAxisOverflow(false)
                             ->setAutoScale(false));

  NineSlice *optsBG = NineSlice::create(SpriteBuilder::backgroundSprName, {0.0f, 0.0f, 80.0f, 80.0f});
  m_mainLayer->addChildAtPosition(optsBG, Anchor::Center, ccp(0.f, -10.f));
  optsBG->setContentSize({420.f, 50.f});
  optsBG->setColor({130, 64, 33});
  optsBG->setZOrder(1);

  NineSlice *testModeBG = NineSlice::create(SpriteBuilder::backgroundSprName, {0.0f, 0.0f, 80.0f, 80.0f});
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
    CCLabelBMFont *label = CCLabelBMFont::create((std::to_string(i + 1)).c_str(), SpriteBuilder::bigFontName);
    label->setZOrder(3);
    label->setScale(0.3f);

    CCSprite *lockSpr = CCSprite::createWithSpriteFrameName(SpriteBuilder::lockOpenSprName);
    CCMenuItemSpriteExtra *lockBtn = CCMenuItemSpriteExtra::create(lockSpr, this, menu_selector(MainPopup::onLockColorChannel));
    lockBtn->setID("lock");
    lockBtn->m_scaleMultiplier = 1.1f;

    CCSprite *infoSpr = CCSprite::createWithSpriteFrameName(SpriteBuilder::infoIconName);
    CCMenuItemSpriteExtra *infoBtn = CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(MainPopup::onColorChannelHarmonies));
    infoBtn->setID("info");
    infoBtn->m_scaleMultiplier = 1.1f;

    CCSprite *swapSpr = CCSprite::createWithSpriteFrameName("edit_eChangeBG_001.png");
    CCRect rect = swapSpr->getTextureRect();
    CCRect newRect = {rect.origin.x, rect.origin.y, rect.size.width, rect.size.height - 10.f};
    swapSpr->setTextureRect(newRect, true, newRect.size);

    CCMenuItemSpriteExtra *swapBtn = CCMenuItemSpriteExtra::create(swapSpr, this, menu_selector(MainPopup::onSwapColorChannel));
    swapBtn->setID("swap");
    swapBtn->m_scaleMultiplier = 1.1f;

    CCMenu *colorMenu = CCMenu::create();
    colorMenu->setID("menu");
    colorMenu->setTag(i);
    colorMenu->setScale(0.6f);
    colorMenu->setZOrder(3);
    colorMenu->setContentSize(ccp(20.f, 70.f));
    colorMenu->setLayout(optionsLayout);
    colorMenu->addChild(lockBtn);
    colorMenu->addChild(infoBtn);
    colorMenu->addChild(swapBtn);
    colorMenu->updateLayout();

    // init button with a default values, this will be updated in updateUI and loadLastState
    CCMenuItemSpriteExtra *item = CCMenuItemSpriteExtra::create(CCSprite::create(), this, menu_selector(MainPopup::onColorChannel));
    m_colorButtons->addObject(item);

    item->m_scaleMultiplier = 1.f;
    item->setNormalImage(SpriteBuilder::createColorSpr(item, i, manager.getRequest().num_colors));
    item->setVisible(false);
    item->addChildAtPosition(label, Anchor::TopRight, ccp(-10.f, -10.f));
    item->addChildAtPosition(colorMenu, Anchor::Center);
    m_colorsMenu->addChild(item);
  }

  m_nameInput = TextInput::create(150.f, "Palette name", SpriteBuilder::goldFontName);
  m_nameInput->setString("Palette name");
  m_nameInput->setCommonFilter(CommonFilter::Name);
  m_nameInput->setMaxCharCount(25);
  m_nameInput->setTextAlign(TextInputAlign::Left);
  m_nameInput->setAnchorPoint({0.f, 0.5f});
  m_nameInput->setScale(0.7f);
  m_nameInput->hideBG();
  m_mainLayer->addChildAtPosition(m_nameInput, Anchor::TopLeft, ccp(10.f, -30.f));

  CCMenu *mainMenu = CCMenu::create();
  mainMenu->setContentSize(ccp(400.f, 25.f));
  mainMenu->setScale(0.8f);

  m_navMenu = CCMenu::create();
  m_navMenu->setContentSize(ccp(60.f, 30.f));
  m_navMenu->setScale(0.8f);

  optsBG->addChildAtPosition(mainMenu, Anchor::Center, ccp(40.f, -12.5f));
  optsBG->addChildAtPosition(m_navMenu, Anchor::Center, ccp(85.f, -12.5f));

  m_generateSpr = CircleButtonSprite::create(
      CCSprite::createWithSpriteFrameName("icon.png"_spr),
      CircleBaseColor::Cyan, CircleBaseSize::Tiny);

  m_generate = CCMenuItemSpriteExtra::create(
      m_generateSpr, this, menu_selector(MainPopup::onGeneratePalette));

  ButtonSprite *saveSpr = ButtonSprite::create("Save");
  m_save = CCMenuItemSpriteExtra::create(saveSpr, this, menu_selector(MainPopup::onSavePalette));
  m_save->setAnchorPoint(ccp(0.f, 0.5f));
  m_save->setScale(0.6f);
  m_save->m_baseScale = 0.6f;
  m_save->m_scaleMultiplier = 1.1f;

  CCSprite *prevSprite = SpriteBuilder::createArrow(ArrowSprite::Cyan);
  CCSprite *nextSprite = SpriteBuilder::createArrow(ArrowSprite::Cyan, true);
  m_prev = CCMenuItemSpriteExtra::create(prevSprite, this, menu_selector(MainPopup::onPrevPalette));
  m_next = CCMenuItemSpriteExtra::create(nextSprite, this, menu_selector(MainPopup::onNextPalette));

  m_navMenu->setEnabled(false);
  m_navMenu->setVisible(false);

  m_navMenu->addChildAtPosition(m_prev, Anchor::Center, ccp(70.f, 0.f));
  m_navMenu->addChildAtPosition(m_next, Anchor::Center, ccp(100.f, 0.f));
  m_navMenu->addChildAtPosition(m_save, Anchor::Center, ccp(-225.f, 0.f));
  mainMenu->addChildAtPosition(m_generate, Anchor::Center, ccp(197.5f, 0.f));

  m_infoLabel = CCLabelBMFont::create("", SpriteBuilder::bigFontName);
  m_infoLabel->setScale(0.35f);
  m_infoLabel->setAnchorPoint(ccp(0.f, 0.5f));
  optsBG->addChildAtPosition(m_infoLabel, Anchor::Left, ccp(10.f, -10.5f));

  CCLabelBMFont *simulationLabel = CCLabelBMFont::create("Palette Simulation Mode", SpriteBuilder::goldFontName);
  simulationLabel->setScale(0.4f);
  simulationLabel->setAnchorPoint(ccp(0.f, 0.5f));
  testModeBG->addChildAtPosition(simulationLabel, Anchor::TopLeft, ccp(10.f, -10.f));

  m_simulationColorsLabel = CCLabelBMFont::create("Modified Colors: 0", SpriteBuilder::bigFontName);
  m_simulationColorsLabel->setScale(0.3f);
  m_simulationColorsLabel->setAnchorPoint(ccp(0.f, 0.5f));
  testModeBG->addChildAtPosition(m_simulationColorsLabel, Anchor::TopLeft, ccp(10.f, -25.f));

  m_simulationSavedLabel = CCLabelBMFont::create("Saved Colors: 0", SpriteBuilder::bigFontName);
  m_simulationSavedLabel->setScale(0.3f);
  m_simulationSavedLabel->setAnchorPoint(ccp(0.f, 0.5f));
  testModeBG->addChildAtPosition(m_simulationSavedLabel, Anchor::TopLeft, ccp(10.f, -35.f));

  m_testMenu = CCMenu::create();
  m_testMenu->setZOrder(2);
  m_testMenu->setContentSize(ccp(100.f, 30.f));
  m_testMenu->setAnchorPoint(ccp(1.f, 0.5f));
  m_testMenu->setLayout(RowLayout::create()
                            ->setGap(5.f)
                            ->setAxisAlignment(AxisAlignment::Even)
                            ->setCrossAxisOverflow(true)
                            ->setAutoScale(false));
  testModeBG->addChildAtPosition(m_testMenu, Anchor::Right, ccp(-10.f, 0.f));

  const char *frame = simulation.isActive() ? SpriteBuilder::stopEditorBtnSprName : SpriteBuilder::playEditorBtnSprName;
  CCSprite *testSpr = CCSprite::createWithSpriteFrameName(frame);
  CCSprite *setupSpr = CCSprite::createWithSpriteFrameName(SpriteBuilder::optionsBtnSprName);
  CCSprite *helpSpr = CCSprite::createWithSpriteFrameName(SpriteBuilder::helpBtnSprName);

  testSpr->setScale(0.8f);
  setupSpr->setScale(0.6f);
  helpSpr->setScale(0.8f);

  m_test = CCMenuItemSpriteExtra::create(testSpr, testSpr, this, menu_selector(MainPopup::onSimulationToggle));
  CCMenuItemSpriteExtra *settingsBtn = CCMenuItemSpriteExtra::create(setupSpr, setupSpr, this, menu_selector(MainPopup::onSimulationSettings));
  CCMenuItemSpriteExtra *helpBtn = CCMenuItemSpriteExtra::create(helpSpr, helpSpr, this, menu_selector(MainPopup::onSimulationInfo));

  m_testMenu->addChild(helpBtn);
  m_testMenu->addChild(settingsBtn);
  m_testMenu->addChild(m_test);

  if (m_isLoaded) {
    loadLastState();
  } else {
    // updates color channels and buttons based on the current settings
    updateColorSprites(manager.getCurrentPalette().colors);
  }

  optionsMenu->updateLayout();
  mainMenu->updateLayout();
  m_colorsMenu->updateLayout();
  m_navMenu->updateLayout();
  m_testMenu->updateLayout();
  return true;
}

void MainPopup::loadLastState() {
  updateColorSprites(manager.getCurrentPalette().colors);
  updateUI();
}

void MainPopup::onReset(CCObject *) {
  geode::createQuickPopup(
    "Reset all colors",
    "Are you sure you want to reset all colors? This "
    "action will also <cg>release all locked colors </c> "
    "and <cy>stop palette simulation if active.</c>",
    "Cancel", "Reset", [this](auto, bool btn2) {
      if (btn2) {
        handleReset();
      }
    });
}

void MainPopup::onSave(CCObject *) {
  SavedPopup *popup = SavedPopup::create();
  popup->show();
  popup->onLoadPalette = [this]() {
    this->m_isLoaded = service.getPoolSize() > 0;
    this->updateUI();
    if (simulation.isActive()) this->onPalettePoolChanged();
    if (this->service.getPoolSize() <= 1) {
      updateColorSprites(manager.getCurrentPalette().colors);
    }
  };
}

void MainPopup::onHide(CCObject *) { handleHide(!isColorMenuVisible()); }

void MainPopup::onSettings(CCObject *) {
  auto settingsPopup = SettingsPopup::create();
  settingsPopup->show();
  settingsPopup->onColorsChanged = [this]() {
    updateColorSprites(manager.getCurrentPalette().colors);
  };
}

void MainPopup::onColorChannel(CCObject *sender) {
  auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
  auto colorSpr = static_cast<ColorChannelSprite *>(item->getNormalImage());

  if (manager.isColorLocked(cc3bToHexString(colorSpr->getColor()))) {
    FLAlertLayer::create("This color is locked", "<cb>Unlock</c> this color to edit it.", "OK")->show();
    return;
  }

  auto color = static_cast<ccColor3B>(colorSpr->getColor());
  auto popup = ColorSelectPopup::create({0, 0, 0});
  popup->m_colorPicker->setColorTarget(colorSpr);
  popup->m_colorPicker->setColorValue(color);
  popup->show();
  return;
}

void MainPopup::onGeneratePalette(CCObject *) {
  m_spinner = LoadingSpinner::create(15.f);
  m_generateSpr->getTopNode()->setVisible(false);
  m_generate->addChildAtPosition(m_spinner, Anchor::Center);
  m_generate->setEnabled(false);

  bool request =
      service.request([weak = geode::WeakRef(this), request](Palette result) {
        if (auto self = weak.lock()) {
          self->m_spinner->removeFromParent();
          self->m_spinner = nullptr;
          self->m_generateSpr->getTopNode()->setVisible(true);
          self->m_generate->setEnabled(true);
          if (!result.colors.empty()) {
            self->m_isLoaded = true;
            self->data.clearSaved();
            self->manager.clearLoaded();
            self->updateColorSprites(result.colors);
            self->updateUI();
            self->onPalettePoolChanged();
          } else {
            std::string message =
                request ? "Failed to generate palette. Please try again."
                        : fmt::format(
                          "Rate limit exceeded. Please wait {} seconds before making another request.",
                          self->service.getSecondsUntilNextSlot());
            FLAlertLayer::create("Error", message, "OK")->show();
            return;
          }
        }
      });
}

void MainPopup::onSavePalette(CCObject *sender) {
  SavedPalette &palette = manager.getCurrentPalette();
  if (manager.isLoaded(palette.id)) {
    std::string newName = m_nameInput->getString();
    geode::createQuickPopup(
        "Update palette", "Do you want to update this palette?", 
        "Cancel", "Update", [this, newName](auto, bool btn2) {
          if (btn2) {
            manager.setPaletteName(newName);
            data.update(manager.getCurrentPalette());
            Notification::create("Palette updated", NotificationIcon::Success)->show();
          }
        }
      );
      return;
  }

  data.create(manager.getCurrentPalette(), m_nameInput->getString());
  data.setSaved(service.getPalettePool().currentItem);
  updateSaveButton();

  Notification::create("Palette saved", NotificationIcon::Success)->show();
}

void MainPopup::onNextPalette(CCObject *sender) {
  if (service.getPalettePool().currentItem < service.getPoolSize() - 1) {
    updateColorSprites(manager.getNextPalette().colors);
    updateUI();

    if (simulation.isActive()) {
      simulation.replace();
      onPalettePoolChanged();
    }
  }
}

void MainPopup::onPrevPalette(CCObject *sender) {
  if (service.getPalettePool().currentItem > 0) {
    updateColorSprites(manager.getPrevPalette().colors);
    updateUI();

    if (simulation.isActive()) {
      simulation.replace();
      onPalettePoolChanged();
    }
  }
}

void MainPopup::onLockColorChannel(CCObject *sender) {
  auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
  auto menu = static_cast<CCMenu *>(item->getParent());
  int index = menu->getTag();

  NineSlice *colorSpr = static_cast<NineSlice *>(m_colorButtons->asExt<CCMenuItemSpriteExtra *>()[index]->getNormalImage());
  manager.toggleColorLock(index, cc3bToHexString(colorSpr->getColor()));
  updateLockButton(index, manager.isColorLocked(index));
}

void MainPopup::onSwapColorChannel(CCObject *sender) {
  auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
  auto menu = static_cast<CCMenu *>(item->getParent());
  int index = menu->getTag();

  if (manager.isColorLocked(index)) {
    FLAlertLayer::create("This color is locked", "<cb>Unlock</c> this color to swap it.", "OK")->show();
    return;
  }

  if (m_swapIndex == -1) {
    m_swapIndex = index;
    Notification::create("Select a color to swap with", NotificationIcon::Info)->show();
    return;
  }

  manager.swapColors(m_swapIndex, index);
  updateColorSprites(manager.getCurrentPalette().colors);
  if (simulation.isActive()) simulation.replace();
  m_swapIndex = -1;
}

void MainPopup::onColorChannelHarmonies(CCObject *sender) {
  auto item = static_cast<CCMenuItemSpriteExtra *>(sender);
  auto menu = static_cast<CCMenu *>(item->getParent());
  int index = menu->getTag();

  auto colorSpr = static_cast<NineSlice *>(m_colorButtons->asExt<CCMenuItemSpriteExtra *>()[index]->getNormalImage());
  HarmonyPopup::create(colorSpr->getColor())->show();
}

void MainPopup::onSimulationToggle(CCObject *) {
  bool isActive = simulation.toggleSimulation();

  std::string message = isActive ? "Simulation mode activated" : "Simulation mode stopped, original colors restored";
  Notification::create(message.c_str(), NotificationIcon::Info)->show();
  updateTestButton();
  updateSimulationLabels();
}

void MainPopup::onSimulationSettings(CCObject *) { 
  auto popup = SimulationPopup::create();
  popup->onSettingsChanged = [this]() {
    this->updateUI();
  };
  popup->show(); 
}

void MainPopup::onSimulationInfo(CCObject *) {
  geode::MDPopup::create(
      "Simulation Mode",
      "Test how a generated <cy>palette</c> looks in your level without "
      "manually editing every color channel.\n\n"
      "In <cg>Settings</c>, link your <cy>color channels</c> to palette slots. "
      "Then toggle the simulation with the <cg>play/stop</c> button.\n\n"
      "When the simulation starts, a <cy>backup</c> of all your used level "
      "colors is created. Linked channels are then replaced with the "
      "<cy>palette colors</c>. During the simulation you can freely modify "
      "<cg>opacity</c>, <cg>HSV</c> and other color settings to experiment "
      "in real time. When you stop, all colors are <cy>restored</c> to their "
      "original state, like <cg>nothing happened</c>.\n\n"
      "You can also start a simulation <cy>without linking</c> any color "
      "channel. "
      "This will still create a backup of all your colors, letting you freely "
      "experiment with your level and then restore everything back "
      "if you didn't like the changes.\n\n"
      "<cg>Modified Colors</c> — how many channels are currently "
      "replaced by palette colors.\n"
      "<cy>Saved Colors</c> — how many channels are backed up and will be "
      "restored on stop. This includes linked channels <cy>and</c> any other "
      "channel you modified during the simulation, even if they are not "
      "linked to a palette slot.\n\n"
      "You can also <cg>browse palettes</c> while the simulation is active "
      "to apply the changes to the new palette on the fly.",
      "OK")
      ->show();
}

void MainPopup::updateColorSprites(std::vector<std::string> colors) {
  CCArrayExt<CCMenuItemSpriteExtra *> colorButtons = m_colorButtons->asExt();
  int limit = getCurrentColorLimit();
  for (size_t i = 0; i < manager.MAX_COLORS; i++) {
    updateColorButton(i, limit);

    if (i < colors.size()) {
      NineSlice *colorSpr = static_cast<NineSlice *>(colorButtons[i]->getNormalImage());
      colorSpr->setColor(cc3bFromHexString(colors.at(i)).unwrapOr(ccColor3B{255, 255, 255}));
    }
  }

  // sync visibility of the color menu with the color button
  handleHide(isColorMenuVisible());
  m_colorsMenu->updateLayout();
}

void MainPopup::updateInfoLabel() {
  if (!m_isLoaded)
    return m_infoLabel->setString("");
  m_infoLabel->setVisible(m_isLoaded);
  m_infoLabel->setString(fmt::format("Results: {} - {}",
                                     service.getPalettePool().currentItem + 1,
                                     service.getPoolSize())
                             .c_str());
}

void MainPopup::updateSimulationLabels() {
  m_simulationColorsLabel->setString(fmt::format("Modified Colors: {}", simulation.getModifiedColors()).c_str());
  m_simulationSavedLabel->setString(fmt::format("Saved Colors: {}", simulation.getSavedColors()).c_str());
}

void MainPopup::updateColorButton(int index, int limit) {
  bool isVisible = index < limit;
  CCMenuItemSpriteExtra *btn = m_colorButtons->asExt<CCMenuItemSpriteExtra *>()[index];

  if (isVisible) {
    updateLockButton(index, manager.isColorLocked(index));

    float width = cropWidth / limit;
    btn->setNormalImage(SpriteBuilder::createColorSpr(btn, index, limit, width, 100.f));
    btn->setContentSize({width, 100.f});
    btn->updateSprite();
    btn->updateLayout();
  }
  btn->setVisible(isVisible);
}

void MainPopup::updateNavigationButtons() {
  if (m_isLoaded) {
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

  m_navMenu->setEnabled(m_isLoaded);
  m_navMenu->setVisible(m_isLoaded);
}

void MainPopup::updateLockButton(int index, bool locked) {
  CCMenuItemSpriteExtra *color =
      m_colorButtons->asExt<CCMenuItemSpriteExtra *>()[index];
  CCMenuItemSpriteExtra *lockBtn = static_cast<CCMenuItemSpriteExtra *>(
      color->getChildByIDRecursive("lock"));

  const char *frame = locked ? SpriteBuilder::lockClosedSprName : SpriteBuilder::lockOpenSprName;
  static_cast<CCSprite *>(lockBtn->getNormalImage())->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame));
}

void MainPopup::updateSaveButton() {
  bool isSaved = data.isSaved(HueMintService::get().getPalettePool().currentItem);
  m_save->setVisible(m_isLoaded);
  m_save->setEnabled(!isSaved);
  if (!m_isLoaded) return;

  bool isLoaded = SettingsManager::get().isLoaded(manager.getCurrentPalette().id);
  std::string title = isSaved ? "Saved" : isLoaded ? "Update" : "Save";
  ButtonSprite *saveSpr = static_cast<ButtonSprite *>(m_save->getNormalImage());
  saveSpr->setString(title.c_str());
  saveSpr->updateBGImage(isSaved    ? "GJ_button_02.png" : isLoaded ? "GJ_button_03.png" : "GJ_button_01.png");
}

void MainPopup::updateNameInput() {
  if (!m_isLoaded) {
    m_nameInput->setString("Palette name");
    return;
  }
  std::string name = manager.getCurrentPalette().name;
  m_nameInput->setString(name.empty() ? "Palette name" : name.c_str());
}

void MainPopup::updateTestButton() {
  CCSprite *testSpr = static_cast<CCSprite *>(m_test->getNormalImage());

  const char *frame = simulation.isActive() ? SpriteBuilder::stopEditorBtnSprName : SpriteBuilder::playEditorBtnSprName;
  testSpr->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frame));
}

void MainPopup::updateUI() {
  updateInfoLabel();
  updateSaveButton();
  updateNameInput();
  updateNavigationButtons();
  updateTestButton();
  updateSimulationLabels();
}

void MainPopup::handleReset() {
  m_isLoaded = false;
  manager.resetPalettePool();
  manager.clearLoaded();
  manager.resetLocks();
  data.clearSaved();

  // stop simulation if active, this will also restore original colors
  if (simulation.isActive()) {
    simulation.toggleSimulation();
    simulation.reset();
    Notification::create("Simulation mode stopped, original colors restored", NotificationIcon::Info)->show();
  }
  updateColorSprites(manager.getCurrentPalette().colors);
  updateUI();
}

void MainPopup::handleHide(bool show) {
  CCArrayExt<CCMenuItemSpriteExtra *> colorButtons = m_colorButtons->asExt();
  int colors = getCurrentColorLimit();
  for (int i = 0; i < colors; i++) {
    CCMenu *menu = static_cast<CCMenu *>(colorButtons[i]->getChildByID("menu"));
    if (menu) menu->setVisible(show);
  }
}

bool MainPopup::isColorMenuVisible() {
  CCMenu *menu = static_cast<CCMenu *>(m_colorButtons->asExt<CCMenuItemSpriteExtra *>()[0]->getChildByID("menu"));
  return menu ? menu->isVisible() : false;
}

int MainPopup::getCurrentColorLimit() {
  int currentIndex = service.getPalettePool().currentItem;
  int loadedColors = manager.m_loadedPalettes.size();
  bool isCustomPalette = currentIndex >= service.getPalettePool().totalItems && loadedColors > 0;

  /* since loaded palettes are always at the end of the pool, if the current
  index is greater than the total items in the pool, it means that we're in a
  loaded palette and we should iterate colors from palette size instead of
  requested colors */
  int limit = isCustomPalette ? manager.getCurrentPalette().colors.size() : manager.getRequest().num_colors;
  return limit;
}
