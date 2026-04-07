#include "../managers/DataManager.hpp"
#include "../utils/ColorUtils.hpp"

using namespace geode::prelude;

class SavedPopup : public geode::Popup {
public:
  static SavedPopup *create() {
    auto popup = new SavedPopup();
    if (popup->init()) {
      popup->autorelease();
      return popup;
    }
    delete popup;
    return nullptr;
  }

protected:
  DataManager &m_manager = DataManager::get();
  CCMenu* m_menu;
  TextInput* m_searchInput;
  CCMenuItemSpriteExtra* m_prev;
  CCMenuItemSpriteExtra* m_next;
  CCMenuItemSpriteExtra* m_searchBtn;
  CCMenuItemSpriteExtra* m_clearBtn;
  CCMenuItemSpriteExtra* m_favOnlyBtn;
  CCLabelBMFont* m_pageLabel;
  std::vector<SavedPalette> m_items;
  const int m_itemsPerPage = 9;
  int m_page = 0;
  int m_totalItems = 0;
  bool m_showFavOnly = false;
  
  bool init() {
    if (!Popup::init(440.f, 260.f)) return false;

    m_items = m_manager.load();
    m_totalItems = m_items.size();

    if (m_totalItems == 0) {
      CCLabelBMFont* label = CCLabelBMFont::create("No saved palettes!", "goldFont.fnt");
      label->setScale(0.7f);
      m_mainLayer->addChildAtPosition(label, Anchor::Center);
      return true;
    }

    // reverse the vector to show the most recent palettes first
    std::reverse(m_items.begin(), m_items.end());

    m_pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_pageLabel->setScale(0.4f);

    m_searchInput = TextInput::create(300.f, "Search by name");
    m_searchInput->setCommonFilter(CommonFilter::Alphanumeric);
    m_searchInput->setMaxCharCount(25);
    m_searchInput->setTextAlign(TextInputAlign::Left);
    m_searchInput->setAnchorPoint({0.f, 0.5f});
    m_searchInput->setScale(0.8f);

    auto nextSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    auto prevSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    nextSpr->setScale(0.5f);
    prevSpr->setScale(0.5f);

    m_prev = CCMenuItemSpriteExtra::create(prevSpr, this, menu_selector(SavedPopup::onPrev));
    m_next = CCMenuItemSpriteExtra::create(nextSpr, this, menu_selector(SavedPopup::onNext));
    m_next->setRotation(180.f);
    m_prev->m_scaleMultiplier = 1.05f;
    m_next->m_scaleMultiplier = 1.05f;
    m_prev->setVisible(false);
    m_next->setVisible(m_totalItems > m_itemsPerPage);

    auto searchSpr = CCSprite::createWithSpriteFrameName("GJ_longBtn06_001.png");
    auto clearSpr = CCSprite::createWithSpriteFrameName("GJ_longBtn07_001.png");
    auto favSpr = CCSprite::createWithSpriteFrameName("gj_heartOff_001.png");

    searchSpr->setScale(0.8f);
    clearSpr->setScale(0.8f);
    favSpr->setScale(0.8f);

    m_searchBtn = CCMenuItemSpriteExtra::create(searchSpr, searchSpr, this, menu_selector(SavedPopup::onSearchInput));
    m_clearBtn = CCMenuItemSpriteExtra::create(clearSpr, clearSpr, this, menu_selector(SavedPopup::onClearInput));
    m_favOnlyBtn = CCMenuItemSpriteExtra::create(favSpr, favSpr, this, menu_selector(SavedPopup::onFavoriteFilter));
    m_searchBtn->m_scaleMultiplier = 0.85f;
    m_clearBtn->m_scaleMultiplier = 0.85f; 
    m_favOnlyBtn->m_scaleMultiplier = 0.85f;

    m_menu = CCMenu::create();
    m_menu->setContentSize({370.f, 180.f});
    m_menu->setLayout(
      RowLayout::create()
        ->setGap(5.f)
        ->setAxisAlignment(AxisAlignment::Start)
        ->setCrossAxisAlignment(AxisAlignment::End)
        ->setGrowCrossAxis(true)
        ->setCrossAxisOverflow(false)
        ->setAutoScale(false));

    m_mainLayer->addChildAtPosition(m_searchInput, Anchor::TopLeft, ccp(35.f, -40.f));
    m_mainLayer->addChildAtPosition(m_menu, Anchor::Center, ccp(0.f, -20.f));
    m_mainLayer->addChildAtPosition(m_pageLabel, Anchor::TopRight, ccp(-30.f, -10.f));

    auto optsMenu = this->m_closeBtn->getParent();
    optsMenu->addChildAtPosition(m_searchBtn, Anchor::TopRight, ccp(-145.f, -40.f));
    optsMenu->addChildAtPosition(m_clearBtn, Anchor::TopRight, ccp(-110.f, -40.f));
    optsMenu->addChildAtPosition(m_favOnlyBtn, Anchor::TopRight, ccp(-80.f, -40.f));
    optsMenu->addChildAtPosition(m_prev, Anchor::Left, ccp(20.f, 0.f));
    optsMenu->addChildAtPosition(m_next, Anchor::Right, ccp(-20.f, 0.f));

    updatePage(0);
    updatePageLabel();
    return true;
  }

