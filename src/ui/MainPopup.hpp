#pragma once
#include "../managers/SettingsManager.hpp"
#include "../managers/DataManager.hpp"
#include "../managers/SimulationManager.hpp"
#include "../network/HueMintService.hpp"
#include "../utils/ColorUtils.hpp"

using namespace geode::prelude;

class MainPopup : public Popup {
public:
    std::function<void()> onPalettePoolChanged = []() {};
    static MainPopup* create();

protected:
    ColorUtils& utils = ColorUtils::get();
    DataManager& data = DataManager::get();
    SettingsManager& manager = SettingsManager::get();
    SimulationManager& simulation = SimulationManager::get();
    HueMintService& service = HueMintService::get();
    Ref<CCArray> m_colorButtons;
    TextInput* m_nameInput;
    LoadingSpinner* m_spinner = nullptr;
    CCLabelBMFont* m_infoLabel;
    CCLabelBMFont* m_simulationColorsLabel;
    CCLabelBMFont* m_simulationSavedLabel;
    CCLabelBMFont* m_simulationSkippedLabel;
    CircleButtonSprite* m_generateSpr;
    CCMenuItemSpriteExtra* m_generate;
    CCMenuItemSpriteExtra* m_save;
    CCMenuItemSpriteExtra* m_test;
    CCMenuItemSpriteExtra* m_prev;
    CCMenuItemSpriteExtra* m_next;
    CCMenu* m_navMenu;
    CCMenu* m_colorsMenu;
    CCMenu* m_testMenu;
    bool m_showColorMenu = true;
    bool m_isLoaded = false;
    int m_swapIndex = -1;
    const float width = 440.f;
    const float height = 260.f;
    const float cropWidth = width - 20.f;

    bool init();
    void loadLastState();

    void onReset(CCObject* sender);
    void onSave(CCObject* sender);
    void onHide(CCObject* sender);
    void onInfo(CCObject* sender);
    void onSettings(CCObject* sender);
    void onColorChannel(CCObject* sender);
    void onGeneratePalette(CCObject* sender);
    void onSavePalette(CCObject* sender);
    void onNextPalette(CCObject* sender);
    void onPrevPalette(CCObject* sender);
    void onLockColorChannel(CCObject* sender);
    void onSwapColorChannel(CCObject* sender);
    void onColorChannelHarmonies(CCObject* sender);
    void onSimulationToggle(CCObject* sender);
    void onSimulationSettings(CCObject* sender);
    void onSimulationInfo(CCObject* sender);

    void updateColorSprites(std::vector<std::string> colors);
    void updateInfoLabel();
    void updateSimulationLabels();
    void updateColorButton(CCMenuItemSpriteExtra *btn, int index, int limit);
    void updateNavigationButtons();
    void updateLockButton(int index, bool locked);
    void updateSaveButton();
    void updateNameInput();
    void updateTestButton();
    void updateUI();

    void handleReset();
    void handleHide(bool show);

    int getCurrentColorLimit();
};
