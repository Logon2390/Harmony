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
    updateAdjacency();
    return m_request.preset;
}

void SettingsManager::setMaxColors(int numColors) 
{
    if (numColors < 2) numColors = 2;
    if (numColors > 12) numColors = 12;

    m_request.num_colors = numColors;
    resizePalette(m_request.palette, numColors);
    resizeAdjacency(numColors);
    updateAdjacency();

    if (isLoaded(getCurrentPalette().id)) {
        auto& palette = getCurrentPalette().colors;
        resizePalette(palette, numColors, true);

        //sync colors with saved palette if loaded to avoid losing colors when changing color count
        auto saved = DataManager::get().getPaletteByID(getCurrentPalette().id);
        std::copy(saved.colors.begin(),
                  saved.colors.begin() + std::min(palette.size(), saved.colors.size()),
                  palette.begin());
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
    int paletteSize = getCurrentPalette().colors.size();
    if (indexFrom < 0 || indexFrom >= paletteSize) return;
    if (indexTo < 0 || indexTo >= paletteSize) return;
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

    // Loaded palettes are always stored at the end of the pool
    const size_t start = palettes.size() - m_loadedPalettes.size();
    auto it = std::find_if(palettes.begin() + start, palettes.end(),
                           [&id](const SavedPalette &p) { return p.id == id; });

    if (it != palettes.end()) palettes.erase(it);
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
    m_isGradientActive = false;
    m_request = m_defaultRequest;
}

int SettingsManager::getPresetBase(const std::string &preset) {
  if (preset == "default") return 10;
  if (preset == "high-contrast") return 15;
  if (preset == "bright-light") return 6;
  if (preset == "pastel") return 6;
  if (preset == "vibrant") return 10;
  if (preset == "dark") return 15;
  if (preset == "hyper-color") return 10;
  return 10;
}

void SettingsManager::resizeAdjacency(int newSize) 
{
    m_request.adjacency.assign(newSize * newSize, 0);
}

void SettingsManager::resizePalette(std::vector<std::string>& palette, int numColors, bool isCustom) {
    std::string val = isCustom ? "#FFFFFF" : "-";
    
    if (palette.size() < numColors) {
        palette.resize(numColors, val);
    } else if (palette.size() > numColors) {
        palette.resize(numColors);
    }
}

void SettingsManager::setAdjacency(int i, int j, int value) 
{
    if (i < 0 || i >= m_request.num_colors) return;
    if (j < 0 || j >= m_request.num_colors) return;

    auto& adjacency = m_request.adjacency;
    adjacency[i * m_request.num_colors + j] = value;
    adjacency[j * m_request.num_colors + i] = value;
}

int SettingsManager::getAdjacency(int i, int j) 
{
    if (i < 0 || i >= m_request.num_colors) return 0;
    if (j < 0 || j >= m_request.num_colors) return 0;

    auto& adjacency = m_request.adjacency;
    return adjacency[i * m_request.num_colors + j];
}

void SettingsManager::gradient() 
{
    int base = getPresetBase(m_request.preset);
    int size = m_request.num_colors;
    auto &matrix = m_request.adjacency;

    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        if (i == j) {
          matrix[i * size + j] = 0;
        } else {
          int value = base * std::abs(i - j);
          matrix[i * size + j] = std::min(value, 100);
        }
      }
    }
}

const BaseTable &SettingsManager::getBaseTable(const std::string &preset) 
{
  if (preset == "high-contrast") return high_contrast;
  if (preset == "bright-light") return t_soft;
  if (preset == "pastel") return t_soft;
  if (preset == "dark") return t_dark;
  return t_default;
}

int SettingsManager::detailContrast(int roleIndex, const std::string &preset) {
  // [BG, GROUND, PRIMARY, SECONDARY, ACCENT1, ACCENT2]
  const int base[6] = {40, 25, 20, 15, 35, 30};
  const int high_contrast[6] = {55, 35, 30, 20, 50, 45};
  const int soft[6] = {25, 15, 12, 10, 22, 18};
  const int dark[6] = {50, 30, 20, 15, 55, 50};

  const int *table = base;
  if (preset == "high-contrast") table = high_contrast;
  else if (preset == "bright-light" || preset == "pastel") table = soft;
  else if (preset == "dark") table = dark;

  if (roleIndex < 6) return table[roleIndex];
  return 10;
}

void SettingsManager::palette()
{
    int size = m_request.num_colors;
    auto& matrix = m_request.adjacency;
    const std::string& preset = m_request.preset;

    const auto& table = getBaseTable(preset);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i == j) {
                matrix[i * size + j] = 0;
                continue;
            }

            int value = 0;

            if (i < 6 && j < 6) {
                value = table.values[i][j];
            }
            else if (i < 6) {
                value = detailContrast(i, preset);
            }
            else if (j < 6) {
                value = detailContrast(j, preset);
            }
            else {
                value = 10;
            }

            matrix[i * size + j] = std::min(value, 100);
        }
    }
}

bool SettingsManager::isGradientActive() {
    return m_isGradientActive;
}

void SettingsManager::toggleGradient() {
    m_isGradientActive = !m_isGradientActive;
}

void SettingsManager::updateAdjacency() {
    if (m_isGradientActive) gradient();
    else palette();
}

const RequestBody &SettingsManager::getRequest() const { return m_request; }