  void onSearchInput(CCObject* sender) {
    std::string query = m_searchInput->getString();
    if (query.empty()) return;

    m_items = m_manager.getPaletteByName(m_searchInput->getString());
    if (m_items.empty()) {
      Notification::create("No palettes found", NotificationIcon::Error)->show();
      return;
    }

    m_page = 0;
    updateItems(false);
  }

  void onClearInput(CCObject* sender) {
    m_searchInput->setString("");
    updateItems();
  }

  void onFavoriteFilter(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra *>(sender);
    auto spr = static_cast<CCSprite *>(btn->getNormalImage());
    m_showFavOnly = !m_showFavOnly;

    updateBtnSprite(btn, m_showFavOnly);
    m_items = m_showFavOnly ? m_manager.getFavoritePalettes() : m_manager.load();
    m_page = 0;
    updateItems(false);
  }

  void onPrev(CCObject* sender) {
    updatePage(m_page - 1);
    updatePageLabel();
    updateNavButtons();
  }

  void onNext(CCObject* sender) {
    updatePage(m_page + 1);
    updatePageLabel();
    updateNavButtons();
  }

  void onFavoriteToggle(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra *>(sender);
    auto menu = static_cast<CCMenu *>(btn->getParent());
    std::string paletteId = static_cast<CCString*>(menu->getUserObject())->getCString();

    m_manager.setFavorite(paletteId);
    updateBtnSprite(btn, m_manager.isFavorite(paletteId));
  }

  void onRemove(CCObject *sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra *>(sender);
    auto menu = static_cast<CCMenu *>(btn->getParent());
    std::string paletteId = static_cast<CCString*>(menu->getUserObject())->getCString();

    geode::createQuickPopup(
      "Delete palette",
      "Are you sure you want to delete this palette?",
      "Cancel", "Delete", [this, paletteId](auto, bool btn2) {
        if (btn2) {
          m_manager.remove(paletteId);
          updateItems();
        }
      });
  }

  void updatePage(int page) {
    if (page < 0 || page > (m_totalItems - 1) / m_itemsPerPage) return;
    if (m_items.empty()) return;

    m_totalItems = m_items.size();
    m_page = page;
    m_menu->removeAllChildren();

    int start = page * m_itemsPerPage;
    int end = std::min(start + m_itemsPerPage, m_totalItems);

    for (int i = start; i < end; ++i) {
      geode::log::info("Adding palette index {} to page {}", i, page + 1);
      m_menu->addChild(colorPalette(m_items[i]));
    }
    m_menu->updateLayout();
  }

  void onColorSelect(CCObject *sender) {
    auto colorBtn = static_cast<CCMenuItemSpriteExtra *>(sender);
    auto colorSpr =static_cast<ColorChannelSprite *>(colorBtn->getNormalImage());
    auto color = colorSpr->getColor();

    ColorUtils::get().copyColor(color, sender);
  }

