#pragma once
#include "SettingsManager.hpp"

using namespace geode::prelude;

class SimulationManager {
    public:
        GJEffectManager* m_effectManager;
        static SimulationManager &get() {
            static SimulationManager instance;
            return instance;
        }
        void setup();
        void restore();
        void update(Palette palette);
        const bool isActive() const { return m_isActive; }
        const int getColors() const { return m_colors; }

    private:
        bool m_isActive = false;
        int m_colors = 2;
        CCArray* m_colorActions;
        SimulationManager() = default;
};