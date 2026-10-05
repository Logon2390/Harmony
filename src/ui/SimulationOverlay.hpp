#pragma once
#include "../managers/SimulationManager.hpp"
#include "../managers/SettingsManager.hpp"
#include "../network/HueMintService.hpp"
#include "../utils/ColorUtils.hpp"
#include "HsvWidgetHost.hpp"
#include <Geode/utils/cocos.hpp>

using namespace geode::prelude;

class SimulationOverlay : public NineSlice, public HsvWidgetHost {
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
    Ref<CCArray> m_selectSprites;
    CCMenuItemSpriteExtra* m_prev;
    CCMenuItemSpriteExtra* m_next;
    CCMenuItemSpriteExtra* m_visibilityBtn;
    CCMenuItemSpriteExtra* m_shuffleBtn;
    CCMenuItemSpriteExtra* m_hsvBtn;
    const float width = 290.f;
    const float height = 20.f;
    bool m_isHidden = false;
    bool m_hsvMode = false;

    bool init(bool isLiveColorsEnabled, float positionY);

    void onNext(CCObject* sender);
    void onPrev(CCObject* sender);
    void onShuffle(CCObject* sender);
    void onVisibilityToggle(CCObject* sender);
    void onHsvToggle(CCObject* sender);
    void onHsvValueChanged(const ccHSVValue& value) override;

    void updateUI();
    void updateNavigationButtons();
    void updateInfoLabel();
    void updatePalettePreview();
};
