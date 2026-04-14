#pragma once
#include "../managers/SettingsManager.hpp"
#include "../managers/DataManager.hpp"
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

struct PaletteResult
{
    int totalItems;
    int currentItem;
    std::vector<SavedPalette> palettes;
};

class HueMintService {
public:
  static HueMintService &get() {
    static HueMintService instance;
    return instance;
  }
  PaletteResult &getPalettePool() { return m_currentPaletteResult; } 
  void request(std::function<void(Palette)> onComplete);

private:
  PaletteResult m_currentPaletteResult;
  PaletteResult mapPaletteResult(ResponseBody response);
  async::TaskHolder<web::WebResponse> m_listener;
  std::string m_url;
  HueMintService() {
     m_url = "https://api.huemint.com/color";
     m_currentPaletteResult = PaletteResult{};
    };
};