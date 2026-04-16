#pragma once
#include "SettingsManager.hpp"

using namespace geode::prelude;

class SimulationManager : public ConfigureValuePopupDelegate {
    public:
        int m_selectedColorID = 0;
        bool m_isSetupStage = false;
        GJEffectManager* m_effectManager;
        LevelSettingsObject* m_settings;
        static SimulationManager &get() {
            static SimulationManager instance;
            return instance;
        }
        int getMaxColorCount();
        void setup(int colorID, int colorIndex);
        void update(SavedPalette palette);
        void remove(int colorID);
        void reset() { m_colorSettings.clear(); }
        bool restore();
        bool replace();
        bool toggleSimulation();
        bool isColorSetup(int colorID);
        const bool isActive() const { return m_isActive; }
        const int getColors() const { return m_colors; }
        const std::unordered_map<int, int>& getColorSettings() const { return m_colorSettings; }
        void saveOrginalColorActions();
        void valuePopupClosed(ConfigureValuePopup* popup, float value) override;

    private:
        std::unordered_map<int, ColorAction*> m_colorActions; //store original color actions to restore them later
        std::unordered_map<int, int> m_colorSettings; //colorID -> colorIndex
        bool m_isActive = false;
        int m_colors = 2;
        SimulationManager() = default;
};