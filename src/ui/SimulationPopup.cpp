#include "SimulationPopup.hpp"
#include "SimulationSetupPopup.hpp"
#include "../builders/ComponentBuilder.hpp"

SimulationPopup *SimulationPopup::create() {
  auto popup = new SimulationPopup();
  if (popup->init()) {
    popup->autorelease();
    return popup;
  }
  delete popup;
  return nullptr;
}

bool SimulationPopup::init() {
  if (!Popup::init(width, height)) return false;
  this->setTitle("Simulation Settings");

  // sync m_colors
  simulation.setColors(ColorsCount);

  m_layer = ScrollLayer::create({width - 20.f, 120.f}, true, true);
  m_layer->setZOrder(2);
  m_layer->m_contentLayer->setContentSize({width - 20.f, 120.f});
  m_layer->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

  m_mainLayer->addChildAtPosition(m_layer, Anchor::BottomLeft, ccp(10.f, 20.f));

  m_colors = TextInput::create(100.f, "6");
  m_colors->setString(geode::utils::numToString(simulation.getMaxColorCount()));
  m_colors->setCommonFilter(CommonFilter::Int);
  m_colors->setMaxCharCount(2);
  m_colors->setScale(0.6f);
  m_colors->setCallback(
      [this](gd::string input) { this->onColorsInput(input); });

  MenuAction colorsAction = {
    "Number of Colors",
    nullptr,
    menu_selector(SimulationPopup::onColorsInfo),
    menu_selector(SimulationPopup::onDecreaseColors),
    menu_selector(SimulationPopup::onIncreaseColors),
    m_colors
  };

  MenuAction specialColorsAction = {
    "Special Colors",
    menu_selector(SimulationPopup::onSpecialColors),
    menu_selector(SimulationPopup::onSpecialColorsInfo)
  };

  MenuAction customColorsAction = {
    "Custom Colors",
    menu_selector(SimulationPopup::onCustomColors),
    menu_selector(SimulationPopup::onCustomColorsInfo)
  };

  MenuAction restoreConfig = {
    "Restore Config",
    menu_selector(SimulationPopup::onRestoreConfig),
    menu_selector(SimulationPopup::onRestoreConfigInfo)
  };

  MenuAction resetAll = {
    "Reset All",
    menu_selector(SimulationPopup::onResetAll),
    menu_selector(SimulationPopup::onResetAllInfo)
  };

  CCNode* colorsRow = ComponentBuilder::createOptionSelector("Colors", width - 20.f, SelectorType::Option, colorsAction, this);
  m_layer->m_contentLayer->addChild(colorsRow);

  CCNode* specialColorsRow = ComponentBuilder::createOptionSelector("Special Colors", width - 20.f, SelectorType::Button, specialColorsAction, this);
  m_layer->m_contentLayer->addChild(specialColorsRow);

  CCNode* customColorsRow = ComponentBuilder::createOptionSelector("Custom Colors", width - 20.f, SelectorType::Button, customColorsAction, this);
  m_layer->m_contentLayer->addChild(customColorsRow);

  CCNode* restoreConfigRow = ComponentBuilder::createOptionSelector("Restore Config", width - 20.f, SelectorType::Button, restoreConfig, this);
  m_layer->m_contentLayer->addChild(restoreConfigRow);

  CCNode* resetAllRow = ComponentBuilder::createOptionSelector("Reset All", width - 20.f, SelectorType::Button, resetAll, this);
  m_layer->m_contentLayer->addChild(resetAllRow);

  m_layer->m_contentLayer->updateLayout();
  m_layer->moveToTop();
  return true;
}

void SimulationPopup::onDecreaseColors(CCObject *) {
  if (ColorsCount > SettingsManager::MIN_COLORS) {
    simulation.setColors(--ColorsCount);
    m_colors->setString(geode::utils::numToString(ColorsCount));
  } else {
    ColorsCount = simulation.getMaxColorCount();
    simulation.setColors(ColorsCount);
    m_colors->setString(geode::utils::numToString(ColorsCount));
  }
}

