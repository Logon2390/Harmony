#include "SettingsPopup.hpp"
#include "../builders/SpriteBuilder.hpp"
#include "../ui/AdjacencyPopup.hpp"

SettingsPopup *SettingsPopup::create() {
  auto popup = new SettingsPopup();
  if (popup->init()) {
    popup->autorelease();
    return popup;
  }
  delete popup;
  return nullptr;
}

bool SettingsPopup::init() {
  if (!Popup::init(width, height)) return false;

  this->setTitle("Settings");

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

  m_infoMenu = CCMenu::create();
  m_infoMenu->setZOrder(1);
  m_infoMenu->setContentSize({width, height});
  m_mainLayer->addChildAtPosition(m_infoMenu, Anchor::BottomLeft);

  m_mode = CCLabelBMFont::create(manager.getRequest().mode.c_str(), SpriteBuilder::bigFontName);
  m_mode->setScale(0.5f);

  CCMenu *modeMenu = createMenu(layout);
  modeMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green), this,
      menu_selector(SettingsPopup::onprevMode)));
  modeMenu->addChild(m_mode);
  modeMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green, true), this,
      menu_selector(SettingsPopup::onNextMode)));

  CCMenuItemSpriteExtra *modeInfoBtn = CCMenuItemSpriteExtra::create(
      CCSprite::createWithSpriteFrameName(SpriteBuilder::infoIconName), this,
      menu_selector(SettingsPopup::onModeInfo));
  createSelectorRow("Mode", modeMenu, modeInfoBtn, 80.f);

  m_preset = CCLabelBMFont::create(manager.getRequest().preset.c_str(), SpriteBuilder::bigFontName);
  m_preset->setScale(0.5f);

  CCMenu *presetMenu = createMenu(layout);
  presetMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green), this,
      menu_selector(SettingsPopup::onprevPreset)));
  presetMenu->addChild(m_preset);
  presetMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green, true), this,
      menu_selector(SettingsPopup::onNextPreset)));
  CCMenuItemSpriteExtra *presetInfoBtn = CCMenuItemSpriteExtra::create(
      CCSprite::create(), this, menu_selector(SettingsPopup::onPresetInfo));
  createSelectorRow("Presets", presetMenu, presetInfoBtn, 50.f);

  m_colors = TextInput::create(100.f, "6");
  m_colors->setString(std::to_string(manager.getRequest().num_colors).c_str());
  m_colors->setCommonFilter(CommonFilter::Int);
  m_colors->setMaxCharCount(2);
  m_colors->setScale(0.7f);
  m_colors->setCallback(
[this](gd::string input) { this->onColorsInput(input); });

  CCMenu *colorsMenu = createMenu(layout);
  colorsMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green), this,
      menu_selector(SettingsPopup::onDecreaseColors)));
  colorsMenu->addChild(m_colors);
  colorsMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green, true), this,
      menu_selector(SettingsPopup::onIncreaseColors)));
  CCMenuItemSpriteExtra *colorsInfoBtn = CCMenuItemSpriteExtra::create(
      CCSprite::create(), this, menu_selector(SettingsPopup::onColorsInfo));
  createSelectorRow("Colors", colorsMenu, colorsInfoBtn, 20.f);

  m_temperature = TextInput::create(100.f, "1.3");
  m_temperature->setString(std::to_string(manager.getRequest().temperature).erase(3).c_str());
  m_temperature->setCommonFilter(CommonFilter::Float);
  m_temperature->setMaxCharCount(3);
  m_temperature->setScale(0.7f);
  m_temperature->setCallback(
      [this](gd::string input) { this->onTemperatureInput(input); });

  CCMenu *tempMenu = createMenu(layout);
  tempMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green), this,
      menu_selector(SettingsPopup::onDecreaseTemp)));
  tempMenu->addChild(m_temperature);
  tempMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green, true), this,
      menu_selector(SettingsPopup::onIncreaseTemp)));
  CCMenuItemSpriteExtra *tempInfoBtn = CCMenuItemSpriteExtra::create(CCSprite::create(), this, menu_selector(SettingsPopup::onTemperatureInfo));
  createSelectorRow("Temperature", tempMenu, tempInfoBtn, -10.f);

  m_results = TextInput::create(100.f, "10");
  m_results->setString(std::to_string(manager.getRequest().num_results).c_str());
  m_results->setCommonFilter(CommonFilter::Int);
  m_results->setMaxCharCount(2);
  m_results->setScale(0.7f);
  m_results->setCallback(
      [this](gd::string input) { this->onResultsInput(input); });

  CCMenu *resultsMenu = createMenu(layout);
  resultsMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green), this,
      menu_selector(SettingsPopup::onDecreaseResults)));
  resultsMenu->addChild(m_results);
  resultsMenu->addChild(CCMenuItemSpriteExtra::create(
      SpriteBuilder::createArrow(ArrowSprite::Green, true), this,
      menu_selector(SettingsPopup::onIncreaseResults)));
  CCMenuItemSpriteExtra *resultsInfoBtn = CCMenuItemSpriteExtra::create(CCSprite::create(), this, menu_selector(SettingsPopup::onResultsInfo));
  createSelectorRow("Results", resultsMenu, resultsInfoBtn, -40.f);

  ButtonSprite *adjacencyBtn = ButtonSprite::create("Modify");
  adjacencyBtn->setScale(0.7f);

  CCMenu *adjacencyMenu = createMenu(buttonLayout);
  adjacencyMenu->addChild(CCMenuItemSpriteExtra::create(adjacencyBtn, this, menu_selector(SettingsPopup::onAdjacency)));
  CCMenuItemSpriteExtra *adjacencyInfoBtn = CCMenuItemSpriteExtra::create(CCSprite::create(), this, menu_selector(SettingsPopup::onAdjacencyInfo));
  createSelectorRow("Adjacency", adjacencyMenu, adjacencyInfoBtn, -70.f);

  ButtonSprite *resetBtn = ButtonSprite::create("Reset");
  resetBtn->setScale(0.7f);

  CCMenu *resetMenu = createMenu(buttonLayout);
  resetMenu->addChild(CCMenuItemSpriteExtra::create(resetBtn, this, menu_selector(SettingsPopup::onResetSettings)));
  CCMenuItemSpriteExtra *resetInfoBtn = CCMenuItemSpriteExtra::create(CCSprite::create(), this, menu_selector(SettingsPopup::onResetInfo));
  createSelectorRow("Default values", resetMenu, resetInfoBtn, -100.f);

  m_infoMenu->updateLayout();
  return true;
}

