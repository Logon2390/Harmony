#include "HueMintService.hpp"
#include "../managers/SettingsManager.hpp"

SettingsManager &manager = SettingsManager::get();

void HueMintService::pruneExpiredTimers() {
    while (!m_requestTimers.empty() &&
           m_requestTimers.front().elapsed<std::chrono::seconds>() >= WINDOW) {
        m_requestTimers.pop_front();
    }
}

bool HueMintService::isRateLimited() {
    pruneExpiredTimers();
    return static_cast<int>(m_requestTimers.size()) >= MAX_REQUESTS;
}

int HueMintService::getRemainingRequests() {
    pruneExpiredTimers();
    return MAX_REQUESTS - static_cast<int>(m_requestTimers.size());
}

int HueMintService::getSecondsUntilNextSlot() {
    pruneExpiredTimers();
    if (static_cast<int>(m_requestTimers.size()) < MAX_REQUESTS) return 0;

    int elapsed = m_requestTimers.front().elapsed<std::chrono::seconds>();
    int remaining = WINDOW - elapsed;
    return remaining;
}

bool HueMintService::request(std::function<void(Palette)> onComplete) {
  if (isRateLimited()) {
      onComplete(Palette{});
      return false;
  }

  auto req = web::WebRequest();
  matjson::Value request = manager.getRequest();
  req.header("Content-Type", "application/json");
  req.timeout(std::chrono::seconds(30));
  req.bodyJSON(request);

  m_listener.spawn(req.post(m_url), [this, onComplete](web::WebResponse res) {
    if (res.ok()) {
      ResponseBody parsed = res.json()
        .unwrapOrElse([]() { return ResponseBody{}; })
        .as<ResponseBody>()
        .unwrap();

      m_requestTimers.emplace_back();
      mapPaletteResult(parsed);
      Palette palette = parsed.results.empty() ? Palette{} : parsed.results.at(0);
      onComplete(palette);
      return palette;
    }

    geode::log::error("Request failed: {} - {}", res.code(), res.errorMessage());
    onComplete(Palette{});
    return Palette{};
  });

  return true;
}

PaletteResult HueMintService::mapPaletteResult(ResponseBody response) {
    PaletteResult result;
    result.currentItem = 0;
    result.totalItems = response.results.size();

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
