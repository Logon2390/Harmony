#include <Geode/modify/EditorUI.hpp>
#include "../ui/MainPopup.cpp"
#include "../managers/DataManager.hpp"

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {

    struct Fields {
        CCMenuItemSpriteExtra* mainBtn;
    };

    bool init(LevelEditorLayer* layer) {
        if (!EditorUI::init(layer)) return false;

        //pre-load data
        DataManager::get().load();
        
        auto sprite = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("icon.png"_spr), EditorBaseColor::LightBlue);
        m_fields->mainBtn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(MyEditorUI::onMainPopup));
        m_fields->mainBtn->setContentSize(ccp(40.f, 40.f));
        m_fields->mainBtn->setID("harmony-button"_spr);

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
		return EditorUI::showUI(show);
    }
};
