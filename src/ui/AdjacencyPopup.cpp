#include "AdjacencyPopup.hpp"
#include "../builders/SpriteBuilder.hpp"

AdjacencyPopup *AdjacencyPopup::create(int colors) {
  auto popup = new AdjacencyPopup();
  if (popup->init(colors)) {
    popup->autorelease();
    return popup;
  }
  delete popup;
  return nullptr;
}

bool AdjacencyPopup::init(int colors) {
  if (!Popup::init(300.f, 260.f)) return false;
  this->setTitle("Adjacency settings");
  m_colors = colors - 1;

  int tag = 0;
  int btnsPerRow = m_colors;
  float itemWidth = 40.f;
  float scale = 0.5f;

  m_labels = CCArray::create();

  CCMenuItemToggler *toggler = CCMenuItemToggler::createWithSize(
      "GJ_checkOff_001.png", "GJ_checkOn_001.png", this,
      menu_selector(AdjacencyPopup::onGradientToggle), 0.5f);
  toggler->setPosition({100, 100});
  toggler->toggle(settings.isGradientActive());
  m_closeBtn->getParent()->addChildAtPosition(toggler, Anchor::Top,
                                              ccp(-50.f, -40.f));

  CCLabelBMFont *label = CCLabelBMFont::create("Gradient mode", SpriteBuilder::bigFontName);
  label->setScale(0.4f);
  label->setAnchorPoint(ccp(0.f, 0.5f));
  m_mainLayer->addChildAtPosition(label, Anchor::Top, ccp(-30.f, -40.f));

  m_slider = Slider::create(this, menu_selector(AdjacencyPopup::onSlider), 1.0f);
  m_slider->setPosition({200, 200});
  m_slider->setValue(0.5f);
  m_slider->setScale(0.6f);
  m_slider->setAnchorPoint(ccp(0.f, 0.f));
  m_slider->setRotation(-90.f);
  m_mainLayer->addChildAtPosition(m_slider, Anchor::Right, ccp(-20.f, 0.f));

  m_grid = CCNode::create();
  m_grid->setAnchorPoint(ccp(0.5f, 0.5f));
  m_grid->setContentSize({itemWidth * m_colors / 2, itemWidth * m_colors / 2});
  m_grid->setLayout(ColumnLayout::create()
                        ->setAxisAlignment(AxisAlignment::End)
                        ->setCrossAxisAlignment(AxisAlignment::End)
                        ->setGap(0.f)
                        ->setAxisReverse(true)
                        ->setAutoScale(true));

  CCNode *index_x = CCNode::create();
  index_x->setContentSize({m_grid->getContentSize().width, itemWidth});
  index_x->setAnchorPoint(ccp(0.5f, 0.5f));
  index_x->setLayout(RowLayout::create()
                         ->setAxisAlignment(AxisAlignment::Even)
                         ->setCrossAxisAlignment(AxisAlignment::Even)
                         ->setGap(0.f)
                         ->setAutoScale(false));

  CCNode *index_y = CCNode::create();
  index_y->setContentSize({itemWidth, m_grid->getContentSize().height});
  index_y->setAnchorPoint(ccp(0.5f, 0.5f));
  index_y->setLayout(ColumnLayout::create()
                         ->setAxisAlignment(AxisAlignment::Even)
                         ->setCrossAxisAlignment(AxisAlignment::Even)
                         ->setGap(0.f)
                         ->setAxisReverse(true)
                         ->setAutoScale(false));

  if (colors > 10) {
    m_grid->setScale(0.8f);
    index_x->setScale(0.8f);
    index_y->setScale(0.8f);
  }

  m_mainLayer->addChildAtPosition(index_y, Anchor::Center, ccp((-m_grid->getContentSize().width / 2 - 10.f) * m_grid->getScale(), -30.f));
  m_mainLayer->addChildAtPosition(
      index_x, Anchor::Center, ccp(0.f, (m_grid->getContentSize().height / 2 + 10.f) * m_grid->getScale() - 30.f));
  m_mainLayer->addChildAtPosition(m_grid, Anchor::Center, ccp(0.f, -30.f));

  for (int i = 0; i < m_colors; i++) {
    auto row = CCMenu::create();
    row->setContentSize({itemWidth * m_colors / 2, itemWidth});
    row->setLayout(RowLayout::create()
                       ->setAxisAlignment(AxisAlignment::End)
                       ->setCrossAxisAlignment(AxisAlignment::End)
                       ->setGap(0.f)
                       ->setAutoScale(false));

    CCLabelBMFont *x = CCLabelBMFont::create(numToString(i + 1).c_str(), SpriteBuilder::goldFontName);
    x->setScale(0.4f);
    index_x->addChild(x);

    CCLabelBMFont *y = CCLabelBMFont::create(numToString(i).c_str(), SpriteBuilder::goldFontName);
    y->setScale(0.4f);
    index_y->addChild(y);

    for (int j = 0; j < btnsPerRow; j++) {
      tag = i * m_colors + j;
      CCSprite *spr = CCSprite::create("GJ_button_05.png");
      spr->setScale(scale);

      CCLabelBMFont *label = CCLabelBMFont::create(
        numToString(settings.getAdjacency(tag / m_colors, (tag + tag / m_colors) % m_colors + 1)).c_str(), 
        SpriteBuilder::bigFontName);
      label->setScale(0.3f);
      m_labels->addObject(label);

      auto cell = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AdjacencyPopup::onCellClick));
      cell->setTag(tag);
      cell->addChildAtPosition(label, Anchor::Center);
      row->addChild(cell);
    }
    btnsPerRow--;
    row->updateLayout();
    m_grid->addChild(row);
  }
  m_grid->updateLayout();

  index_x->updateLayout();
  index_y->updateLayout();
  return true;
}

