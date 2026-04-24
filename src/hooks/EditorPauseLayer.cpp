#include "../managers/SimulationManager.hpp"
#include <Geode/modify/EditorPauseLayer.hpp>

using namespace geode::prelude;

class $modify(MyEditorPauseLayer, EditorPauseLayer) {
  void onSaveAndPlay(CCObject *sender) {
    onExitRestore();
    EditorPauseLayer::onSaveAndPlay(sender);
  }

  void onExitEditor(CCObject *sender) {
    onExitRestore();
    EditorPauseLayer::onExitEditor(sender);
  }

  void onSaveAndExit(CCObject* sender) {
    onExitRestore();
    EditorPauseLayer::onSaveAndExit(sender);
  }

  void onSave(CCObject* sender) {
    onExitRestore(false);
    EditorPauseLayer::onSave(sender);
  }

  // restore original colors when exiting editor
  void onExitRestore(bool reset = true) {
    if (reset) SimulationManager::get().reset();
    if (SimulationManager::get().isActive()) {
      SimulationManager::get().toggleSimulation();
      Notification::create("Simulation mode stopped, original colors restored", NotificationIcon::Info)->show();
    }
  }
};