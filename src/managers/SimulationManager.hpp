#pragma once

using namespace geode::prelude;

class SimulationManager {
    public:
        bool m_isSetupStage = false;
        GJEffectManager* m_effectManager;
        LevelSettingsObject* m_settings;
        static SimulationManager &get() {
            static SimulationManager instance;
            return instance;
        }
        int getMaxColorCount();
        void setup(int colorID, int colorIndex);
        void remove(int colorID);
        void reset() { m_colorSettings.clear(); }
        bool restore();
        bool replace();
        bool toggleSimulation();
        bool isColorSetup(int colorID);
        int getColorSetup(int colorID);
        bool shouldDisplayOverlay();
        void saveOrginalColorActions();
        const bool isActive() const { return m_isActive; }
        const int getColors() const { return m_colors; }
        const int getSavedColors() const { return static_cast<int>(m_colorActions.size()); }
        const int getModifiedColors() const { return static_cast<int>(m_colorSettings.size()); }
        const std::unordered_map<int, int>& getColorSettings() const { return m_colorSettings; }
        std::function<void()> onSimulationToggled = []() {};

    private:
        std::unordered_map<int, ColorAction*> m_colorActions; //store original color actions to restore them later
        std::unordered_map<int, int> m_colorSettings; //colorID -> colorIndex
        bool m_isActive = false;
        int m_colors = 2;
        bool toggleSimulationFlag();
        SimulationManager() = default;
};