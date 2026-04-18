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
        SimulationManager& manager = SimulationManager::get();
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
            m_fields->manager.m_effectManager = effectManager;
        }

        if (settings) {
            m_fields->manager.m_settings = settings;
        }

        m_fields->simulationOverlay = SimulationOverlay::create();
        m_fields->simulationOverlay->setPosition({ 260.f, 130.f });
        this->addChild(m_fields->simulationOverlay);
        
        m_fields->manager.onSimulationToggled = [this]() {
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
        MainPopup* popup = MainPopup::create();
        popup->onPalettePoolChanged = [this]() {
            m_fields->simulationOverlay->refresh();
        };

        popup->show();
    }

    void showUI(bool show) {
        m_fields->mainBtn->setVisible(show);
        if (m_fields->simulationOverlay) {
            m_fields->simulationOverlay->setVisible(show && m_fields->manager.isActive());
        }
		return EditorUI::showUI(show);
    }
};