void SettingsPopup::onNextMode(CCObject *) {
  m_mode->setString(manager.setMode(true).c_str());
}

void SettingsPopup::onprevMode(CCObject *) {
  m_mode->setString(manager.setMode(false).c_str());
}

void SettingsPopup::onprevPreset(CCObject *) {
  m_preset->setString(manager.setPreset(false).c_str());
}

void SettingsPopup::onNextPreset(CCObject *) {
  m_preset->setString(manager.setPreset(true).c_str());
}

void SettingsPopup::onDecreaseColors(CCObject *) {
  int current = manager.getRequest().num_colors;
  if (current > SettingsManager::MIN_COLORS) {
    manager.setMaxColors(current - 1);
    m_colors->setString(geode::utils::numToString(current - 1));
  } else {
    manager.setMaxColors(SettingsManager::MAX_COLORS);
    m_colors->setString(geode::utils::numToString(SettingsManager::MAX_COLORS));
  }
  onColorsChanged();
}

void SettingsPopup::onIncreaseColors(CCObject *) {
  int current = manager.getRequest().num_colors;
  if (current < SettingsManager::MAX_COLORS) {
    manager.setMaxColors(current + 1);
    m_colors->setString(geode::utils::numToString(current + 1));
  } else {
    manager.setMaxColors(SettingsManager::MIN_COLORS);
    m_colors->setString(geode::utils::numToString(SettingsManager::MIN_COLORS));
  }
  onColorsChanged();
}

void SettingsPopup::onDecreaseTemp(CCObject *) {
  float current = manager.getRequest().temperature;
  if (current > SettingsManager::MIN_TEMPERATURE) {
    manager.setTemperature(current - 0.1f);
    m_temperature->setString(geode::utils::numToString(current - 0.1f, 1));
  } else {
    manager.setTemperature(SettingsManager::MAX_TEMPERATURE);
    m_temperature->setString(geode::utils::numToString(SettingsManager::MAX_TEMPERATURE, 1));
  }
}

void SettingsPopup::onIncreaseTemp(CCObject *) {
  float current = manager.getRequest().temperature;
  if (current < SettingsManager::MAX_TEMPERATURE) {
    manager.setTemperature(current + 0.1f);
    m_temperature->setString(geode::utils::numToString(current + 0.1f, 1));
  } else {
    manager.setTemperature(SettingsManager::MIN_TEMPERATURE);
    m_temperature->setString(geode::utils::numToString(SettingsManager::MIN_TEMPERATURE, 1));
  }
}

void SettingsPopup::onDecreaseResults(CCObject *) {
  int current = static_cast<int>(geode::utils::numFromString<int>(m_results->getString()).unwrapOr(10));
  if (current > SettingsManager::MIN_RESULTS) {
    manager.setNumResults(current - 1);
    m_results->setString(geode::utils::numToString(current - 1));
  } else {
    manager.setNumResults(SettingsManager::MAX_RESULTS);
    m_results->setString(geode::utils::numToString(SettingsManager::MAX_RESULTS));
  }
}

void SettingsPopup::onIncreaseResults(CCObject *) {
  int current = static_cast<int>(numFromString<int>(m_results->getString()).unwrapOr(10));
  if (current < SettingsManager::MAX_RESULTS) {
    manager.setNumResults(current + 1);
    m_results->setString(geode::utils::numToString(current + 1));
  } else {
    manager.setNumResults(SettingsManager::MIN_RESULTS);
    m_results->setString(geode::utils::numToString(SettingsManager::MIN_RESULTS));
  }
}

