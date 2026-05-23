#pragma once

using namespace geode::prelude;

class SimulationManager {
    public:
        bool m_isSetupStage = false;
        bool m_isConfigStage = false;
        GJEffectManager* m_effectManager;
        LevelSettingsObject* m_settings;
        static SimulationManager &get() {
            static SimulationManager instance;
            return instance;
        }
        int getMaxColorCount();
        void setup(int colorID, int colorIndex);
        void remove(int colorID);
        void reset();
        void skip(int colorID);
        void unskip(int colorID);
        void clearSettings();
        void clearSkips();
        bool restore();
        bool replace();
        bool toggleSimulation();
        bool isColorSetup(int colorID);
        bool isColorSkipped(int colorID);
        int getColorSetup(int colorID);
        bool shouldDisplayOverlay();
        void saveOrginalColorActions();
        const bool isActive() const { return m_isActive; }
        const void setColors(int colors) { m_colors = colors; }
        const int getColors() const { return m_colors; }
        const int getSavedColors() const { return static_cast<int>(m_colorActions.size()); }
        const int getModifiedColors() const { return static_cast<int>(m_colorSettings.size()); }
        const int getSkippedColors() const { return static_cast<int>(m_skipColorIDs.size()); }
        const std::array<int, 6>& getSpecialColorIDs() const { return m_specialColors; }
        const std::unordered_map<int, int>& getColorSettings() const { return m_colorSettings; }
        std::unordered_map<int, ccColor3B> getColorsByIndex(int index);
        std::vector<std::string> getSpecialColors();
        std::function<void()> onSimulationToggled = []() {};

    private:
        std::unordered_map<int, Ref<ColorAction>> m_colorActions; //store original color actions to restore them later
        std::unordered_map<int, int> m_colorSettings; //colorID -> colorIndex
        std::unordered_set<int> m_skipColorIDs; //colorIDs that should be skipped when restoring colors
        std::array<int, 6> m_specialColors = {1000, 1001, 1009, 1002, 1013, 1014}; //BG, G, G2, LINE, MG, MG2
        bool m_isActive = false;
        int m_colors = 2;
        bool toggleSimulationFlag();
        SimulationManager() = default;
};