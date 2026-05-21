#pragma once
#include "../managers/SimulationManager.hpp"
#include "../managers/SettingsManager.hpp"
#include "../network/HueMintService.hpp"

using namespace geode::prelude;

class SimulationPopup : public Popup {
public:
    std::function<void()> onSettingsChanged = []() {};
    static SimulationPopup* create();

protected:
    SimulationManager& simulation = SimulationManager::get();
    SettingsManager& settings = SettingsManager::get();
    HueMintService& service = HueMintService::get();
    TextInput* m_colors;
    ScrollLayer* m_layer;
    const float width = 300.f;
    const float height = 180.f;
    int ColorsCount = simulation.getMaxColorCount();

    bool init();

    void onDecreaseColors(CCObject* sender);
    void onIncreaseColors(CCObject* sender);
    void onColorsInput(gd::string input);
    void onSpecialColors(CCObject* sender);
    void onCustomColors(CCObject* sender);
    void onRestoreConfig(CCObject* sender);
    void onResetAll(CCObject* sender);
    void onColorsInfo(CCObject* sender);
    void onSpecialColorsInfo(CCObject* sender);
    void onCustomColorsInfo(CCObject* sender);
    void onResetAllInfo(CCObject* sender);
    void onRestoreConfigInfo(CCObject* sender);
};
