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
  enum class RequestStatus {
    Ok,
    RateLimited,
    Failed,
  };

  static HueMintService &get() {
    static HueMintService instance;
    return instance;
  }
  PaletteResult &getPalettePool() { return m_currentPaletteResult; }
  int getPoolSize() { return static_cast<int>(m_currentPaletteResult.palettes.size()); } 
  bool request(std::function<void(Palette, RequestStatus)> onComplete);
  int getRemainingRequests();
  int getSecondsUntilNextSlot();

private:
  static constexpr int MAX_REQUESTS = 15;
  static constexpr int WINDOW = 60;
  std::deque<geode::utils::Timer<>> m_requestTimers;
  void pruneExpiredTimers();
  bool isRateLimited();
  PaletteResult m_currentPaletteResult;
  PaletteResult mapPaletteResult(ResponseBody response);
  async::TaskHolder<web::WebResponse> m_listener;
  std::string m_url;
  HueMintService() {
     m_url = "https://api.huemint.com/color";
     m_currentPaletteResult = PaletteResult{};
    };
};