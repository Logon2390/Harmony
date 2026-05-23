#include <Geode/modify/GJColorSetupLayer.hpp>
#include "../managers/SimulationManager.hpp"
#include "../ui/SimulationSetupPopup.hpp"

using namespace geode::prelude;

class $modify(MyGJColorSetupLayer, GJColorSetupLayer) {

    struct Fields {
        SimulationManager& manager = SimulationManager::get();
        bool modified = false;
    };

    void onColor(CCObject *sender) {
      if (!m_fields->manager.m_isSetupStage && !m_fields->manager.m_isConfigStage) {
        return GJColorSetupLayer::onColor(sender);
      }

      CCMenuItemSpriteExtra *btn = static_cast<CCMenuItemSpriteExtra *>(sender);
      int colorID = btn->getTag() + (m_colorsPerPage * m_page);

      if (m_fields->manager.m_isSetupStage) {
        SimulationSetupPopup *popup = SimulationSetupPopup::create(colorID);
        popup->onColorSelect = [this, btn, colorID]() {
          m_fields->modified = true;
          GJColorSetupLayer::showPage(m_page);
        };
        popup->show();
      }

      if (m_fields->manager.m_isConfigStage) {
        if (!m_fields->manager.isColorSetup(colorID)) {
          FLAlertLayer::create(
              "Color not setup",
              "This color channel is not setup yet. Please setup the color "
              "channel first to be able to skip it when restoring colors.",
              "OK")
              ->show();
        } else {
          if (m_fields->manager.isColorSkipped(colorID)) {
            geode::createQuickPopup(
                "Color already skipped",
                "This color channel is already set to be skipped when "
                "restoring colors. Do you want to set it to be restored to its original color?",
                "Cancel", "Unskip", [this, colorID](auto, bool btn2) {
                  if (btn2) {
                    m_fields->manager.unskip(colorID);
                    GJColorSetupLayer::showPage(m_page);
                  }
                });
          } else {
            geode::createQuickPopup(
                "Skip color restore",
                "Are you sure you want to skip restoring this color? "
                "This means that when you stop the simulation, this color channel will keep the color it had during the simulation instead of restoring its original color.",
                "Cancel", "Skip", [this, colorID](auto, bool btn2) {
                  if (btn2) {
                    m_fields->manager.skip(colorID);
                    GJColorSetupLayer::showPage(m_page);
                  }
                });
          }
        }
      }
    }

    void onClose(CCObject* sender) {
        m_fields->manager.m_isSetupStage = false;
        m_fields->manager.m_isConfigStage = false;
        if (m_fields->manager.isActive() && m_fields->modified) {
            m_fields->manager.replace();
            Notification::create("Simulation updated", NotificationIcon::Info)->show();
        }
        GJColorSetupLayer::onClose(sender);
    }

    virtual void keyBackClicked() {
        m_fields->manager.m_isSetupStage = false;
        m_fields->manager.m_isConfigStage = false;
        GJColorSetupLayer::keyBackClicked();
    }

    void updateSpriteColor(ColorChannelSprite* sprite, CCLabelBMFont* label, int id) {
        if (!m_fields->manager.m_isSetupStage && !m_fields->manager.m_isConfigStage) return GJColorSetupLayer::updateSpriteColor(sprite, label, id);

        ccColor3B color = m_fields->manager.isColorSkipped(id) ? ccRED : (m_fields->manager.isColorSetup(id) ? ccGREEN : ccWHITE);
        label->setColor(color);
        GJColorSetupLayer::updateSpriteColor(sprite, label, id);
    }
};