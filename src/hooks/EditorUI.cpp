#include <Geode/modify/EditorUI.hpp>
#include "../ui/MainPopup.hpp"
#include "../ui/SimulationOverlay.hpp"
#include "../managers/DataManager.hpp"
#include "../managers/SimulationManager.hpp"
#include "../utils/ColorUtils.hpp"

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {

    struct Fields {
        CCMenuItemSpriteExtra* mainBtn;
        SimulationOverlay* simulationOverlay;
        SimulationManager& manager = SimulationManager::get();
        bool isPlayTesting = false;
        ~Fields() {
            ColorUtils::get().m_colorSelectPopup = nullptr;
            manager.m_effectManager = nullptr;
            manager.m_settings = nullptr;
        }
    };

    bool init(LevelEditorLayer* layer) {
        if (!EditorUI::init(layer)) return false;

        m_fields.self();

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

        auto menu = this->getChildByID("editor-buttons-menu");
        if (menu != nullptr) {
            menu->addChild(m_fields->mainBtn);
            menu->updateLayout();
        }

        this->runAction(CallFuncExt::create([this]() {
          bool isLiveColorsEnabled = (this->getChildByID("alphalaneous.tinker/live-colors-menu") != nullptr);
          m_fields->simulationOverlay = SimulationOverlay::create(isLiveColorsEnabled, this->m_toolbarHeight);
          this->addChild(m_fields->simulationOverlay);

          m_fields->manager.onSimulationToggled = [this]() {
            if (m_fields->simulationOverlay) {
              m_fields->simulationOverlay->onToggleVisibility();
            }
          };
        }));

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
            bool isPlayTesting = m_fields->isPlayTesting;
            m_fields->simulationOverlay->setVisible(show && m_fields->manager.shouldDisplayOverlay() && !isPlayTesting);
        }
		return EditorUI::showUI(show);
    }

    void onPlaytest(CCObject* sender) {
        m_fields->isPlayTesting = true;
        EditorUI::onPlaytest(sender);
    }

    void onStopPlaytest(CCObject* sender) {
        m_fields->isPlayTesting = false;
        EditorUI::onStopPlaytest(sender);
    }
};
