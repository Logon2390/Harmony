#include <Geode/modify/EditorUI.hpp>
#include "../ui/MainPopup.cpp"
#include "../ui/SimulationOverlay.cpp"
#include "../managers/DataManager.hpp"
#include "../managers/SimulationManager.hpp"

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {

    struct Fields {
        CCMenuItemSpriteExtra* mainBtn;
        SimulationOverlay* simulationOverlay;
    };

    bool init(LevelEditorLayer* layer) {
        if (!EditorUI::init(layer)) return false;

        //pre-load data
        DataManager::get().load();

        auto sprite = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("icon.png"_spr), EditorBaseColor::LightBlue);
        m_fields->mainBtn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(MyEditorUI::onMainPopup));
        m_fields->mainBtn->setContentSize(ccp(40.f, 40.f));
        m_fields->mainBtn->setID("harmony-button"_spr);

        LevelSettingsObject* settings = this->m_editorLayer->m_levelSettings;
        GJEffectManager* effectManager = settings->m_effectManager;

        if (effectManager) {
            SimulationManager::get().m_effectManager = effectManager;
        }

        if (settings) {
            SimulationManager::get().m_settings = settings;
        }

        m_fields->simulationOverlay = SimulationOverlay::create();
        m_fields->simulationOverlay->setPosition({ 260.f, 130.f });
        this->addChild(m_fields->simulationOverlay);
        
        SimulationManager::get().onSimulationToggled = [this]() {
            if (m_fields->simulationOverlay) {
                m_fields->simulationOverlay->onToggleVisibility();
            }
        };

        auto menu = this->getChildByID("editor-buttons-menu");

        if (menu != nullptr) {
            menu->addChild(m_fields->mainBtn);
            menu->updateLayout();
        }

        return true;
    }

    void onMainPopup(CCObject*) {
        MainPopup::create()->show();
    }

    void showUI(bool show) {
        m_fields->mainBtn->setVisible(show);
        if (m_fields->simulationOverlay) {
            m_fields->simulationOverlay->setVisible(show && SimulationManager::get().isActive());
        }
		return EditorUI::showUI(show);
    }
};
