#pragma once
#include <Geode/ui/ColorPickPopup.hpp>
#include "../managers/HarmonyManager.hpp"

using namespace geode::prelude;

class HarmonyPopup : public geode::Popup {
public:
    static HarmonyPopup* create(ccColor3B const& color);

protected:
    HarmonyManager& m_manager = HarmonyManager::get();
    bool init(ccColor3B const& color);
    CCNode* createHarmonyRow(const char* title, Palette colors, float width, float height = 40.f);
    void onColorSelect(CCObject* sender);
};
