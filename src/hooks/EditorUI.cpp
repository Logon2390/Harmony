#include <Geode/modify/EditorUI.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/cocos/cocoa/CCObject.h>
#include "../ui/MainPopup.cpp"

using namespace geode::prelude;

class $modify(MyEditorUI, EditorUI) {

    struct Fields {
        CCMenuItemSpriteExtra* button;
    };

    bool init(LevelEditorLayer* layer) {
        if (!EditorUI::init(layer)) return false;

        auto sprite = EditorButtonSprite::create(CCSprite::createWithSpriteFrameName("icon.png"_spr), EditorBaseColor::Green);

        m_fields->button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(MyEditorUI::onHarmonyPopup));
        m_fields->button->setContentSize(ccp(40.f, 40.f));
        m_fields->button->setID("harmony-button"_spr);

        auto menu = this->getChildByID("editor-buttons-menu");

        if (menu != nullptr) {
            menu->addChild(m_fields->button);
            menu->updateLayout();
        }

        return true;
    }

    void onHarmonyPopup(CCObject*)
    {
        MainPopup::create()->show();
    }

    void showUI(bool show) {
        m_fields->button->setVisible(show);
		return EditorUI::showUI(show);
    }
};
