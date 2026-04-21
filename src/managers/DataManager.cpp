#include "DataManager.hpp"

void DataManager::create(SavedPalette palette, const std::string &name) {

  auto id = random::generateUUID();
  palette.id = id;
  palette.name = name;
  palette.isFavorite = false;
  palette.colors = palette.colors;

  auto &palettes = load();
  palettes.push_back(palette);
  save();
}

void DataManager::update(const SavedPalette &palette) {
  auto &palettes = load();
  auto it = std::find_if(
      palettes.begin(), palettes.end(),
      [&palette](const SavedPalette &p) { return p.id == palette.id; });

  if (it != palettes.end()) {
    *it = palette;
    save();
  }
}

void DataManager::save() 
{ 
    Mod::get()->setSavedValue("palettes", m_palettes); 
}

void DataManager::setSaved(int index) 
{
    m_savedResults[index] = true;
}

bool DataManager::isSaved(int index)
{
    return m_savedResults.contains(index) && m_savedResults.at(index);
}

void DataManager::clearSaved() 
{
    m_savedResults.clear();
}

std::vector<SavedPalette> &DataManager::load() 
{
  if (m_palettes.empty()) {
    m_palettes = Mod::get()->getSavedValue<std::vector<SavedPalette>>("palettes");
  }

  return m_palettes;
}

void DataManager::remove(const std::string &id) 
{
  auto &palettes = load();
  auto it = std::find_if(palettes.begin(), palettes.end(),
                         [&id](const SavedPalette &p) { return p.id == id; });

  if (it != palettes.end()) {
    palettes.erase(it);
    save();
  }
}

void DataManager::setFavorite(const std::string &id) 
{
  auto &palettes = load();
  auto it = std::find_if(palettes.begin(), palettes.end(),
                         [&id](const SavedPalette &p) { return p.id == id; });

  if (it != palettes.end()) {
    it->isFavorite = !it->isFavorite;
    save();
  }
}

bool DataManager::isFavorite(const std::string &id) 
{
  auto &palettes = load();
  auto it = std::find_if(palettes.begin(), palettes.end(),
                         [&id](const SavedPalette &p) { return p.id == id; });

  return it != palettes.end() && it->isFavorite;
}

std::vector<SavedPalette> DataManager::getPaletteByName(const std::string& name)
{
    auto& palettes = load();
    std::vector<SavedPalette> result;

    std::string nameLower = name;
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    std::copy_if(palettes.begin(), palettes.end(), std::back_inserter(result),
        [&nameLower](const SavedPalette& p) {
            std::string paletteLower = p.name;
            std::transform(paletteLower.begin(), paletteLower.end(), paletteLower.begin(), ::tolower);
            return paletteLower.find(nameLower) != std::string::npos;
        });

    return result;
}

std::vector<SavedPalette> DataManager::getFavoritePalettes() 
{
  auto &palettes = load();
  std::vector<SavedPalette> result;

  std::copy_if(palettes.begin(), palettes.end(), std::back_inserter(result),
               [](const SavedPalette &p) { return p.isFavorite; });

  return result;
}

SavedPalette DataManager::getPaletteByID(const std::string &id)
{
    auto &palettes = load();
    auto it = std::find_if(
      palettes.begin(), palettes.end(), [&id](const SavedPalette &p) { 
        return p.id == id; 
      }
    );

    if (it != palettes.end()) {
        return *it;
    }
    return SavedPalette();
}
