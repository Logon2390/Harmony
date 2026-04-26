#pragma once
#include <string>
#include <vector>
#include <array>
#include "DataManager.hpp"

using namespace geode::prelude;

struct BaseTable {
  int values[6][6];
};

// Default / Vibrant / Hyper-color:
const BaseTable t_default = {{
    {0, 75, 70, 55, 80, 75}, // BG
    {75, 0, 50, 40, 65, 60}, // GROUND
    {70, 50, 0, 25, 70, 60}, // PRIMARY
    {55, 40, 25, 0, 60, 50}, // SECONDARY
    {80, 65, 70, 60, 0, 40}, // ACCENT1
    {75, 60, 60, 50, 40, 0}, // ACCENT2
}};

// High-contrast: high contrast, more uniform values
const BaseTable high_contrast = {{
    {0, 90, 85, 70, 95, 90}, // BG
    {90, 0, 65, 55, 80, 75}, // GROUND
    {85, 65, 0, 35, 85, 75}, // PRIMARY
    {70, 55, 35, 0, 75, 65}, // SECONDARY
    {95, 80, 85, 75, 0, 55}, // ACCENT1
    {90, 75, 75, 65, 55, 0}, // ACCENT2
}};

// Bright-light / Pastel: lower contrast values
const BaseTable t_soft = {{
    {0, 50, 45, 35, 60, 55}, // BG
    {50, 0, 30, 20, 45, 40}, // GROUND
    {45, 30, 0, 15, 50, 40}, // PRIMARY
    {35, 20, 15, 0, 40, 30}, // SECONDARY
    {60, 45, 50, 40, 0, 25}, // ACCENT1
    {55, 40, 40, 30, 25, 0}, // ACCENT2
}};

// Dark: higher contrast for BG and GROUND, 
// but lower for accents to avoid overwhelming darkness
const BaseTable t_dark = {{
    {0, 15, 40, 30, 80, 75}, // BG
    {15, 0, 25, 15, 60, 55}, // GROUND
    {40, 25, 0, 20, 65, 55}, // PRIMARY
    {30, 15, 20, 0, 55, 45}, // SECONDARY
    {80, 60, 65, 55, 0, 35}, // ACCENT1
    {75, 55, 55, 45, 35, 0}, // ACCENT2
}};

struct RequestBody
{
    std::string mode{};                   // transformer, diffusion or random
    int num_colors{};                     // max 12, min 2
    float temperature{};                  // max 2.4, min 0
    int num_results{};                    // max 50 for transformer, 5 for diffusion
    std::string preset{};                 // optional preset name
    std::vector<int> adjacency{};         // nxn adjacency matrix as a flat array of integers
    std::vector<std::string> palette{};   // array of hex color strings, length must match num_colors};
};

struct Palette
{
    std::vector<std::string> colors;
    float score;
};

struct ResponseBody
{
    std::vector<Palette> results;
};

template <>
struct matjson::Serialize<RequestBody>
{
    static geode::Result<RequestBody> fromJson(const matjson::Value &value)
    {
        RequestBody data;
        GEODE_UNWRAP_INTO(data.mode, value["mode"].asString());
        GEODE_UNWRAP_INTO(data.num_colors, value["num_colors"].asInt());
        GEODE_UNWRAP_INTO(data.temperature, value["temperature"].asDouble());
        GEODE_UNWRAP_INTO(data.num_results, value["num_results"].asInt());
        GEODE_UNWRAP_INTO(data.preset, value["preset"].asString());
        GEODE_UNWRAP_INTO(data.adjacency, value["adjacency"].as<std::vector<int>>());
        GEODE_UNWRAP_INTO(data.palette, value["palette"].as<std::vector<std::string>>());
        return geode::Ok(data);
    }

    static matjson::Value toJson(const RequestBody &value)
    {
        return matjson::makeObject(
            {
                {"mode", value.mode},
                {"num_colors", value.num_colors},
                {"temperature", value.temperature},
                {"num_results", value.num_results},
                {"preset", value.preset},
                {"adjacency", value.adjacency},
                {"palette", value.palette}
            });
    }
};

