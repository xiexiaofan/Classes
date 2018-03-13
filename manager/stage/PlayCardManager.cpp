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
#include "RoomAnimationManager.hpp"
#include "SimpleAiActionManager.hpp"
#include "SimpleToastManager.hpp"
#include "CommonDefineSet.hpp"
#include "RoomDebugLayer.hpp"
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
    _card_manager = _manager->_card_manager;
    _code = PlayCode::Firstly;
    _need_ai_think = false;
}

void PlayCardManager::callbackForMing(Event*) {
    /** 流程
     * - panel, 明牌按钮变灰
     * - data, 玩家明牌标识置ture
     * - mutiply, 牌局倍数变化
     * - card, 玩家手牌UI变化
     */
    _cur_panel->setBtnEnabled(false, 0);
}

void PlayCardManager::callbackForTip(Event*) {
    int cur_id = _manager->getTargetId();
    std::vector<int> num_vec = _ai_manager->getPlayNumVec(cur_id);
    if (num_vec.empty())
        SimpleToastManager::getInstance()->playToast("当前无可出牌型");
    else
        _card_manager->refCardToSelected(num_vec, cur_id);
}

void PlayCardManager::callbackForPlay(Event*) {
    int cur_id = _manager->getTargetId();
    if (!_card_manager->callbackForPopCard(cur_id)) {
        SimpleToastManager::getInstance()->playToast("您选的牌不符合出牌规则");
        return;
    }
    _ai_manager->initPlaySeqVec();
    // 牌型动画播放判断
    // 牌型倍数变化判断
    if (_cur_panel) {
        _cur_panel->removeFromParent();
        _cur_panel = nullptr;
    }
    _code = PlayCode::Passive;
    _manager->plusTargetId(1);
    _card_manager->isEmptyForInsideCard(cur_id) ? endPlay() : startPlay();
}

void PlayCardManager::callbackForPass(Event*) {
    int cur_id = _manager->getTargetId();
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_pass, cur_id);
    
    _ai_manager->initPlaySeqVec();
    // 如果前一个玩家也未出牌时，下一个玩家进入主动出牌状态
    int pre_id = _manager->plusTargetId(2, true);
    if (_card_manager->isEmptyForOutsideCard(pre_id))
        _code = PlayCode::Initiative;
    _manager->plusTargetId(1);
    startPlay();
}

void PlayCardManager::callbackForTrust(Event*) {
    _ai_manager->setCurIdx(0);
    int cur_id = _manager->getTargetId();
    std::vector<int> num_vec = _ai_manager->getPlayNumVec(cur_id);
    
    if (num_vec.empty()) {
        callbackForPass(nullptr);
    } else {
        _card_manager->refCardToSelected(num_vec, cur_id);
        callbackForPlay(nullptr);
    }
}

bool PlayCardManager::init() {
    _data_manager = RoomDataManager::getInstance();
    _ai_manager   = SimpleAiActionManager::getInstance();
    this->scheduleUpdate();
    return true;
}

void PlayCardManager::update(float dt) {
    if (_need_ai_think == false)
        return;
    if (_ai_think_timer > 0) {
        _ai_think_timer -= dt;
        return;
    }
    runActionForAiThink();
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
    
    if (cur_id != 0) {
        _ai_think_timer = RandomHelper::random_int(1, 4);
        _need_ai_think = true;
    }
}

void PlayCardManager::runCodeInitiative() {
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
    
    if (cur_id != 0) {
        _ai_think_timer = RandomHelper::random_int(1, 4);
        _need_ai_think = true;
    }
}

void PlayCardManager::runCodePassive() {
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = RoomDataManager::getInstance()->getPlayerData(cur_id);
    _card_manager->clearCardFromOutside(cur_id);
    RoomAnimationManager::getInstance()->clearCurPosted(cur_id);
    if (_cur_panel) {
        _cur_panel->removeFromParent();
        _cur_panel = nullptr;
    }
    
    std::vector<int> num_vec = _ai_manager->getPlayNumVec(cur_id);
    if (num_vec.empty()) {
        _cur_panel = UIOptPanel::create(UIOptPanel::Type::PASS, cur_pdata);
        if (cur_id != 0)
            _cur_panel->setTimerRest(15);
    } else {
        _cur_panel = UIOptPanel::create(UIOptPanel::Type::PLAY_LIMIT, cur_pdata);
    }
    this->addChild(_cur_panel);
    
    if (cur_id != 0) {
        _ai_think_timer = RandomHelper::random_int(1, 4);
        _need_ai_think = true;
    }
}

void PlayCardManager::runActionForAiThink() {
    // 调用“托管出牌”
    _need_ai_think = false;
    callbackForTrust(nullptr);
}

void PlayCardManager::endPlay() {
    if (_cur_panel) {
        _cur_panel->removeFromParent();
        _cur_panel = nullptr;
    }
    if (_manager->_debug_layer)
        _manager->_debug_layer->updateCode("");
    // TODD
    SimpleToastManager::getInstance()->playToast("本局游戏结束");
}