void SettingsPopup::onColorsInput(gd::string input) {
  auto result = geode::utils::numFromString<int>(input);

  if (result.isOk()) {
    int value = result.unwrap();
    if (value < SettingsManager::MIN_COLORS || value > SettingsManager::MAX_COLORS) return;
    manager.setMaxColors(value);
    onColorsChanged();
  }
}

void SettingsPopup::onTemperatureInput(gd::string input) {
  auto result = geode::utils::numFromString<float>(input);

  if (result.isOk()) {
    float value = result.unwrap();
    if (value < SettingsManager::MIN_TEMPERATURE || value > SettingsManager::MAX_TEMPERATURE) return;
    manager.setTemperature(value);
  }
}

void SettingsPopup::onResultsInput(gd::string input) {
  auto result = geode::utils::numFromString<int>(input);
  if (result.isOk()) {
    int value = result.unwrap();
    if (value < SettingsManager::MIN_RESULTS || value > SettingsManager::MAX_RESULTS) return;
    manager.setNumResults(value);
  }
}

void SettingsPopup::onAdjacency(CCObject *) {
  AdjacencyPopup::create(manager.getRequest().num_colors)->show();
}

void SettingsPopup::onResetSettings(CCObject *) {
  geode::createQuickPopup(
    "Reset all settings",
    "Are you sure you want to reset all settings?",
    "Cancel", "Reset", [this](auto, bool btn2) {
      if (btn2) {
        manager.resetSettings();
        updateFields();
        onColorsChanged();
      }
    });
}

void SettingsPopup::updateFields() {
  m_mode->setString(manager.getRequest().mode.c_str());
  m_preset->setString(manager.getRequest().preset.c_str());
  m_colors->setString(geode::utils::numToString(manager.getRequest().num_colors));
  m_temperature->setString(geode::utils::numToString(manager.getRequest().temperature, 1));

  int results = (service.getPoolSize() == 0) ? 10 : service.getPoolSize();
  m_results->setString(geode::utils::numToString(results));
}

void SettingsPopup::onModeInfo(CCObject *) {
  FLAlertLayer::create(
      "Generation Mode",
      "<cg>Transformer (AI)</c> generates palettes that feel <cy>natural and "
      "designerly</c>.\n"
      "<cg>Diffusion (AI)</c> follows your <cy>contrast settings</c> more "
      "precisely.\n"
      "<cg>Random</c> generates palettes <cy>by chance</c>, picking the ones "
      "that best match your settings.",
      "OK")
      ->show();
}

void SettingsPopup::onPresetInfo(CCObject *) {
  FLAlertLayer::create(
      "Preset info",
      "Presets are <cy>predefined adjacency values</c> that helps you to "
      "generate the desired <cg>color style</c>.\n"
      "Each preset defines different <cy>contrast relationships</c> "
      "between the colors in your palette.",
      "OK")
      ->show();
}

void SettingsPopup::onColorsInfo(CCObject *) {
  FLAlertLayer::create(
      "Number of Colors",
      "The number of colors in the generated <cy>palette</c>.\n"
      "You can generate between <cg>2</c> and <cg>12</c> colors.",
      "OK")
      ->show();
}

void SettingsPopup::onTemperatureInfo(CCObject *) {
  FLAlertLayer::create(
      "Creativity",
      "Controls how <cy>creative</c> the model is when generating palettes.\n"
      "<cg>Low</c> values return the most <cy>statistically common</c> color "
      "combinations.\n"
      "<cr>High</c> values allow more <cy>diversity</c> but may ignore some "
      "contrast settings.\n"
      "The <cg>default</c> value of <cy>1.2</c> is recommended for most cases.",
      "OK")
      ->show();
}

void SettingsPopup::onResultsInfo(CCObject *) {
  FLAlertLayer::create(
      "Number of Results",
      "How many <cy>palettes</c> the service generates per request.\n"
      "<cg>Transformer</c> mode supports up to <cy>50</c> results.\n"
      "<cg>Diffusion</c> mode supports up to <cy>5</c> results.\n"
      "More results means more <cy>options</c> to choose from.",
      "OK")
      ->show();
}

void SettingsPopup::onAdjacencyInfo(CCObject *) {
  FLAlertLayer::create(
      "Adjacency Matrix",
      "Defines the desired <cy>contrast</c> between each pair of colors.\n"
      "Values go from <cg>0</c> (any contrast) to <cr>100</c> (black/white).\n"
      "<cy>1</c> = no contrast · <cy>50</c> = medium · <cy>100</c> = maximum.",
      "OK")
      ->show();
}

void SettingsPopup::onResetInfo(CCObject *) {
  FLAlertLayer::create(
      "Reset Settings",
      "Resets all settings back to their <cy>default values</c>.", "OK")
      ->show();
}

CCNode* SettingsPopup::createSelectorRow(const char *title, CCNode *node, CCMenuItemSpriteExtra *infoBtn, float height) {
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

CCMenu* SettingsPopup::createMenu(RowLayout *layout) {
  auto menu = CCMenu::create();
  menu->setLayout(layout);
  menu->setScale(0.8f);
  menu->setAnchorPoint({1.f, 0.5f});
  menu->setContentSize({160.f, 60.f});
  return menu;
}
