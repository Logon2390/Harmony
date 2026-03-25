#pragma once
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include "../managers/HueMintManager.hpp"

using namespace geode::prelude;

class HueMintService
{
public:
    static PaletteResult m_currentPaletteResult;
    static void request(std::function<void(Palette)> onComplete);
    static void resetPalette();

private:
    static PaletteResult buildPaletteResult(ResponseBody response);
    static async::TaskHolder<web::WebResponse> m_listener;
    static std::string m_url;
};