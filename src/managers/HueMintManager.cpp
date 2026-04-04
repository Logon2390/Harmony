#include "HueMintManager.hpp"
#include "../network/HueMintService.hpp"

Palette &HueMintManager::getCurrentPalette() {

  if (HueMintService::m_currentPaletteResult.items == 0) {
    return defaultPalette;
  }

  int currentIndex = HueMintService::m_currentPaletteResult.currentItem;
  return HueMintService::m_currentPaletteResult.response.results.at(currentIndex);
}

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

std::string HueMintManager::setMode(bool next) 
{
    auto it = std::find(m_modes.begin(), m_modes.end(), m_request.mode);
    size_t index = (it == m_modes.end()) ? 0 : std::distance(m_modes.begin(), it);

    index = next
        ? (index + 1) % m_modes.size()
        : (index + m_modes.size() - 1) % m_modes.size();

    m_request.mode = m_modes.at(index);
    return m_request.mode;
}

std::string HueMintManager::setPreset(bool next)
{
    auto it = std::find(m_presets.begin(), m_presets.end(), m_request.preset);
    size_t index = (it == m_presets.end()) ? 0 : std::distance(m_presets.begin(), it);

    index = next
        ? (index + 1) % m_presets.size()
        : (index + m_presets.size() - 1) % m_presets.size();

    m_request.preset = m_presets.at(index);
    return m_request.preset;
}

void HueMintManager::setMaxColors(int numColors) 
{
    if (numColors < 2) numColors = 2;
    if (numColors > 12) numColors = 12;

    m_request.num_colors = numColors;

    m_request.adjacency.clear();
    for (int i = 0; i < numColors * numColors; i++) {
        m_request.adjacency.push_back("0");
    }


    auto& palette = m_request.palette;
    if (palette.size() < numColors) {
        palette.resize(numColors, "-");
    } else if (palette.size() > numColors) {
        palette.resize(numColors);
    }
}

void HueMintManager::setNumResults(int numResults) 
{
    if (numResults < 5) numResults = 5;
    if (numResults > 50) numResults = 50;

    // diffusion mode has a max of 5 results
    if (m_request.mode == "diffusion" && numResults > 5) {
        numResults = 5;
    }

    m_request.num_results = numResults;
}

void HueMintManager::setTemperature(float temperature) 
{
    if (temperature < 0) temperature = 0;
    if (temperature > 2.4) temperature = 2.4;

    m_request.temperature = temperature;
}

void HueMintManager::toggleColorLock(int index, std::string colorHex) 
{
    if (index < 0 || index >= m_request.num_colors) return;
    colorHex = "#" + colorHex;
    m_request.palette.at(index) = isColorLocked(index) ? "-" : colorHex;
}

bool HueMintManager::isColorLocked(int index) 
{
    if (index < 0 || index >= m_request.num_colors) return false;
    return m_request.palette.at(index) != "-";
}

bool HueMintManager::isColorLocked(std::string colorHex) 
{
    colorHex = "#" + colorHex;
    return std::find(m_request.palette.begin(), m_request.palette.end(), colorHex) != m_request.palette.end();
}

void HueMintManager::swapColors(int indexFrom, int indexTo) 
{
    if (indexFrom < 0 || indexFrom >= m_request.num_colors) return;
    if (indexTo < 0 || indexTo >= m_request.num_colors) return;
    if (indexFrom == indexTo) return;

    std::swap(getCurrentPalette().colors.at(indexFrom), getCurrentPalette().colors.at(indexTo));
}

void HueMintManager::reset() 
{
    m_request = m_defaultRequest;
}

const RequestBody &HueMintManager::getRequest() const { return m_request; }
