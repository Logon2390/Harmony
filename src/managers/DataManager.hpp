#pragma once

using namespace geode::prelude;

struct SavedPalette {
    std::string id;
    std::vector<std::string> colors;
    std::string name;
    bool isFavorite;
};

template <>
struct matjson::Serialize<SavedPalette>
{
    static geode::Result<SavedPalette> fromJson(const matjson::Value& value)
    {
        SavedPalette palette;
        GEODE_UNWRAP_INTO(palette.id, value["id"].asString());
        GEODE_UNWRAP_INTO(palette.colors, value["colors"].as<std::vector<std::string>>());
        GEODE_UNWRAP_INTO(palette.name, value["name"].asString());
        GEODE_UNWRAP_INTO(palette.isFavorite, value["isFavorite"].asBool());
        return geode::Ok(palette);
    }

    static matjson::Value toJson(const SavedPalette& value)
    {
        return matjson::makeObject({
            {"id", value.id},
            {"colors", value.colors},
            {"name", value.name},
            {"isFavorite", value.isFavorite}
        });
    }
};

class DataManager {
    public:
        static DataManager& get() {
            static DataManager instance;
            return instance;
        }
        //stores the index of the saved palettes in the current results to avoid saving duplicates
        std::unordered_map<int, bool> m_savedResults;
        std::vector<SavedPalette> m_palettes;
        std::vector<SavedPalette>& load();
        std::vector<SavedPalette> getPaletteByName(const std::string& name);
        std::vector<SavedPalette> getFavoritePalettes();
        SavedPalette getPaletteByID(const std::string& id);
        void create(SavedPalette& palette, const std::string &name);
        void update(const SavedPalette &palette);
        void remove(const std::string& id);
        void setFavorite(const std::string& id);
        bool isFavorite(const std::string& id);
        void save();
        void setSaved(int index);
        bool isSaved(int index);
        void clearSaved();
    private:
        DataManager() {
            m_palettes = {};
        }
};