#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/ui/BasedButtonSprite.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/cocos/label_nodes/CCLabelBMFont.h>
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/cocos/support/CCPointExtension.h>
#include <Geode/ui/Layout.hpp>
#include <GUI/CCControlExtension/CCScale9Sprite.h>

using namespace geode::prelude;

class MainPopup : public Popup {
public:
    static MainPopup* create() {
        auto popup = new MainPopup;
        if (popup->init()) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }

protected:
    bool init() {
        if (!Popup::init(440.f, 260.f)) return false;

        const char* infoIconName = "GJ_infoIcon_001.png";
        const char* backgroundSpriteName = "square02b_001.png";
        const char* bigFontName = "bigFont.fnt";
        const char* goldFontName = "goldFont.fnt";

		this->setTitle("Main Popup");

        auto saveBtnSprite = CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png"), CircleBaseColor::Green, CircleBaseSize::Tiny);
        auto settingsBtnSprite = CircleButtonSprite::create(CCSprite::createWithSpriteFrameName("geode.loader/settings.png"), CircleBaseColor::Green, CircleBaseSize::Tiny);

        CCMenu* optionsMenu = CCMenu::create(
                CCMenuItemSpriteExtra::create(saveBtnSprite, this, menu_selector(MainPopup::onSave)),
            CCMenuItemSpriteExtra::create(settingsBtnSprite, this, menu_selector(MainPopup::onSettings)),
            nullptr
		);

        m_mainLayer->addChildAtPosition(optionsMenu, Anchor::TopRight, ccp(0.f, -20.f));
		optionsMenu->setAnchorPoint(ccp(1.f, 0.5f));
        optionsMenu->setContentSize({ 70.f, 50.f });

        optionsMenu->setLayout(
            RowLayout::create()
            ->setGap(0.5f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setCrossAxisLineAlignment(AxisAlignment::Center)
            ->setCrossAxisOverflow(false)
            ->setAutoScale(false)
        );

        CCScale9Sprite* colorsBG = cocos2d::extension::CCScale9Sprite::create(backgroundSpriteName, { 0.0f, 0.0f, 80.0f, 80.0f });
        m_mainLayer->addChildAtPosition(colorsBG, Anchor::Center, ccp(0.f, 50.f));
        colorsBG->setContentSize({ 400.f, 50.f });
        colorsBG->setColor({ 130, 64, 33 });
        colorsBG->setZOrder(1);
        colorsBG->setID("colorsBG"_spr);

        CCScale9Sprite* settingsBG = cocos2d::extension::CCScale9Sprite::create(backgroundSpriteName, { 0.0f, 0.0f, 80.0f, 80.0f });
        m_mainLayer->addChildAtPosition(settingsBG, Anchor::Center, ccp(0.f, -60.f));
        settingsBG->setContentSize({ 400.f, 100.f });
        settingsBG->setColor({ 130, 64, 33 });
        settingsBG->setZOrder(1);
        settingsBG->setID("settingsBG"_spr);

        CCLabelBMFont* settingsLabel = CCLabelBMFont::create("Settings", goldFontName);
        settingsBG->addChildAtPosition(settingsLabel, Anchor::TopLeft, ccp(10.f, 7.5f));
        settingsLabel->setScale(0.5f);
        settingsLabel->setAnchorPoint(ccp(0.f, 1.f));

		optionsMenu->updateLayout();

        return true;
    }


    void onSave(CCObject*) {

    }

    void onSettings(CCObject*) {

    }
};
