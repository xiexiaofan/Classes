//
//  SimpleToastManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/9.
//

#include "SimpleToastManager.hpp"
#include "ui/UIScale9Sprite.h"
using namespace cocos2d;

static SimpleToastManager* Instance = nullptr;

SimpleToastManager::SimpleToastManager()
: _need_toast(false)
, _carrier(nullptr)
{
    auto scheduler = Director::getInstance()->getScheduler();
    scheduler->schedule(CC_SCHEDULE_SELECTOR(SimpleToastManager::update), this, 0.01f, false);
}

SimpleToastManager* SimpleToastManager::getInstance()
{
    if (!Instance)
        Instance = new SimpleToastManager();
    Instance->bindCarrierNode(Director::getInstance()->getRunningScene());
    return Instance;
}

void SimpleToastManager::destroyInstance()
{
    if (!Instance)
        return;
    delete Instance;
    Instance = nullptr;
}

void SimpleToastManager::bindCarrierNode(Node* node)
{
    if (_carrier == node)
        return;
    // 切换场景时，清除旧场景上的toast播放列表
    for (auto child : _delay_toast_vec) child->removeFromParent();
    for (auto child : _active_toast_vec) child->removeFromParent();
    _delay_toast_vec.clear();
    _active_toast_vec.clear();
    _carrier = node;
}

void SimpleToastManager::playToast(const std::string& content)
{
    playToast(content, 2.0f, 0.0f);
}

void SimpleToastManager::playToast(const std::string& content, float dur, float delay)
{
    if (!_carrier)
        return;
    
    SingleToastCreator* cur_toast = SingleToastCreator::create(content, dur, delay);
    _carrier->addChild(cur_toast, 100);
    _delay_toast_vec.pushBack(cur_toast);
    _need_toast = true;
}

void SimpleToastManager::update(float dt)
{
    if (!_need_toast)
        return;
    // 移除播放完成的toast
    auto iter = _active_toast_vec.begin();
    while (iter != _active_toast_vec.end())
    {
        auto& toast = *iter;
        toast->setDuration(toast->getDuration() - dt);
        if (toast->getDuration() > 0)
            ++iter;
        else
        {
            toast->runAnimationForDisappear();
            iter = _active_toast_vec.erase(iter);
        }
    }
    // 加入待播放的toast
    iter = _delay_toast_vec.begin();
    while (iter != _delay_toast_vec.end())
    {
        auto& toast = *iter;
        toast->setDelayCount(toast->getDelayCount() - dt);
        if (toast->getDelayCount() > 0)
            ++iter;
        else
        {
            for (auto& pre_toast : _active_toast_vec)
                pre_toast->runAnimationForRise();
            toast->runAnimationForEnter();
            _active_toast_vec.pushBack(toast);
            iter = _delay_toast_vec.erase(iter);
        }
    }
    
    _need_toast = !_active_toast_vec.empty() || !_delay_toast_vec.empty();
}


SingleToastCreator* SingleToastCreator::create(const std::string& content, float dur, float delay)
{
    SingleToastCreator* ret = new SingleToastCreator();
    if (ret && ret->init(content, dur, delay))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

void SingleToastCreator::runAnimationForEnter()
{
    this->setVisible(true);
    for (auto& child : this->getChildren())
        child->runAction(FadeIn::create(0.5f));
    this->runAction(MoveBy::create(0.25f, Vec2(0, 50)));
}

void SingleToastCreator::runAnimationForRise()
{
    this->runAction(MoveBy::create(0.25f, Vec2(0, 50)));
}

void SingleToastCreator::runAnimationForDisappear()
{
    for (auto& child : this->getChildren())
        child->runAction(FadeOut::create(0.5f));
    CallFunc* call = CallFunc::create([&]{this->removeFromParent();});
    this->runAction(Sequence::create(DelayTime::create(0.5f), call, nullptr));
}

bool SingleToastCreator::init(const std::string& content, float dur, float delay)
{
    _dur = dur;
    _delay = delay;
    
    Label* label = Label::createWithSystemFont(content, "", 24);
    label->setColor(Color3B(0x54, 0x28, 0x1d));
    
    ui::Scale9Sprite* image = ui::Scale9Sprite::create("alert_toast.png");
    image->setCapInsets(Rect(15, 15, 8, 8));
    image->setContentSize(label->getContentSize() + Size(35, 20));
    this->addChild(image, 0);
    this->addChild(label, 1);
    
    this->setVisible(false);
    this->setPosition(Director::getInstance()->getVisibleSize() * 0.5);
    
    return true;
}
