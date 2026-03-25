#include "HueMintManager.hpp"
#include "../network/HueMintService.hpp"

Palette HueMintManager::getNextPalette()
{
    int currentIndex = HueMintService::m_currentPaletteResult.currentItem;
    int nextIndex = currentIndex + 1;

    if (nextIndex >= HueMintService::m_currentPaletteResult.items)
        return HueMintService::m_currentPaletteResult.response.results.at(currentIndex);

    HueMintService::m_currentPaletteResult.currentItem = nextIndex;
    return HueMintService::m_currentPaletteResult.response.results.at(nextIndex);
}

Palette HueMintManager::getPrevPalette()
{
    int currentIndex = HueMintService::m_currentPaletteResult.currentItem;
    int prevIndex = currentIndex - 1;

    if (currentIndex == 0)
        return HueMintService::m_currentPaletteResult.response.results.at(currentIndex);

    HueMintService::m_currentPaletteResult.currentItem = prevIndex;
    return HueMintService::m_currentPaletteResult.response.results.at(prevIndex);
}