void AdjacencyPopup::updateGrid() {
  auto labels = m_labels->asExt<CCLabelBMFont *>();
  for (auto label : labels) {
    int tag = label->getParent()->getTag();
    int i = tag / m_colors;
    int j = (tag + i) % m_colors + 1;

    if (label) {
      label->setString(numToString(settings.getAdjacency(i, j)).c_str());
    }
  }
}

void AdjacencyPopup::onCellClick(CCObject *sender) {
  auto cell = static_cast<CCMenuItemSpriteExtra *>(sender);
  m_valueLabel = static_cast<CCLabelBMFont *>(cell->getChildByIndex(1));

  if (m_cell) {
    CCSprite *prevSpr = static_cast<CCSprite *>(m_cell->getNormalImage());
    prevSpr->initWithFile("GJ_button_05.png");
  }

  CCSprite *spr = static_cast<CCSprite *>(cell->getNormalImage());
  spr->initWithFile("GJ_button_02.png");
  m_cell = cell;

  auto result = numFromString<int>(m_valueLabel->getString());
  if (result.isOk()) {
    int value = result.unwrap();
    m_slider->setValue(value / 100.f);
  }
}

void AdjacencyPopup::onSlider(CCObject *sender) {
  if (!m_cell) return;
  auto thumb = static_cast<SliderThumb *>(sender);
  float value = static_cast<int>(thumb->getValue() * 100);

  if (m_valueLabel) {
    int i = m_cell->getTag() / m_colors;
    int j = (m_cell->getTag() + i) % m_colors + 1;

    settings.setAdjacency(i, j, value);
    m_valueLabel->setString(numToString(value).c_str());
  }
}

void AdjacencyPopup::onGradientToggle(CCObject *sender) {
  auto toggler = static_cast<CCMenuItemToggler *>(sender);
  bool isGradient = toggler->isToggled();
  settings.toggleGradient();

  if (!isGradient) settings.gradient();
  else settings.palette();
  updateGrid();
}