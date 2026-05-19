#pragma once
#include "../network/HueMintService.hpp"
#include "../managers/SimulationManager.hpp"
#include "../managers/SettingsManager.hpp"
#include "../utils/ColorUtils.hpp"

using namespace geode::prelude;

class SimulationSetupPopup : public Popup {
public:
    std::function<void()> onColorSelect = []() {};

    static SimulationSetupPopup* create(int colorID, bool specialColors = false);

protected:
    SimulationManager& simulation = SimulationManager::get();
    SettingsManager& settings = SettingsManager::get();
    HueMintService& service = HueMintService::get();
    ColorUtils& utils = ColorUtils::get();
    Ref<CCArray> m_colorButtons;
    Ref<CCArray> m_labels;
    Ref<CCArray> m_colorSetups;
    Ref<CCArray> m_paletteColors;
    CCMenu* m_navMenu;
    CCMenu* m_colorsMenu;
    CCMenuItemSpriteExtra* m_prev;
    CCMenuItemSpriteExtra* m_next;
    CCMenuItemSpriteExtra* m_reset;
    CCMenuItemSpriteExtra* m_resetAll;
    CCMenuItemSpriteExtra* m_mode;
    ScrollLayer* m_setups;
    const int COLORS_COUNT = simulation.getColors();
    const int MAX_COLORS = settings.MAX_COLORS;
    const float width = 440.f;
    const float height = 260.f;
    const float cropWidth = width - 20.f;
    bool m_isSpecialColors = false;
    bool m_isSpecialColorsSetup = false;
    int selectedColorID = 0;

    bool init(int colorID, bool specialColors = false);

    void onInfo(CCObject* sender);
    void onResetAll(CCObject* sender);
    void onReset(CCObject* sender);
    void onResetColor(CCObject* sender);
    void onModeChange(CCObject* sender);
    void onPrevPalette(CCObject* sender);
    void onNextPalette(CCObject* sender);
    void onColorSetup(CCObject* sender);
    void onClose(CCObject* sender) override;

    void updateNavigationButtons();
    void updateModeButton();
    void updateControlsVisibility(bool state);
    void updateColorSprites(std::vector<std::string> colors);
    void updateColorButton();
    void updateColorLabels();
    void updatePaletteColors();
    void updateResetControls();

    void toggleSpecialColors();
    void initColorsSetup();
    void handleReset(int colorID);
    void handleResetAll();

    std::string formatColorName(int colorID);
};
