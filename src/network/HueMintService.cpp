#include "HueMintService.hpp"
#include "../managers/SettingsManager.hpp"

SettingsManager &manager = SettingsManager::get();

void HueMintService::request(std::function<void(Palette)> onComplete)
{
    auto req = web::WebRequest();

    matjson::Value myjson = manager.getRequest();
    req.header("Content-Type", "application/json");
    req.bodyJSON(myjson);

    m_listener.spawn(req.post(m_url), [this, onComplete](web::WebResponse res) {
      if (res.ok()) {
        ResponseBody parsed = res.json().unwrapOrElse([]() { return ResponseBody{}; }).as<ResponseBody>().unwrap();
        PaletteResult result = mapPaletteResult(parsed);

        Palette palette = parsed.results.empty() ? Palette{} : parsed.results.at(0);
        onComplete(palette);
        return palette;
      }

      geode::log::error("Request failed with code {} and message: {}", res.code(), res.errorMessage());
      onComplete(Palette{});
      return Palette{};
    });
}

PaletteResult HueMintService::mapPaletteResult(ResponseBody response) {
    PaletteResult result;
    result.currentItem = 0;

    for (const auto& palette : response.results) {
      result.palettes.push_back(SavedPalette{
        .id = "",
        .colors = palette.colors,
        .name = "Palette name",
        .isFavorite = false
      });
    }

    m_currentPaletteResult = result;
    return result;
}
