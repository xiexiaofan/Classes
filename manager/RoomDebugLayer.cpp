//
//  RoomDebugLayer.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/26.
//

#include "RoomDebugLayer.hpp"
#include "GameManager.hpp"
#include "RoomCardManager.hpp"
#include "DealCardManager.hpp"
#include "SelectManager.hpp"
#include "PlayCardManager.hpp"
#include "UIOptPanel.hpp"
#include "RoomScene.hpp"
#include "RoomAnimationManager.hpp"
#include "SimpleToastManager.hpp"
#include "CommonDefineSet.hpp"
using namespace cocos2d;
using namespace cocos2d::ui;

void RoomDebugLayer::callbackForReDeal(Ref*) {
    RoomAnimationManager::getInstance()->clearAllPosted();
    
    if (_manager->_stage == GameManager::StageCode::DEAL)
        _manager->_deal_manager->startDeal();
    else if (_manager->_stage == GameManager::StageCode::SELECT) {
        _manager->_scene->rmIdlogoForPlayer();
        _manager->_select_manager->removeAllChildren();
        _manager->_select_manager->endSelect();
    }
    else if (_manager->_stage == GameManager::StageCode::PLAY) {
        _manager->_play_manager->endPlay();
        _manager->_scene->rmIdlogoForPlayer();
        _manager->_deal_manager->startDeal();
    }
    else
        return;
}

void RoomDebugLayer::callbackForRePick(Ref*) {
    if (_manager->_stage == GameManager::StageCode::PLAY)
        _manager->_card_manager->resetSelected(_manager->_target_id);
    else
        SimpleToastManager::getInstance()->playToast("只支持在出牌阶段重选");
}

void RoomDebugLayer::callbackForUpBtn(Ref*) {
    if (_code == _xxf::en_call)
        _manager->_select_manager->callbackForCall(nullptr);
    else if (_code == _xxf::en_rob)
        _manager->_select_manager->callbackForRob(nullptr);
    else if (_code == _xxf::en_play)
        _manager->_play_manager->_cur_panel->dispatchOptEvent(_xxf::en_play);
    else
        SimpleToastManager::getInstance()->playToast("发起进攻：叫地主，抢地主，出牌");
}

void RoomDebugLayer::callbackForDownBtn(Ref*) {
    if (_code == _xxf::en_call)
        _manager->_select_manager->callbackForNocall(nullptr);
    else if (_code == _xxf::en_rob)
        _manager->_select_manager->callbackForNorob(nullptr);
    else if (_code == _xxf::en_play)
        _manager->_play_manager->_cur_panel->dispatchOptEvent(_xxf::en_pass);
    else
        SimpleToastManager::getInstance()->playToast("全军撤退：不叫，不抢，不出");
}

void RoomDebugLayer::updateCode(const std::string& code) {
    _code = code;
    if (_code == _xxf::en_call) {
        _up_btn->setTitleText("叫地主");
        _down_btn->setTitleText("不叫");
    } else if (_code == _xxf::en_rob) {
        _up_btn->setTitleText("抢地主");
        _down_btn->setTitleText("不抢");
    } else if (_code == _xxf::en_play) {
        _up_btn->setTitleText("出牌");
        _down_btn->setTitleText("不出");
    } else {  // ""
        _up_btn->setTitleText("进攻");
        _down_btn->setTitleText("防守");
    }
}

bool RoomDebugLayer::init() {
    if (!Layer::init())
        return false;
    
    _repick = createLazyBtn("重选", CC_CALLBACK_1(RoomDebugLayer::callbackForRePick, this));
    _repick->setPosition(Vec2(_repick->getContentSize().width*0.5, 310));
    this->addChild(_repick);
    
    _redeal = createLazyBtn("重发", CC_CALLBACK_1(RoomDebugLayer::callbackForReDeal, this));
    _redeal->setPosition(Vec2(_redeal->getContentSize().width*0.5, 240));
    this->addChild(_redeal);
    
    const float size_x = Director::getInstance()->getVisibleSize().width;
    _up_btn = createLazyBtn("进攻", CC_CALLBACK_1(RoomDebugLayer::callbackForUpBtn, this));
    _up_btn->setPosition(Vec2(size_x-_up_btn->getContentSize().width*0.5, 310));
    this->addChild(_up_btn);

    _down_btn = createLazyBtn("防守", CC_CALLBACK_1(RoomDebugLayer::callbackForDownBtn, this));
    _down_btn->setPosition(Vec2(size_x-_down_btn->getContentSize().width*0.5, 240));
    this->addChild(_down_btn);
    
    return true;
}

Button* RoomDebugLayer::createLazyBtn(const std::string& name,
                                      const std::function<void(cocos2d::Ref*)>& callback) {
    Button* ret = Button::create();
    ret->loadTextureNormal("btm_a_yellow.png", Button::TextureResType::PLIST);
    
    ret->setTitleText(name);
    ret->setTitleFontSize(21);
    ret->setTitleColor(Color3B::RED);
    
    ret->setScale9Enabled(true);
    ret->setContentSize(Size(80, 49));
    
    ret->setTouchEnabled(true);
    ret->setZoomScale(-0.1);
    ret->addClickEventListener(callback);
    
    return ret;
}
