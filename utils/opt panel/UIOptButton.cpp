//
//  UIOptButton.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#include "UIOptButton.hpp"
#include "CommonMethodSet.hpp"
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
using namespace cocos2d;

UIOptButton* UIOptButton::create(const std::string& fn,
                                 const std::string& bg,
                                 const callbackForClicked& callback) {
    UIOptButton* ret = new UIOptButton();
    if (ret && ret->init(fn, bg, callback))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

UIOptButton::UIOptButton()
: _fc(nullptr)
, _bg(nullptr)
, _callback(nullptr)
, _enabled(true)
, _need_remove(false)
, _user_scale(Vec2(1, 1))
{}

UIOptButton::~UIOptButton()
{}

void UIOptButton::setEnabled(bool enabled) {
    if (_enabled == enabled)
        return;
    _enabled = enabled;
    runAnimationForTouchEnabled(_enabled);
}

void UIOptButton::setUserScale(const cocos2d::Vec2& scale) {
    _user_scale = scale;
    this->setScale(_user_scale.x, _user_scale.y);
}

bool UIOptButton::init(const std::string& fn,
                       const std::string& bg,
                       const callbackForClicked& callback) {
    // 添加 背景
    std::string fc_str = "btn_" + bg + "_fonts_" + fn + ".png";
    std::string bg_str = "btn_z_" + bg + ".png";
    _bg = Sprite::createWithSpriteFrameName(bg_str);
    _fc = Sprite::createWithSpriteFrameName(fc_str);
    if (!_bg || !_fc)
        return false;
    this->addChild(_bg);
    this->addChild(_fc);
    this->setContentSize(_bg->getContentSize());
    
    // 添加 点击回调函数
    _callback = callback;
    
    // 注册 监听器
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(UIOptButton::touchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(UIOptButton::touchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void UIOptButton::runAnimationForPressed(float zoom) {
    ScaleTo* ac = ScaleTo::create(0.1f, zoom * _user_scale.x, zoom * _user_scale.y);
    ac->setTag(10000);
    this->stopActionByTag(10000);
    this->runAction(ac);
}

void UIOptButton::runAnimationForTouchEnabled(bool enabled) {
    SpriteFrame* sf = nullptr;
    if (_enabled)
        sf = SpriteFrameCache::getInstance()->getSpriteFrameByName("btn_z_yellow.png");
    else
        sf = SpriteFrameCache::getInstance()->getSpriteFrameByName("btn_z_gray.png");
    _bg->setSpriteFrame(sf);
}

bool UIOptButton::touchBegan(Touch* touch, Event* event) {
    if (_enabled && _xxf::isTouchPointAtValidScope(touch, this)) {
        _bg->setColor(Color3B::GRAY);
        _fc->setColor(Color3B::GRAY);
        runAnimationForPressed(0.88);
        SimpleAudioEngine::getInstance()->playEffect("sound/effect/button.mp3");
        return true;
    }
    return false;
}

void UIOptButton::touchEnded(Touch* touch, Event* event) {
    _bg->setColor(Color3B::WHITE);
    _fc->setColor(Color3B::WHITE);
    bool touch_succes = _xxf::isTouchPointAtValidScope(touch, this, 1.12);   // 0.9 * 1.1 = 0.99
    CallFunc* call_1 = CallFunc::create([this]{this->runAnimationForPressed(1.0);});
    CallFunc* call_2 = touch_succes && _callback ? CallFunc::create([this]{_callback(this);}) : nullptr;
    CallFunc* call_3 = _need_remove ? CallFunc::create([this]{this->removeFromParent();}) : nullptr;
    this->runAction(Sequence::create(call_1, DelayTime::create(0.1f), call_2, call_3, nullptr));
}
