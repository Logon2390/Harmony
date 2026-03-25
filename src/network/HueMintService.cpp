#include "HueMintService.hpp"
#include "../managers/HueMintManager.hpp"

std::string HueMintService::m_url = "https://api.huemint.com/color";
async::TaskHolder<web::WebResponse> HueMintService::m_listener;
PaletteResult HueMintService::m_currentPaletteResult {};

void HueMintService::request(std::function<void(Palette)> onComplete)
{
    auto req = web::WebRequest();

    matjson::Value myjson = HueMintManager::exampleRequest();
    req.header("Content-Type", "application/json");
    req.bodyJSON(myjson);

    m_listener.spawn(
        req.post(m_url),
        [onComplete](web::WebResponse res)
        {
            ResponseBody parsed = res.json().unwrap().as<ResponseBody>().unwrap();
            PaletteResult result = buildPaletteResult(parsed);

            Palette palette = parsed.results.empty() ? Palette{} : parsed.results.at(0);
            onComplete(palette);
            return palette;
        });
}

PaletteResult HueMintService::buildPaletteResult(ResponseBody response) {
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
