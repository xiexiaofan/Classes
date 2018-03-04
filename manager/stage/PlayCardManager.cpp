//
//  PlayCardManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#include "PlayCardManager.hpp"
#include "UIOptPanel.hpp"
#include "RoomData.hpp"
#include "PlayerData.hpp"
#include "RoomCardManager.hpp"
#include "RoomDebugLayer.hpp"
#include "RoomAnimationManager.hpp"
#include "SimpleToastManager.hpp"
#include "CommonDefineSet.hpp"
using namespace cocos2d;

void PlayCardManager::startPlay() {
    switch (_code) {
        case PlayCode::Firstly:    runCodeFirstly();    break;
        case PlayCode::Initiative: runCodeInitiative(); break;
        case PlayCode::Passive:    runCodePassive();    break;
        default: break;
    }
}

void PlayCardManager::updatePlayConfig() {
    _card_manager = _manager->getCardManager();
    _code = PlayCode::Firstly;
}

void PlayCardManager::callbackForMing(Event*) {
    /** 流程
     * - panel, 明牌按钮变灰
     * - data, 玩家明牌标识置ture
     * — mutiply, 牌局倍数变化
     * - card, 玩家手牌UI变化
     */
    _cur_panel->setBtnEnabled(false, 0);
}

void PlayCardManager::callbackForTip(Event*) {
    SimpleToastManager::getInstance()->playToast("提示功能尚未完善");
}

void PlayCardManager::callbackForPlay(Event*) {
    int cur_id = _manager->getTargetId();
    if (!_card_manager->callbackForPopCard(cur_id)) {
        SimpleToastManager::getInstance()->playToast("您选的牌不符合出牌规则");
        return;
    }
    
    // 牌型动画播放判断
    // 牌型倍数变化判断
    if (_cur_panel) {
        _cur_panel->removeFromParent();
        _cur_panel = nullptr;
    }
    _manager->plusTargetId(1);
    _code = PlayCode::Passive;
    _card_manager->isEmptyForInsideCard(cur_id) ? endPlay() : startPlay();
}

void PlayCardManager::callbackForPass(Event*) {
    int cur_id = _manager->getTargetId();
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_pass, cur_id);
    
    // 如果前一个玩家也未出牌时，下一个玩家进入主动出牌状态
    if (_card_manager->isEmptyForOutsideCard(_manager->minusTargetId(cur_id, true)));
        _code = PlayCode::Initiative;
    _manager->plusTargetId(1);
    startPlay();
}

void PlayCardManager::callbackForTrust(Event*) {
    ; // 托管
    SimpleToastManager::getInstance()->playToast("托管？其实就是不出牌");
    callbackForPass(nullptr);
}

bool PlayCardManager::init() {
    return true;
}

void PlayCardManager::runCodeFirstly() {
    // debug
    if (_manager->_debug_layer)
        _manager->_debug_layer->updateCode(_xxf::en_play);
    
    // 获取地主座次id
    int cur_id = -1;
    while (!RoomDataManager::getInstance()->getPlayerData(++cur_id)->isLandlord()) continue;
    _manager->resetTargetId(cur_id);
    log("地主是%d号", cur_id);
    
    PlayerData* cur_pdata = RoomDataManager::getInstance()->getPlayerData(cur_id);
    _cur_panel = UIOptPanel::create(UIOptPanel::Type::MING, cur_pdata);
    this->addChild(_cur_panel);
    
    // TODD: need ai action
}

void PlayCardManager::runCodeInitiative() {
    // 清空出牌区
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = RoomDataManager::getInstance()->getPlayerData(cur_id);
    _card_manager->clearCardFromOutside(cur_id);
    RoomAnimationManager::getInstance()->clearCurPosted(cur_id);
    if (_cur_panel) {
        _cur_panel->removeFromParent();
        _cur_panel = nullptr;
    }
    
    _cur_panel = UIOptPanel::create(UIOptPanel::Type::PLAY, cur_pdata);
    this->addChild(_cur_panel);
    
    // TODD: need ai action
}

void PlayCardManager::runCodePassive() {
    // 清空出牌区
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = RoomDataManager::getInstance()->getPlayerData(cur_id);
    _card_manager->clearCardFromOutside(cur_id);
    RoomAnimationManager::getInstance()->clearCurPosted(cur_id);
    if (_cur_panel) {
        _cur_panel->removeFromParent();
        _cur_panel = nullptr;
    }
    
    // TODD：panel type还有“要不起”这种可能，需要判断
    _cur_panel = UIOptPanel::create(UIOptPanel::Type::PLAY, cur_pdata);
    this->addChild(_cur_panel);
    
    // TODD: need ai action
}

void PlayCardManager::endPlay() {
    if (_cur_panel) {
        _cur_panel->removeFromParent();
        _cur_panel = nullptr;
    }
    if (_manager->_debug_layer)
        _manager->_debug_layer->updateCode("");
    
    // 是否结算
    SimpleToastManager::getInstance()->playToast("本局游戏结束");
}
