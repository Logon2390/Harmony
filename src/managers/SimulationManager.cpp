#include "SimulationManager.hpp"
#include "../network/HueMintService.hpp"
#include "../utils/ColorUtils.hpp"

SettingsManager& settings = SettingsManager::get();


bool SimulationManager::restore() 
{
    if (m_colorActions.empty()) return false;
    for (auto& [colorID, colorAction] : m_colorActions) {
        if (!colorAction) continue;
        m_effectManager->setColorAction(colorAction, colorID);
    }

    m_colorActions.clear();
    return false;
}

void SimulationManager::setup(int colorID, int colorIndex) 
{
    if (colorIndex < 0 || colorIndex >= getMaxColorCount()) return;

    // link color channel ID with color index in the palette
    m_colorSettings[colorID] = colorIndex;    
}

bool SimulationManager::replace() 
{
    if (!m_isActive) return false;
    if (m_colorActions.empty()) saveOrginalColorActions();

    auto currentPalette = settings.getCurrentPalette();
    int paletteSize = static_cast<int>(currentPalette.colors.size());

    for (auto& [colorID, colorIndex] : m_colorSettings) {
        auto colorAction = m_effectManager->getColorAction(colorID);
        ccColor3B color = colorIndex < paletteSize ? ColorUtils::get().hexToColor(currentPalette.colors.at(colorIndex).erase(0, 1)) : ccColor3B{ 255, 255, 255 };
        colorAction->m_fromColor = color;
    }

    return true;
}

void SimulationManager::remove(int colorID) 
{
    m_colorSettings.erase(colorID);
}

bool SimulationManager::toggleSimulation()
{
    toggleSimulationFlag();

    if (!m_isActive) return restore();
    return replace();
}

bool SimulationManager::isColorSetup(int colorID)
{
    return m_colorSettings.contains(colorID);
}

int SimulationManager::getColorSetup(int colorID)
{
    if (m_colorSettings.contains(colorID)) {
        return m_colorSettings[colorID];
    }

    return -1;
}

int SimulationManager::getMaxColorCount() 
{
    auto& pool = HueMintService::get().getPalettePool();
    if (pool.palettes.empty()) return settings.getRequest().num_colors;

    auto it = std::max_element(pool.palettes.begin(), pool.palettes.end(), 
        [](const SavedPalette& a, const SavedPalette& b) {
            return a.colors.size() < b.colors.size();
        });

    return static_cast<int>(it->colors.size());
}

void SimulationManager::saveOrginalColorActions() 
{
    m_colorActions.clear();
    CCArray* original = m_effectManager->getAllColorActions();

    for (int i = 0; i < original->count(); i++) {
        auto colorAction = static_cast<ColorAction*>(original->objectAtIndex(i));
        if (!colorAction) continue;

        auto copy = colorAction->getCopy();
        copy->m_colorID = colorAction->m_colorID;
        copy->m_uniqueID = colorAction->m_uniqueID;
        copy->m_controlID = colorAction->m_controlID;
        copy->m_copyID = colorAction->m_copyID;
        copy->m_copyHSV = colorAction->m_copyHSV;
        copy->m_legacyHSV = colorAction->m_legacyHSV;
        copy->m_copyOpacity = colorAction->m_copyOpacity;
        copy->m_copyColorCalculated = colorAction->m_copyColorCalculated;
        copy->m_copyColorLoop = colorAction->m_copyColorLoop;
        copy->m_inheritanceNode = colorAction->m_inheritanceNode;
        copy->m_colorSprite = colorAction->m_colorSprite;
        m_colorActions[colorAction->m_colorID] = copy;
    }
}

bool SimulationManager::shouldDisplayOverlay()
{
    return getModifiedColors() > 0 && m_isActive;
}

std::vector<std::string> SimulationManager::getSpecialColors() 
{
    std::vector<std::string> result;
    for (const auto& colorID : m_specialColors) {
        auto colorAction = m_effectManager->getColorAction(colorID);
        if (colorAction) {
            ccColor3B color = colorAction->m_fromColor;
            result.push_back(ColorUtils::get().colorToHex(color));
        } else {
            result.push_back("#FFFFFF");
        }
    }
    return result;
}

std::unordered_map<int, ccColor3B> SimulationManager::getColorsByIndex(int index) 
{
    std::unordered_map<int, ccColor3B> result;
    for (const auto& [colorID, colorIndex] : m_colorSettings) {
        if (colorIndex == index) {
            auto colorAction = m_effectManager->getColorAction(colorID);
            if (colorAction) {
                result[colorID] = colorAction->m_fromColor;
            }
        }
    }
    return result;
}

bool SimulationManager::toggleSimulationFlag()
{
    m_isActive = !m_isActive;
    onSimulationToggled();
    return m_isActive;
}
