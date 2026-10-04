#pragma once
#include "../managers/SettingsManager.hpp"
#include "../managers/DataManager.hpp"
#include "../managers/SimulationManager.hpp"
#include "../network/HueMintService.hpp"
#include "../utils/ColorUtils.hpp"
#include "HsvWidgetHost.hpp"

using namespace geode::prelude;

class MainPopup : public Popup, public HsvWidgetHost {
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
    CCMenuItemSpriteExtra* m_applyBtn;
    CCMenuItemSpriteExtra* m_test;
    CCMenuItemSpriteExtra* m_prev;
    CCMenuItemSpriteExtra* m_next;
    CCMenuItemSpriteExtra* m_hsvBtn;
    CCMenuItemSpriteExtra* m_hsvSplitBtn;
    CCMenuItemSpriteExtra* m_hideBtn;
    CCMenu* m_navMenu;
    CCMenu* m_colorsMenu;
    CCMenu* m_testMenu;
    NineSlice* m_testModeBG = nullptr;
    ccHSVValue m_hsvValue = ColorUtils::HSV_IDENTITY;
    int m_singleHsvIndex = -1;
    ccHSVValue m_singleHsvValue = ColorUtils::HSV_IDENTITY;
    std::array<int, 12> m_singleHsvBtnState; // -1 unknown, 0 off, 1 on
    bool m_showColorMenu = true;
    bool m_isLoaded = false;
    bool m_hsvMode = false;
    bool m_hsvSplit = false;
    int m_swapIndex = -1;
    const float width = 440.f;
    const float height = 260.f;
    const float cropWidth = width - 20.f;

    bool init() override;
    void loadLastState();

    void onHsvValueChanged(const ccHSVValue& value) override;
    void applyHsv();
    void updateHsvLayout();
    void updateApplyButton();
    void resetHsvState(bool resetValues);

    void onReset(CCObject* sender);
    void onSave(CCObject* sender);
    void onHide(CCObject* sender);
    void onInfo(CCObject* sender);
    void onSettings(CCObject* sender);
    void onColorChannel(CCObject* sender);
    void onGeneratePalette(CCObject* sender);
    void onSavePalette(CCObject* sender);
    void onHsvToggle(CCObject* sender);
    void onHsvSplitToggle(CCObject* sender);
    void onApplyHsv(CCObject* sender);
    void onNextPalette(CCObject* sender);
    void onPrevPalette(CCObject* sender);
    void onLockColorChannel(CCObject* sender);
    void onSwapColorChannel(CCObject* sender);
    void onColorChannelHarmonies(CCObject* sender);
    void onColorChannelHsv(CCObject* sender);
    void onSimulationToggle(CCObject* sender);
    void onSimulationSettings(CCObject* sender);
    void onSimulationInfo(CCObject* sender);

    void updateColorSprites(std::vector<std::string> colors);
    void updateInfoLabel();
    void updateSimulationLabels();
    void updateColorButton(CCMenuItemSpriteExtra *btn, int index, int limit, bool hsv);
    void updateNavigationButtons();
    void updateLockButton(int index, bool locked);
    void updateSingleHsvButton(int index);
    void updateSaveButton();
    void updateNameInput();
    void updateTestButton();
    void updateUI();

    void handleReset();
    void handleHide(bool show);

    int getCurrentColorLimit();
};
