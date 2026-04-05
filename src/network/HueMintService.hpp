#pragma once
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include "../managers/SettingsManager.hpp"

using namespace geode::prelude;

class HueMintService
{
public:
    static HueMintService &get() {
        static HueMintService instance;
        return instance;
    } 
    static PaletteResult m_currentPaletteResult;
    void request(std::function<void(Palette)> onComplete);
    void resetPalette();

private:
    PaletteResult mapPaletteResult(ResponseBody response);
    async::TaskHolder<web::WebResponse> m_listener;
    std::string m_url;
    HueMintService() {
        m_url = "https://api.huemint.com/color";
    };
};