void SimulationPopup::onIncreaseColors(CCObject *) {
  if (ColorsCount < simulation.getMaxColorCount()) {
    ColorsCount++;
    simulation.setColors(ColorsCount);
    m_colors->setString(geode::utils::numToString(ColorsCount));
  } else {
    ColorsCount = SettingsManager::MIN_COLORS;
    simulation.setColors(ColorsCount);
    m_colors->setString(geode::utils::numToString(ColorsCount));
  }
}

void SimulationPopup::onColorsInput(gd::string input) {
  auto result = geode::utils::numFromString<int>(input);

  if (result.isOk()) {
    int value = result.unwrap();
    if (value < SettingsManager::MIN_COLORS ||
        value > simulation.getMaxColorCount())
      return;
    ColorsCount = value;
    simulation.setColors(ColorsCount);
    m_colors->setString(geode::utils::numToString(ColorsCount));
  }
}

void SimulationPopup::onSpecialColors(CCObject *) {
  if (service.getPoolSize() == 0) {
    FLAlertLayer::create(
        "No palettes loaded",
        "You need to have at least one palette loaded to start setting up "
        "custom colors. You can <cy>load a palette from the saved section </c> "
        "or <cg>generate a new one. </c>",
        "OK")
        ->show();
    return;
  }
  SimulationSetupPopup::create(0, true)->show();
}

void SimulationPopup::onCustomColors(CCObject *) {
  if (service.getPoolSize() == 0) {
    FLAlertLayer::create(
        "No palettes loaded",
        "You need to have at least one palette loaded to start setting up "
        "custom colors. You can <cy>load a palette from the saved section </c> "
        "or <cg>generate a new one. </c>",
        "OK")
        ->show();
    return;
  }
  simulation.m_isSetupStage = true;
  GJColorSetupLayer::create(simulation.m_settings)->show();
}

void SimulationPopup::onResetAll(CCObject *) {
  geode::createQuickPopup(
    "Reset all colors settings",
    "Are you sure you want to reset all colors settings. This will <cy>stop palette simulation if active.</c>?",
    "Cancel", "Reset", [this](auto, bool btn2) {
      if (btn2) {
        simulation.reset();
        if (simulation.isActive()) {
          simulation.toggleSimulation();
          onSettingsChanged();
        } 
        Notification::create("All color channel setups removed",NotificationIcon::Info)->show();
      }
    });
}

void SimulationPopup::onColorsInfo(CCObject *) {
  FLAlertLayer::create(
      "Number of Colors",
      "Sets the number of <cy>color slots</c> available to link to "
      "<cg>color channels</c> in the simulation.\n"
      "By default it is limited by the <cy>largest palette</c> loaded.",
      "OK")
      ->show();
}

void SimulationPopup::onSpecialColorsInfo(CCObject *) {
  FLAlertLayer::create(
      "Special Colors",
      "Allows you to link <cg>level special colors</c> such as "
      "<cy>BG</c>, <cy>Ground</c> and <cy>Line</c> "
      "to a palette color in the simulation.",
      "OK")
      ->show();
}

void SimulationPopup::onCustomColorsInfo(CCObject *) {
  FLAlertLayer::create(
      "Custom Colors",
      "Allows you to link specific <cg>color channels</c> to a "
      "<cy>palette color</c> in the simulation.\n"
      "Use this to preview how your palette looks applied to "
      "your <cg>level colors</c>.",
      "OK")
      ->show();
}

void SimulationPopup::onResetAllInfo(CCObject *) {
  FLAlertLayer::create(
      "Reset All", "Resets all <cy>color channel</c> links in the simulation.",
      "OK")
      ->show();
}

void SimulationPopup::onRestoreConfigInfo(CCObject *) {
  FLAlertLayer::create(
      "Setup Config",
      "Here you can link your <cg>color channels</c> to <cy>palette slots</c> "
      "by clicking on them to setup your simulation colors."
      "Below you can see the current simulation config, showing the color from "
      "the palette assigned to each color channel."
      "You can also reset individual channels by clicking on the color button "
      "or all of them at once.",
      "Ok")
      ->show();
}

void SimulationPopup::onRestoreConfig(CCObject *) {
  SimulationSetupPopup::create(0, false)->show();
}