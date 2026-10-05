#pragma once
#include "../utils/ColorUtils.hpp"

using namespace geode::prelude;

class HsvWidgetHost : public HSVWidgetDelegate {
public:
    void hsvChanged(ConfigureHSVWidget* widget) override {
        if (!widget) return;
        onHsvValueChanged(widget->m_hsv);
    }

protected:
    ConfigureHSVWidget* m_hsvWidget = nullptr;

    void createHsvWidget(CCNode* parent, Anchor anchor = Anchor::BottomLeft, const CCPoint& offset = CCPointZero) {
        m_hsvWidget = ConfigureHSVWidget::create(ColorUtils::HSV_IDENTITY, false, true);
        m_hsvWidget->m_delegate = this;
        m_hsvWidget->setZOrder(10);
        m_hsvWidget->setVisible(false);
        parent->addChildAtPosition(m_hsvWidget, anchor, offset);
    }

    void resetHsvWidget() {
        if (!m_hsvWidget) return;
        m_hsvWidget->m_hsv = ColorUtils::HSV_IDENTITY;
        m_hsvWidget->updateSliders();
        m_hsvWidget->updateLabels();
    }

    // called whenever the widget's value changes
    virtual void onHsvValueChanged(const ccHSVValue& value) {}
};
