#pragma once

using namespace geode::prelude;

enum class ArrowSprite {
    Green,
    Cyan,
    Pink,
};

namespace SpriteBuilder {
    constexpr const char *bigFontName = "bigFont.fnt";
    constexpr const char *goldFontName = "goldFont.fnt";
    constexpr const char* arrow1SprName = "GJ_arrow_01_001.png";
    constexpr const char* arrow2SprName = "GJ_arrow_02_001.png";
    constexpr const char* arrow3SprName = "GJ_arrow_03_001.png";
    constexpr const char *infoIconName = "GJ_infoIcon_001.png";
    constexpr const char *backgroundSprName = "square02b_001.png";
    constexpr const char *backgroundSmallSprName = "square02b_small.png";
    constexpr const char *squareSprName = "square.png";
    constexpr const char *hideSprName = "hideBtn_001.png";
    constexpr const char *helpBtnSprName = "GJ_helpBtn_001.png";
    constexpr const char *optionsBtnSprName = "GJ_optionsBtn_001.png";
    constexpr const char *stopEditorBtnSprName = "GJ_stopEditorBtn_001.png";
    constexpr const char *playEditorBtnSprName = "GJ_playEditorBtn_001.png";
    constexpr const char *folderBtnSprName = "gj_folderBtn_001.png";
    constexpr const char *lockOpenSprName = "GJ_lock_open_001.png";
    constexpr const char *lockClosedSprName = "GJ_lock_001.png";
    constexpr const char *resetBtnSprName = "GJ_resetBtn_001.png";
    constexpr const char *heartOffSprName = "gj_heartOff_001.png";
    constexpr const char *heartOnSprName = "gj_heartOn_001.png";
    constexpr const char *inUseBtnSprName = "GJ_selectSongOnBtn_001.png";
    constexpr const char *useBtnSprName = "GJ_selectSongBtn_001.png";
    constexpr const char *circleSprName = "circle.png";
    constexpr const char *swapBtnSprName = "edit_eChangeBG_001.png";
    const char* formatArrowSpriteName(ArrowSprite sprite);
    CCSprite *createArrow(ArrowSprite sprite, bool flipped = false, float scale = 0.6f);
    NineSlice *createColorSpr(CCMenuItemSpriteExtra *btn, int index, int limit, float width = 0.f,float height = 0.f);
}