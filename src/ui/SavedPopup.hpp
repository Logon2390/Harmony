#pragma once
#include "../managers/DataManager.hpp"
#include "../managers/SettingsManager.hpp"

using namespace geode::prelude;

class SavedPopup : public geode::Popup {
public:
    std::function<void()> onLoadPalette = []() {};
    static SavedPopup* create();

protected:
    DataManager& m_manager = DataManager::get();
    SettingsManager& m_settings = SettingsManager::get();
    CCMenu* m_menu;
    TextInput* m_searchInput;
    CCMenuItemSpriteExtra* m_prev;
    CCMenuItemSpriteExtra* m_next;
    CCMenuItemSpriteExtra* m_searchBtn;
    CCMenuItemSpriteExtra* m_clearBtn;
    CCMenuItemSpriteExtra* m_favOnlyBtn;
    CCLabelBMFont* m_pageLabel;
    std::vector<SavedPalette> m_items;
    const int m_itemsPerPage = 6;
    int m_page = 0;
    int m_totalItems = 0;
    bool m_showFavOnly = false;

    bool init();

    void onSearchInput(CCObject* sender);
    void onClearInput(CCObject* sender);
    void onFavoriteFilter(CCObject* sender);
    void onPrev(CCObject* sender);
    void onNext(CCObject* sender);
    void onFavoriteToggle(CCObject* sender);
    void onRemove(CCObject* sender);
    void onLoadToggle(CCObject* sender);
    void onColorSelect(CCObject* sender);

    void updatePage(int page);
    void updatePageLabel();
    void updateNavButtons();
    void updateFavBtnSprite(CCMenuItemSpriteExtra* btn, bool isFav);
    void updateUseBtn(CCMenuItemSpriteExtra* btn, bool isLoaded);
    void updateItems(bool reload = true);
    void reloadItems();

    NineSlice* colorPalette(SavedPalette palette);
};
