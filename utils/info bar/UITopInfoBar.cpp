//
//  UITopInfoBar.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#include "UITopInfoBar.hpp"
#include "PlayerData.hpp"
#include "RoomData.hpp"
#include "CommonMethodSet.hpp"
#include "ui/UIScale9Sprite.h"
using namespace cocos2d;
using ui::Scale9Sprite;

static Color4B Label_Colol = Color4B(0x00, 0x00, 0x00, 175);
static float Timer_AnutoDisplay = 3.0f;

void UITopInfoBar::displayForGameNotBegin() {
    updateAnte();
    updatemultiple();
    
    _is_gaming = false;
    displayClock();
}

void UITopInfoBar::displayForGameBegin() {
    updateAnte();
    updatemultiple();
    
    _is_gaming = true;
    displayRestCard();
}

void UITopInfoBar::updateAnte() {
    if (!_ante_num) {
        _ante_num = Label::createWithSystemFont("", "", 21);
        _ante_num->setTextColor(Label_Colol);
        _ante_num->setHorizontalAlignment(TextHAlignment::LEFT);
        _ante_num->setPosition(-108, 8);
        this->addChild(_ante_num);
    }
    std::string val = std::to_string(RoomDataManager::getInstance()->getRoomConfig().ante);
    _ante_num->setString(val);
}

void UITopInfoBar::updatemultiple() {
    if (!_multiple_num) {
        _multiple_num = Label::createWithSystemFont("", "", 21);
        _multiple_num->setTextColor(Label_Colol);
        _multiple_num->setHorizontalAlignment(TextHAlignment::LEFT);
        _multiple_num->setPosition(160, 8);
        this->addChild(_multiple_num);
    }
    std::string val = std::to_string(RoomDataManager::getInstance()->getPdataMap().at(0)->getMultiple());
    _multiple_num->setString(val);
}

void UITopInfoBar::displayClock() {
    if (!_clock) {
        _clock = Label::createWithSystemFont("", "", 21);
        _clock->setColor(Color3B(0xff, 0xff, 0xff));
        _clock->setHorizontalAlignment(TextHAlignment::LEFT);
        _clock->setPositionY(8);
        this->addChild(_clock);
    }
    _is_display_clock = true;
    _clock->setVisible(true);  // TODD
}

void UITopInfoBar::displayRestCard() {
    _is_display_clock = false;
    _clock->setVisible(false);  // TODD
}

bool UITopInfoBar::init() {
    // background
    Scale9Sprite* bg = Scale9Sprite::createWithSpriteFrameName("table_ui_02.png");
    bg->setCapInsets(Rect(50, 5, 41, 30));
    bg->setContentSize(Size(412, 52));
    this->addChild(bg);
    Scale9Sprite* bg_middle = Scale9Sprite::createWithSpriteFrameName("table_ui_01.png");
    bg_middle->setCapInsets(Rect(10, 10, 12, 16));
    bg_middle->setContentSize(Size(158, 36));
    bg_middle->setPositionY(8);
    this->addChild(bg_middle);
    
    // _contentsize
    this->setContentSize(bg_middle->getContentSize());
    
    // font label
    Label* ante_font = Label::createWithSystemFont("底分", "", 21);
    ante_font->setTextColor(Label_Colol);
    ante_font->setPosition(-160, 8);
    this->addChild(ante_font);
    Label* times_font = Label::createWithSystemFont("倍数", "", 21);
    times_font->setTextColor(Label_Colol);
    times_font->setPosition(110, 8);
    this->addChild(times_font);
    
    // listener
    _listener = EventListenerTouchOneByOne::create();
    _listener->onTouchBegan = [&](Touch* touch, Event* event)->bool{
        if (_xxf::isTouchPointAtValidScope(touch, this, Size(20, 30)) && _is_gaming) {
            _is_display_clock ? this->runAnimationFromClockToRestCard()
            : this->runAnimationFromRestCardToClock();
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
    
    // schedule
    this->scheduleUpdate();
    
    return true;
}

void UITopInfoBar::runAnimationFromClockToRestCard() {
    
}

void UITopInfoBar::runAnimationFromRestCardToClock() {
    
}

void UITopInfoBar::update(float dt) {
    if (!_is_display_clock || !_clock)
        return;
    _clock->setString(adjustFormatForClockStr());
    
    if (!_is_gaming)
        return;
    
    if (Timer_AnutoDisplay > 0) {
        Timer_AnutoDisplay -= dt;
    } else {
        runAnimationFromClockToRestCard();
        Timer_AnutoDisplay = 3.0f;
    }
}

std::string UITopInfoBar::adjustFormatForClockStr() const {
    std::string ret = _xxf::getCurrentLocalTime();
    ret.insert(ret.begin() + 2, ':');
    ret.insert(ret.end() - 2, ':');
    return ret;
}
