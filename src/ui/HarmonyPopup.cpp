#include <Geode/ui/ColorPickPopup.hpp>
#include <Geode/ui/ColorPickPopup.hpp>
#include "../managers/HarmonyManager.hpp"

using namespace geode::prelude;

class HarmonyPopup : public geode::Popup {
public:
    static HarmonyPopup* create(ccColor3B const& color) {
        auto popup = new HarmonyPopup();
        if (popup->init(color)) {
            popup->autorelease();
            return popup;
        }
        delete popup;
        return nullptr;
    }

protected:
    HarmonyManager& m_manager = HarmonyManager::get();

    bool init(ccColor3B const& color) {

        if (!Popup::init(340.f, 260.f)) return false;
        this->setTitle("Harmonies");

        CCNode* complementary = createHarmonyRow("Complementary", m_manager.getComplementary(color), 100.f);
        m_mainLayer->addChildAtPosition(complementary, Anchor::Center, ccp(-110.f, 70.f));

        CCNode* analogous = createHarmonyRow("Analogous", m_manager.getAnalogous(color), 100.f);
        m_mainLayer->addChildAtPosition(analogous, Anchor::Center, ccp(0.f, 70.f));

        CCNode* triadic = createHarmonyRow("Triadic", m_manager.getTriadic(color), 100.f);
        m_mainLayer->addChildAtPosition(triadic, Anchor::Center, ccp(110.f, 70.f));

        CCNode* tetradic = createHarmonyRow("Tetradic", m_manager.getTetradic(color), 100.f);
        m_mainLayer->addChildAtPosition(tetradic, Anchor::Center, ccp(-110.f, 25.f));

        CCNode* splitComplementary = createHarmonyRow("Split Complementary", m_manager.getSplitComplementary(color), 100.f);
        m_mainLayer->addChildAtPosition(splitComplementary, Anchor::Center, ccp(0.f, 25.f));

        CCNode* square = createHarmonyRow("Square", m_manager.getSquare(color), 100.f);
        m_mainLayer->addChildAtPosition(square, Anchor::Center, ccp(110.f, 25.f));

        CCNode* shades = createHarmonyRow("Shades", m_manager.getShades(color), 320.f, 35.f);
        m_mainLayer->addChildAtPosition(shades, Anchor::Center, ccp(0.f, -20.f));

        CCNode* tints = createHarmonyRow("Tints", m_manager.getTints(color), 320.f, 35.f);
        m_mainLayer->addChildAtPosition(tints, Anchor::Center, ccp(0.f, -60.f));

        CCNode* tones = createHarmonyRow("Color Scale", m_manager.getColorScale(color), 320.f, 35.f);
        m_mainLayer->addChildAtPosition(tones, Anchor::Center, ccp(0.f, -100.f));
        return true;
    }

    CCNode* createHarmonyRow(const char* title, Palette colors, float width, float height = 40.f) {
        auto bg = NineSlice::create("square02b_001.png", {0.0f, 0.0f, 80.0f, 80.0f});
        bg->setContentSize({width, height});
        bg->setColor({130, 64, 33});

        auto label = CCLabelBMFont::create(title, "goldFont.fnt");
        label->setScale(0.3f);
        label->setAnchorPoint({0.f, 0.5f});
        bg->addChildAtPosition(label, Anchor::Left, ccp(5.f, 10.f));

        auto menu = CCMenu::create();
        menu->setContentSize({80.f, 40.f});
        menu->setAnchorPoint({0.f, 0.5f});
        menu->setLayout(
            RowLayout::create()
            ->setGap(5.f)
            ->setAxisAlignment(AxisAlignment::Center)
            ->setAutoGrowAxis(0.f)
            ->setAutoScale(false));

        bg->addChildAtPosition(menu, Anchor::Left, ccp(5.f, -5.f));

        for (int i = 0; i < colors.colors.size(); ++i) {
            auto hex = colors.colors[i];
            auto color = ColorChannelSprite::create();
            color->setColor(ColorSelectPopup::hexToColor(hex));
            color->setScale(0.5f);

            auto colorBtn = CCMenuItemSpriteExtra::create(color, this, menu_selector(HarmonyPopup::onColorSelect));
            menu->addChild(colorBtn);
        }

        menu->updateLayout();
        return bg;
    }

    void onColorSelect(CCObject *sender) {
      auto colorBtn = static_cast<CCMenuItemSpriteExtra *>(sender);
      auto colorSpr =static_cast<ColorChannelSprite *>(colorBtn->getNormalImage());
      auto color = colorSpr->getColor();

      auto popup = ColorSelectPopup::create(color);
      popup->m_colorPicker->setColorValue(color);
      popup->onCopy(sender);
      Notification::create("Color copied!", NotificationIcon::Success)->show();
    }
};
