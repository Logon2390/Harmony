#pragma once
#include <string>
#include <vector>
#include <array>

using namespace geode::prelude;

struct RequestBody
{
    std::string mode{};                   // transformer, diffusion or random
    int num_colors{};                     // max 12, min 2
    float temperature{};                  // max 2.4, min 0
    int num_results{};                    // max 50 for transformer, 5 for diffusion
    std::string preset{};                 // optional preset name
    std::vector<std::string> adjacency{}; // nxn adjacency matrix as a flat array of strings
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

struct PaletteResult
{
    int items;
    int currentItem;
    ResponseBody response;
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
        GEODE_UNWRAP_INTO(data.adjacency, value["adjacency"].as<std::vector<std::string>>());
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

class HueMintManager {
public:
  static constexpr int MAX_COLORS = 12;
  static constexpr int MIN_COLORS = 2;
  static constexpr float MAX_TEMPERATURE = 2.4f;
  static constexpr float MIN_TEMPERATURE = 0.f;
  static constexpr int MAX_RESULTS = 50;
  static constexpr int MIN_RESULTS = 5;
  static HueMintManager &get() {
    static HueMintManager instance;
    return instance;
  }

  Palette& getCurrentPalette();
  Palette getNextPalette();
  Palette getPrevPalette();
  std::string setMode(bool next);
  std::string setPreset(bool next);
  void setMaxColors(int numColors);
  void setNumResults(int numResults);
  void setTemperature(float temperature);
  void toggleColorLock(int index, std::string colorHex);
  bool isColorLocked(int index);
  bool isColorLocked(std::string colorHex);
  void swapColors(int indexFrom, int indexTo);
  void reset();
  const RequestBody &getRequest() const;

private:
    Palette defaultPalette;
    RequestBody m_request;
    std::array<std::string, 3> m_modes;
    std::array<std::string, 7> m_presets;
    const RequestBody m_defaultRequest = {
        .mode = "transformer",
        .num_colors = 4,
        .temperature = 1.0f,
        .num_results = 10,
        .preset = "default",
        .adjacency = {"0", "30", "90", "0", "30", "0", "70", "50", "90", "70",
                      "0", "0", "0", "50", "0", "0"},
        //.adjacency = {"0", "65", "45", "35", "65", "0", "35","65", "45", "35",
        //"0", "35", "35", "65","35", "0"},
        .palette = {"-", "-", "-", "-"}};

    HueMintManager() {
        defaultPalette = {{"#03045E", "#023E8A", "#0077B6", "#0096C7", "#00B4D8", "#48CAE4", "#90E0EF", "#ADE8F4", "#CAF0F8", "#CFF1F9", "#DBF4FA", "#FFFFFF"}, 0.f};
        m_modes = {"transformer", "diffusion", "random"};
        m_presets = {"default", "high-contrast", "bright-light", "pastel", "vibrant", "dark", "hyper-color"};
        m_request = m_defaultRequest;
    }
  };
