#pragma once
#include <Geode/ui/NineSlice.hpp>
#include <Geode/ui/TextInput.hpp>
#include "../managers/SettingsManager.hpp"
#include "../network/HueMintService.hpp"

using namespace geode::prelude;

class SettingsPopup : public Popup {
public:
    std::function<void()> onColorsChanged = []() {};
    static SettingsPopup* create();

protected:
    SettingsManager& manager = SettingsManager::get();
    HueMintService& service = HueMintService::get();
    CCLabelBMFont* m_mode;
    CCLabelBMFont* m_preset;
    TextInput* m_colors;
    TextInput* m_temperature;
    TextInput* m_results;
    CCMenu* m_infoMenu;

    const float width = 300.f;
    const float height = 260.f;

    bool init();

    void onNextMode(CCObject* sender);
    void onprevMode(CCObject* sender);
    void onprevPreset(CCObject* sender);
    void onNextPreset(CCObject* sender);
    void onDecreaseColors(CCObject* sender);
    void onIncreaseColors(CCObject* sender);
    void onDecreaseTemp(CCObject* sender);
    void onIncreaseTemp(CCObject* sender);
    void onDecreaseResults(CCObject* sender);
    void onIncreaseResults(CCObject* sender);
    void onColorsInput(gd::string input);
    void onTemperatureInput(gd::string input);
    void onResultsInput(gd::string input);
    void onAdjacency(CCObject* sender);
    void onResetSettings(CCObject* sender);
    void onModeInfo(CCObject* sender);
    void onPresetInfo(CCObject* sender);
    void onColorsInfo(CCObject* sender);
    void onTemperatureInfo(CCObject* sender);
    void onResultsInfo(CCObject* sender);
    void onAdjacencyInfo(CCObject* sender);
    void onResetInfo(CCObject* sender);

    void updateFields();

    CCNode* createSelectorRow(const char* title, CCNode* node, CCMenuItemSpriteExtra* infoBtn, float height);
    CCMenu* createMenu(RowLayout* layout);
};