  void updatePageLabel() {
    m_pageLabel->setString(fmt::format("Page {}/{}", m_page + 1, (m_totalItems - 1) / m_itemsPerPage + 1).c_str());
  }

  void updateNavButtons() {
    m_prev->setVisible(m_page > 0);
    m_next->setVisible(m_page < (m_totalItems - 1) / m_itemsPerPage);
  }

  void updateBtnSprite(CCMenuItemSpriteExtra* btn, bool isFav) {
    auto favSpr = static_cast<CCSprite *>(btn->getNormalImage());
    favSpr->setDisplayFrame(
      CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(isFav ? "gj_heartOn_001.png" : "gj_heartOff_001.png")
    );
  }

  void updateItems(bool reload = true) {
    if (reload) {
      m_items = m_manager.load();
      m_totalItems = m_items.size();
      std::reverse(m_items.begin(), m_items.end());
      
      if (m_totalItems % m_itemsPerPage == 0 && m_page > 0) {
        m_page--;
      }
    }
    
    updatePage(m_page);
    updatePageLabel();
    updateNavButtons();
  }

  NineSlice *colorPalette(SavedPalette palette) {
    auto bg = NineSlice::create("square02b_001.png", {0.0f, 0.0f, 80.0f, 80.0f});
    bg->setContentSize({170.f, 70.f}); //150 x 70
    bg->setColor({130, 64, 33});
    bg->setScale(0.7f);

    auto label = CCLabelBMFont::create(palette.name.c_str(), "goldFont.fnt");
    label->setScale(0.35f);
    label->setAnchorPoint({0.f, 0.5f});
    bg->addChildAtPosition(label, Anchor::TopLeft, ccp(10.f, -10.f));

    auto colors = CCMenu::create();
    colors->setContentSize({140.f, 40.f});
    colors->setAnchorPoint({0.f, 0.5f});
    colors->setLayout(
      RowLayout::create()
      ->setGap(5.f)
      ->setAxisAlignment(AxisAlignment::Start)
      ->setCrossAxisAlignment(AxisAlignment::End)
      ->setGrowCrossAxis(true)
      ->setCrossAxisOverflow(false)
      ->setAutoScale(false));

    auto favBtnSpr = CCSprite::createWithSpriteFrameName(palette.isFavorite ? "gj_heartOn_001.png" : "gj_heartOff_001.png");
    auto removeBtnSpr = CCSprite::createWithSpriteFrameName("GJ_resetBtn_001.png");
    favBtnSpr->setScale(0.6f);
    removeBtnSpr->setScale(0.7f);

    auto favBtn = CCMenuItemSpriteExtra::create(favBtnSpr, this, menu_selector(SavedPopup::onFavoriteToggle));
    auto removeBtn = CCMenuItemSpriteExtra::create(removeBtnSpr, this, menu_selector(SavedPopup::onRemove));
    favBtn->m_scaleMultiplier = 0.65f;
    removeBtn->m_scaleMultiplier = 0.75f;

    auto menu = CCMenu::create();
    menu->setContentSize({150.f, 70.f});
    menu->setUserObject(CCString::create(palette.id));
    menu->addChildAtPosition(removeBtn, Anchor::Right, ccp(-5.f, -20.f));
    menu->addChildAtPosition(favBtn, Anchor::Right, ccp(-5.f, 0.f));

    bg->addChildAtPosition(colors, Anchor::TopLeft, ccp(10.f, -40.f));
    bg->addChildAtPosition(menu, Anchor::Center);

    for (int i = 0; i < palette.colors.size(); ++i) {
      auto hex = palette.colors[i].erase(0, 1); // remove # from hex string
      auto color = ColorChannelSprite::create();
      color->setColor(ColorSelectPopup::hexToColor(hex));
      color->setScale(0.5f);

      auto colorBtn = CCMenuItemSpriteExtra::create(color, this, menu_selector(SavedPopup::onColorSelect));
      colors->addChild(colorBtn);
    }
    
    colors->updateLayout();
    return bg;
  }
};