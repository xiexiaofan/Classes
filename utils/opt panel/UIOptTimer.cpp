//
//  UIOptTimer.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#include "UIOptTimer.hpp"
using namespace cocos2d;

// 背景图半径 & 圆环半径
static const float Image_Radius  = 36.0f;
static const float Circle_Radius = 40.0f;

UIOptTimer* UIOptTimer::create(const std::string& image, float dur, const callbackForEnd &callback) {
    UIOptTimer* ret = new UIOptTimer();
    if (ret && ret->init(image, dur, callback))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

UIOptTimer::UIOptTimer()
: _enabled(false)
, _dur(1.0f)
, _callback(nullptr)
, _move_star(nullptr)
{}

UIOptTimer::~UIOptTimer()
{}

bool UIOptTimer::init(const std::string& image, float dur, const callbackForEnd& callback) {
    _dur = dur + 0.5;  // 0.5s缓冲时间
    _callback = callback;
    // 添加 计时器
    Sprite* sp = Sprite::createWithSpriteFrameName("room_clock_bg_1.png");
    this->addChild(sp);
    Sprite* sp2 = Sprite::createWithSpriteFrameName("room_clock_bg_2.png");
    _timer = ProgressTimer::create(sp2);
    _timer->setType(ProgressTimer::Type::RADIAL);
    this->addChild(_timer);
    
    // 添加 背景图（裁剪成圆形）
    Sprite* image_bg = Sprite::create(StringUtils::format("image/icon/%s", image.c_str()));
    image_bg->setScale(Image_Radius * 2 / image_bg->getContentSize().width,
                       Image_Radius * 2 / image_bg->getContentSize().height);
    DrawNode* circle = DrawNode::create();
    circle->drawSolidCircle(Vec2::ZERO, Image_Radius, M_PI * 2, 50, 1.0f, 1.0f, Color4F::BLACK);
    ClippingNode* clip = ClippingNode::create(circle);
    clip->addChild(image_bg);
    this->addChild(clip);
    
    // 添加 移动光效
    _move_star = Sprite::create();
    _move_star->setPositionY(Circle_Radius);
    this->addChild(_move_star);
    CallFunc* call = CallFunc::create([this]{_move_star->initWithSpriteFrameName("particle_star_01.png");});
    CallFunc* call_2 = CallFunc::create([this]{_move_star->initWithSpriteFrameName("particle_star_02.png");});
    Sequence* ac = Sequence::create(call, DelayTime::create(0.1f), call_2, DelayTime::create(0.5f), nullptr);
    _move_star->runAction(RepeatForever::create(ac));
    
    this->scheduleUpdate();
    
    return true;
}

void UIOptTimer::update(float dt) {
    if (!_enabled)
        return;
    
    if (_timer->getPercentage() < 100) {
        _timer->setPercentage(_timer->getPercentage() + dt / _dur * 100);
        float angle = _timer->getPercentage() * 2 * M_PI / 100;
        _move_star->setPosition(Vec2(sinf(angle) * Circle_Radius, cosf(angle) * Circle_Radius));
        return;
    }
    
    if (_callback) {
        _callback(this);
    }
    _enabled = false;
}
