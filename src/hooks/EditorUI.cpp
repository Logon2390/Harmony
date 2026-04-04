#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include "../ui/MainPopup.cpp"
//#include "../ui/HarmonyPopup.cpp"

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {

    struct Fields {
        CCMenuItemSpriteExtra* mainBtn;
        CCMenuItemSpriteExtra* harmonyBtn;
    };

    bool init(LevelEditorLayer* layer) {
        if (!EditorUI::init(layer)) return false;

        auto sprite = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("icon.png"_spr), EditorBaseColor::LightBlue);
        m_fields->mainBtn = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(MyEditorUI::onMainPopup));
        m_fields->mainBtn->setContentSize(ccp(40.f, 40.f));
        m_fields->mainBtn->setID("harmony-button"_spr);

        auto harmonySpr = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"), EditorBaseColor::LightBlue);
        m_fields->harmonyBtn = CCMenuItemSpriteExtra::create(harmonySpr, this, menu_selector(MyEditorUI::onHarmonyPopup));
        m_fields->harmonyBtn->setContentSize(ccp(40.f, 40.f));
        m_fields->harmonyBtn->setID("harmony-info-button"_spr);

        auto menu = this->getChildByID("editor-buttons-menu");

        if (menu != nullptr) {
            menu->addChild(m_fields->mainBtn);
            menu->addChild(m_fields->harmonyBtn);
            menu->updateLayout();
        }

        return true;
    }

    void onMainPopup(CCObject*) {
        MainPopup::create()->show();
    }

    void onHarmonyPopup(CCObject*)
    {
        HarmonyPopup::create({255, 5, 255})->show();
    }

    void showUI(bool show) {
        m_fields->mainBtn->setVisible(show);
        m_fields->harmonyBtn->setVisible(show);
		return EditorUI::showUI(show);
    }
};
