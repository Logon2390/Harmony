#pragma once


using namespace geode::prelude;

struct MenuAction {
    std::string title;
    SEL_MenuHandler action;
    SEL_MenuHandler onInfo;
    SEL_MenuHandler onPrevious = nullptr; 
    SEL_MenuHandler onNext = nullptr;
    CCNode* node = nullptr;
    
    ~MenuAction() = default;
};

enum class SelectorType {
    Button,
    Option,
};

namespace ComponentBuilder {
    CCNode* createOptionSelector(const char *title, float width, SelectorType type, MenuAction action, CCObject* target);
}