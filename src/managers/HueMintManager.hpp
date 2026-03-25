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
        GEODE_UNWRAP_INTO(data.adjacency, value["adjacency"].as<std::vector<std::string>>());
        GEODE_UNWRAP_INTO(data.palette, value["palette"].as<std::vector<std::string>>());
        return geode::Ok(data);
    }

    static matjson::Value toJson(const RequestBody &value)
    {
        return matjson::makeObject({{"mode", value.mode},
                                    {"num_colors", value.num_colors},
                                    {"temperature", value.temperature},
                                    {"num_results", value.num_results},
                                    {"adjacency", value.adjacency},
                                    {"palette", value.palette}});
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
        return matjson::makeObject({{"palette", value.colors},
                                    {"score", value.score}});
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
        return matjson::makeObject({{"results", value.results}});
    }
};

class HueMintManager
{
public:
    static Palette getNextPalette();
    static Palette getPrevPalette();
    static RequestBody exampleRequest()
    {
        RequestBody request;
        request.mode = "transformer";
        request.num_colors = 4;
        request.temperature = 1.0f;
        request.num_results = 3;
        request.adjacency = {"0", "65", "45", "35", "65", "0", "35", "65", "45", "35", "0", "35", "35", "65", "35", "0"};
        request.palette = {"#ffffff", "-", "-", "-"};
        return request;
    }
};
