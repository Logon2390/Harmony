#pragma once
#include "../managers/SimulationManager.hpp"
#include "../managers/SettingsManager.hpp"
#include "../network/HueMintService.hpp"
#include "../utils/ColorUtils.hpp"
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class SimulationOverlay : public NineSlice {
public:
    static SimulationOverlay* create(bool isLiveColorsEnabled, float positionY);

    void onToggleVisibility();
    void refresh();

protected:
    SimulationManager& simulation = SimulationManager::get();
    SettingsManager& settings = SettingsManager::get();
    HueMintService& service = HueMintService::get();
    ColorUtils& utils = ColorUtils::get();
    CCLabelBMFont* m_label;
    CCMenu* m_menu;
    CCNode* m_colors;
    Ref<CCArray> m_colorSprites;
    CCMenuItemSpriteExtra* m_prev;
    CCMenuItemSpriteExtra* m_next;
    CCMenuItemSpriteExtra* m_visibilityBtn;
    CCMenuItemSpriteExtra* m_shuffleBtn;
    const float width = 250.f;
    const float height = 20.f;
    bool m_isHidden = false;

    bool init(bool isLiveColorsEnabled, float positionY);

    void onNext(CCObject* sender);
    void onPrev(CCObject* sender);
    void onShuffle(CCObject* sender);
    void onVisibilityToggle(CCObject* sender);

    void updateUI();
    void updateNavigationButtons();
    void updateInfoLabel();
    void updatePalettePreview();
};
