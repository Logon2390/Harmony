#include "HueMintService.hpp"
#include "../managers/HueMintManager.hpp"

PaletteResult HueMintService::m_currentPaletteResult{};
HueMintManager &manager = HueMintManager::get();

void HueMintService::request(std::function<void(Palette)> onComplete)
{
    auto req = web::WebRequest();

    matjson::Value myjson = manager.getRequest();
    req.header("Content-Type", "application/json");
    req.bodyJSON(myjson);

    m_listener.spawn(req.post(m_url), [this, onComplete](web::WebResponse res) {
      if (res.ok()) {
        ResponseBody parsed = res.json().unwrap().as<ResponseBody>().unwrap();
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
    result.items = response.results.size();
    result.currentItem = 0;
    result.response = response;

    m_currentPaletteResult = result;
    return result;
}

void HueMintService::resetPalette() {
    m_currentPaletteResult = PaletteResult{};
}
