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
        const std::array<int, 6>& getSpecialColorIDs() const { return m_specialColors; }
        const std::unordered_map<int, int>& getColorSettings() const { return m_colorSettings; }
        std::vector<std::string> getSpecialColors();
        std::function<void()> onSimulationToggled = []() {};

    private:
        std::unordered_map<int, Ref<ColorAction>> m_colorActions; //store original color actions to restore them later
        std::unordered_map<int, int> m_colorSettings; //colorID -> colorIndex
        std::array<int, 6> m_specialColors = {1000, 1001, 1009, 1002, 1013, 1014}; //BG, G, G2, LINE, MG, MG2
        bool m_isActive = false;
        int m_colors = 2;
        bool toggleSimulationFlag();
        SimulationManager() = default;
};