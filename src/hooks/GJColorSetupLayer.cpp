#include <Geode/modify/GJColorSetupLayer.hpp>
#include "../managers/SimulationManager.hpp"
#include "../ui/SimulationSetupPopup.hpp"

using namespace geode::prelude;

class $modify(MyGJColorSetupLayer, GJColorSetupLayer) {

    struct Fields {
        SimulationManager& manager = SimulationManager::get();
        bool modified = false;
    };

    void onColor(CCObject* sender) {
        if (!m_fields->manager.m_isSetupStage) return GJColorSetupLayer::onColor(sender);

        CCMenuItemSpriteExtra* btn = static_cast<CCMenuItemSpriteExtra*>(sender);
        int colorID = btn->getTag() + (m_colorsPerPage * m_page);

        SimulationSetupPopup* popup = SimulationSetupPopup::create(colorID);
        popup->onColorSelect = [this, btn, colorID]() {
            m_fields->modified = true;
            GJColorSetupLayer::showPage(m_page);
        };
        popup->show();
    }

    void onClose(CCObject* sender) {
        m_fields->manager.m_isSetupStage = false;
        if (m_fields->manager.isActive() && m_fields->modified) {
            m_fields->manager.replace();
            Notification::create("Simulation updated", NotificationIcon::Info)->show();
        }
        GJColorSetupLayer::onClose(sender);
    }

    void updateSpriteColor(ColorChannelSprite* sprite, CCLabelBMFont* label, int id) {
        if (!m_fields->manager.m_isSetupStage) return GJColorSetupLayer::updateSpriteColor(sprite, label, id);

        ccColor3B color = m_fields->manager.isColorSetup(id) ? ccColor3B{0, 255, 0} : ccWHITE;
        label->setColor(color);
        GJColorSetupLayer::updateSpriteColor(sprite, label, id);
    }
};