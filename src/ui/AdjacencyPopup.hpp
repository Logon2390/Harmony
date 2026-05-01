#pragma once
#include "../managers/SettingsManager.hpp"

using namespace geode::prelude;

class AdjacencyPopup : public geode::Popup {
public:
    static AdjacencyPopup* create(int colors);

protected:
    SettingsManager& settings = SettingsManager::get();
    CCNode* m_grid;
    Slider* m_slider;
    Ref<CCArray> m_labels;
    CCLabelBMFont* m_valueLabel;
    CCMenuItemSpriteExtra* m_cell;
    int m_colors = 0;

    bool init(int colors);
    void updateGrid();
    void onCellClick(CCObject* sender);
    void onSlider(CCObject* sender);
    void onGradientToggle(CCObject* sender);
};