template <>
struct matjson::Serialize<Palette>
{
    static geode::Result<Palette> fromJson(const matjson::Value &value)
    {
        Palette data;
        GEODE_UNWRAP_INTO(data.colors, value["palette"].as<std::vector<std::string>>());
        GEODE_UNWRAP_INTO(data.score, value["score"].asDouble());
        return geode::Ok(data);
    }

    static matjson::Value toJson(const Palette &value)
    {
        return matjson::makeObject(
            {
                {"palette", value.colors},
                {"score", value.score}
            });
    }
};

template <>
struct matjson::Serialize<ResponseBody>
{
    static geode::Result<ResponseBody> fromJson(const matjson::Value &value)
    {
        ResponseBody data;
        GEODE_UNWRAP_INTO(data.results, value["results"].as<std::vector<Palette>>());
        return geode::Ok(data);
    }

    static matjson::Value toJson(const ResponseBody &value)
    {
        return matjson::makeObject(
            {
                {"results", value.results}
            });
    }
};

class SettingsManager {
public:
  static constexpr int MAX_COLORS = 12;
  static constexpr int MIN_COLORS = 2;
  static constexpr float MAX_TEMPERATURE = 2.4f;
  static constexpr float MIN_TEMPERATURE = 0.f;
  static constexpr int MAX_RESULTS = 50;
  static constexpr int MIN_RESULTS = 5;
  static SettingsManager &get() {
    static SettingsManager instance;
    return instance;
  }

  //stores the id of loaded palettes in the current results
  std::unordered_map<std::string, bool> m_loadedPalettes;
  SavedPalette& getCurrentPalette();
  SavedPalette getNextPalette();
  SavedPalette getPrevPalette();
  std::string setMode(bool next);
  std::string setPreset(bool next);
  void setMaxColors(int numColors);
  void setNumResults(int numResults);
  void setTemperature(float temperature);
  void setPaletteName(const std::string& name);
  void toggleColorLock(int index, std::string colorHex);
  bool isColorLocked(int index);
  bool isColorLocked(std::string colorHex);
  void swapColors(int indexFrom, int indexTo);
  void shufflePalette();
  void resetSettings();
  void resetPalettePool();
  void resetLocks();
  void addPalette(const SavedPalette& palette);
  void removePalette(const std::string& id);
  void setLoaded(const std::string& id);
  bool isLoaded(const std::string& id);
  void clearLoaded();
  void resizeAdjacency(int newSize);
  void setAdjacency(int i, int j, int value);
  int getAdjacency(int i, int j);
  void updateAdjacency();
  int getPresetBase(const std::string &preset);
  void gradient();
  void palette();
  bool isGradientActive();
  void toggleGradient();
  int detailContrast(int roleIndex, const std::string& preset);
  const BaseTable& getBaseTable(const std::string& preset);
  const RequestBody &getRequest() const;

private:
    bool m_isGradientActive = false;
    SavedPalette defaultPalette;
    RequestBody m_request;
    std::array<std::string, 3> m_modes;
    std::array<std::string, 7> m_presets;
    const RequestBody m_defaultRequest = {
        .mode = "transformer",
        .num_colors = 4,
        .temperature = 1.2f,
        .num_results = 10,
        .preset = "default",
        .adjacency = {0, 30, 90, 0, 30, 0, 70, 50, 90, 70, 0, 0, 0, 50, 0, 0},
        .palette = {"-", "-", "-", "-"}};

    SettingsManager() {
        defaultPalette = {
            "",
            {"#03045E", "#023E8A", "#0077B6", "#0096C7", "#00B4D8", "#48CAE4", "#90E0EF", "#ADE8F4", "#CAF0F8", "#CFF1F9", "#DBF4FA", "#FFFFFF"},
            "Palette name",
            false
        };
        m_modes = {"transformer", "diffusion", "random"};
        m_presets = {"default", "high-contrast", "bright-light", "pastel", "vibrant", "dark", "hyper-color"};
        m_request = m_defaultRequest;
    }
  };
