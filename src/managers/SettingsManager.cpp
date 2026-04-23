#include "SettingsManager.hpp"
#include "../network/HueMintService.hpp"

HueMintService& service = HueMintService::get();

SavedPalette &SettingsManager::getCurrentPalette() {

  if (service.getPoolSize() == 0) {
    return defaultPalette;
  }

  int totalItems = service.getPoolSize();
  int currentIndex = service.getPalettePool().currentItem;
  int index = currentIndex >= totalItems ? totalItems - 1 : currentIndex;
  return service.getPalettePool().palettes.at(index);
}

SavedPalette SettingsManager::getNextPalette()
{
    int currentIndex = service.getPalettePool().currentItem;
    int nextIndex = currentIndex + 1;

    if (nextIndex >= service.getPoolSize())
        return service.getPalettePool().palettes.at(currentIndex);

    service.getPalettePool().currentItem = nextIndex;
    return service.getPalettePool().palettes.at(nextIndex);
}

SavedPalette SettingsManager::getPrevPalette()
{
    int currentIndex = service.getPalettePool().currentItem;
    int prevIndex = currentIndex - 1;

    if (currentIndex == 0)
        return service.getPalettePool().palettes.at(currentIndex);

    service.getPalettePool().currentItem = prevIndex;
    return service.getPalettePool().palettes.at(prevIndex);
}

std::string SettingsManager::setMode(bool next) 
{
    auto it = std::find(m_modes.begin(), m_modes.end(), m_request.mode);
    size_t index = (it == m_modes.end()) ? 0 : std::distance(m_modes.begin(), it);

    index = next
        ? (index + 1) % m_modes.size()
        : (index + m_modes.size() - 1) % m_modes.size();

    m_request.mode = m_modes.at(index);
    return m_request.mode;
}

std::string SettingsManager::setPreset(bool next)
{
    auto it = std::find(m_presets.begin(), m_presets.end(), m_request.preset);
    size_t index = (it == m_presets.end()) ? 0 : std::distance(m_presets.begin(), it);

    index = next
        ? (index + 1) % m_presets.size()
        : (index + m_presets.size() - 1) % m_presets.size();

    m_request.preset = m_presets.at(index);
    return m_request.preset;
}

void SettingsManager::setMaxColors(int numColors) 
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

void SettingsManager::setNumResults(int numResults) 
{
    if (numResults < 5) numResults = 5;
    if (numResults > 50) numResults = 50;

    // diffusion mode has a max of 5 results
    if (m_request.mode == "diffusion" && numResults > 5) {
        numResults = 5;
    }

    m_request.num_results = numResults;
}

void SettingsManager::setTemperature(float temperature) 
{
    if (temperature < 0) temperature = 0;
    if (temperature > 2.4) temperature = 2.4;

    m_request.temperature = temperature;
}

void SettingsManager::setPaletteName(const std::string &name)
{
    auto& palette = getCurrentPalette();
    palette.name = name;
}

void SettingsManager::toggleColorLock(int index, std::string colorHex) 
{
    if (index < 0 || index >= m_request.num_colors) return;
    colorHex = "#" + colorHex;
    m_request.palette.at(index) = isColorLocked(index) ? "-" : colorHex;
}

bool SettingsManager::isColorLocked(int index) 
{
    if (index < 0 || index >= m_request.num_colors) return false;
    return m_request.palette.at(index) != "-";
}

bool SettingsManager::isColorLocked(std::string colorHex) 
{
    colorHex = "#" + colorHex;
    return std::find(m_request.palette.begin(), m_request.palette.end(), colorHex) != m_request.palette.end();
}

void SettingsManager::swapColors(int indexFrom, int indexTo) 
{
    if (indexFrom < 0 || indexFrom >= m_request.num_colors) return;
    if (indexTo < 0 || indexTo >= m_request.num_colors) return;
    if (indexFrom == indexTo) return;

    std::swap(getCurrentPalette().colors.at(indexFrom), getCurrentPalette().colors.at(indexTo));
}

void SettingsManager::shufflePalette() 
{
    auto& palette = getCurrentPalette();
    utils::random::shuffle<std::vector<std::string>::iterator>(palette.colors.begin(), palette.colors.end());
}

void SettingsManager::resetPalettePool() 
{
    service.getPalettePool() = PaletteResult{};
}

void SettingsManager::resetLocks() 
{
    std::fill(m_request.palette.begin(), m_request.palette.end(), "-");
}

void SettingsManager::addPalette(const SavedPalette &palette)
{
    service.getPalettePool().palettes.push_back(palette);
    setLoaded(palette.id);
}

void SettingsManager::removePalette(const std::string &id)
{
    auto &palettes = service.getPalettePool().palettes;
    palettes.erase(std::remove_if(palettes.begin(), palettes.end(), [&id](const SavedPalette &p) {
        return p.id == id; }), palettes.end());
    m_loadedPalettes.erase(id);

    if (service.getPalettePool().currentItem >= palettes.size() && !palettes.empty()) {
        service.getPalettePool().currentItem = palettes.size() - 1;
    }
}

void SettingsManager::setLoaded(const std::string &id)
{
    m_loadedPalettes[id] = true;
}

bool SettingsManager::isLoaded(const std::string &id)
{
    return m_loadedPalettes.contains(id) && m_loadedPalettes.at(id);
}

void SettingsManager::clearLoaded() 
{
    m_loadedPalettes.clear();
}

void SettingsManager::resetSettings() 
{
    m_request = m_defaultRequest;
}

const RequestBody &SettingsManager::getRequest() const { return m_request; }
