//
//  SelectManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#include "SelectManager.hpp"
#include "UIOptPanel.hpp"
#include "PlayerData.hpp"
#include "RoomData.hpp"
#include "RoomCardManager.hpp"
#include "RoomScene.hpp"
#include "CommonDefineSet.hpp"
#include "RoomAnimationManager.hpp"
#include "SimpleToastManager.hpp"
#include "RoomDebugLayer.hpp"
using namespace cocos2d;

void SelectManager::startSelect() {
    CC_ASSERT(_cur_opt != _opt_list.end());
    debugJudge();
    switch (*_cur_opt) {
        case OptionCode::CALL:   runForCodeCall();   break;
        case OptionCode::BET:    runForCodeBet();    break;
        case OptionCode::ROB:    runForCodeRob();    break;
        case OptionCode::DOUBLE: runForCodeDouble(); break;
        default: break;
    }
}

void SelectManager::updateSelectConfig() {
    _data->clearPlayerOptRec();
    _manager->openListenerForSelectEvent();
    _need_rob_count   = _player_count - 1;
    _cur_call_count   = 0;
    _cur_rob_count    = 0;
    _cur_bet_val      = 0;
    _cur_double_count = 0;
    _cur_opt = _opt_list.begin();
}

void SelectManager::callbackForCall(Event*) {
    // 刷新玩家选项数据
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = _data->getPlayerData(cur_id);
    cur_pdata->updateOptRecord(_xxf::en_call);
    log("%d号叫地主", cur_id);
    
    // ui效果
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_call, cur_id);
    if (this->getChildByName("panel"))
        this->removeChildByName("panel");
    
    // 下一步判断
    _first_call_id = cur_id;
    _manager->plusTargetId(1);
    _cur_opt++;
    startSelect();
}

void SelectManager::callbackForNocall(Event*) {
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = _data->getPlayerData(cur_id);
    cur_pdata->updateOptRecord(_xxf::en_nocall);
    log("%d号不叫地主", cur_id);
    
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_nocall, cur_id);
    if (this->getChildByName("panel"))
        this->removeChildByName("panel");
    
    _cur_call_count++;
    _manager->plusTargetId(1);
    if (_cur_call_count >= _player_count)
        _cur_bet_val == 0 ? endSelect() : callbackForConfirmLandlord(_last_bet_id);
    else
        startSelect();
}

void SelectManager::callbackForBetOne(Event*) {
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = _data->getPlayerData(cur_id);
    cur_pdata->updateOptRecord(_xxf::en_bet_one);
    log("%d号叫1分", cur_id);
    
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_bet_one, cur_id);
    if (this->getChildByName("panel"))
        this->removeChildByName("panel");
    
    _cur_call_count++;
    _last_bet_id = cur_id;
    _cur_bet_val = 1;
    _manager->plusTargetId(1);
    _cur_call_count >= _player_count ? callbackForConfirmLandlord(cur_id) : startSelect();
}

void SelectManager::callbackForBetTwo(Event*) {
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = _data->getPlayerData(cur_id);
    cur_pdata->updateOptRecord(_xxf::en_bet_two);
    log("%d号叫2分", cur_id);

    RoomAnimationManager::getInstance()->playPosted(_xxf::en_bet_two, cur_id);
    if (this->getChildByName("panel"))
        this->removeChildByName("panel");
    
    _cur_call_count++;
    _last_bet_id = cur_id;
    _cur_bet_val = 2;
    _manager->plusTargetId(1);
    _cur_call_count >= _player_count ? callbackForConfirmLandlord(cur_id) : startSelect();
}

void SelectManager::callbackForBetThr(Event*) {
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = _data->getPlayerData(cur_id);
    cur_pdata->updateOptRecord(_xxf::en_bet_thr);
    log("%d号叫3分", cur_id);
    
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_bet_thr, cur_id);
    if (this->getChildByName("panel"))
        this->removeChildByName("panel");
    
    // 叫3分直接成为地主
    callbackForConfirmLandlord(cur_id);
}

void SelectManager::callbackForRob(Event*) {
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = _data->getPlayerData(cur_id);
    cur_pdata->updateOptRecord(_xxf::en_rob);
    log("%d号抢地主", cur_id);
    
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_rob, cur_id);
    if (this->getChildByName("panel"))
        this->removeChildByName("panel");
    
    // 有玩家抢地主后，需要“叫地主”的玩家也做出“抢地主”判断，所以需要每个玩家都做出选择后才能确定地主
    if (_need_rob_count != _player_count)
        _need_rob_count = _player_count;
    _cur_rob_count++;
    _last_rob_id = cur_id;
    _manager->plusTargetId(1);
    int res = confirmLandlordId();
    res == -1 ? startSelect() : callbackForConfirmLandlord(res);
}

void SelectManager::callbackForNorob(Event*) {
    int cur_id = _manager->getTargetId();
    PlayerData* cur_pdata = _data->getPlayerData(cur_id);
    cur_pdata->updateOptRecord(_xxf::en_norob);
    log("%d号不抢地主", cur_id);
    
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_norob, cur_id);
    if (this->getChildByName("panel"))
        this->removeChildByName("panel");
    
    _cur_rob_count++;
    _manager->plusTargetId(1);
    int res = confirmLandlordId();
    res == -1 ? startSelect() : callbackForConfirmLandlord(res);
}

void SelectManager::callbackForDouble(Event* event) {
    EventCustom* custom_event = static_cast<EventCustom*>(event);
    PlayerData* pdata = static_cast<PlayerData*>(custom_event->getUserData());
    pdata->updateOptRecord(_xxf::en_double);
    log("%d号加倍", pdata->getSeatId());
    
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_double, pdata->getSeatId());
    
    _cur_double_count++;
    if (_cur_double_count >= _player_count) {
        _cur_opt++;
        endSelect();
    }
}

void SelectManager::callbackForNodouble(Event* event) {
    EventCustom* custom_event = static_cast<EventCustom*>(event);
    PlayerData* pdata = static_cast<PlayerData*>(custom_event->getUserData());
    pdata->updateOptRecord(_xxf::en_nodouble);
    log("%d号不加倍", pdata->getSeatId());
    
    RoomAnimationManager::getInstance()->playPosted(_xxf::en_nodouble, pdata->getSeatId());
    
    _cur_double_count++;
    if (_cur_double_count >= _player_count) {
        _cur_opt++;
        endSelect();
    }
}

bool SelectManager::init() {
    _data = RoomDataManager::getInstance();
    _player_count = _data->getPlayerNum();
    if (UserDefault::getInstance()->getIntegerForKey("Type") == 0) {
        //欢乐抢地主玩法
        _opt_list.push_front(OptionCode::DOUBLE);
        _opt_list.push_front(OptionCode::ROB);
        _opt_list.push_front(OptionCode::CALL);
    } else {
        // 叫分玩法
        _opt_list.push_front(OptionCode::DOUBLE);
        _opt_list.push_front(OptionCode::BET);
    }
    return true;
}

void SelectManager::endSelect() {
    if (_cur_opt != _opt_list.end()) {
        _manager->stopListenerForTouchCard();
        _manager->setStageCode(GameManager::StageCode::DEAL);
        SimpleToastManager::getInstance()->playToast("重新发牌");
        // debug
        if (_manager->_debug_layer)
            _manager->_debug_layer->updateCode("");
    } else {
        _manager->stopListenerForSelectEvent();
        _manager->setStageCode(GameManager::StageCode::PLAY);
        SimpleToastManager::getInstance()->playToast("进入出牌阶段");
    }
    CallFunc* ac = CallFunc::create([&]{
        RoomAnimationManager::getInstance()->clearAllPosted();
        _manager->run();
    });
    this->runAction(Sequence::create(DelayTime::create(1.0f), ac, nullptr));
}

void SelectManager::runForCodeCall() {
    PlayerData* pdata = _data->getPlayerData(_manager->getTargetId());
    UIOptPanel* panel = UIOptPanel::create(UIOptPanel::Type::CALL, pdata);
    this->addChild(panel, 0, "panel");
    
    if (pdata->getSeatId() != 0)
        ;  // TODO: need ai action
}

void SelectManager::runForCodeBet() {
    PlayerData* pdata = _data->getPlayerData(_manager->getTargetId());
    UIOptPanel* panel = UIOptPanel::create(UIOptPanel::Type::BET, pdata);
    this->addChild(panel, 0, "panel");
    
    if (pdata->getSeatId() == 0) {
        for (int i = 1; i <= _cur_bet_val; ++i)
            panel->setBtnEnabled(false, i);
    }
    
    if (pdata->getSeatId() != 0)
        ;  // TODO: need ai action
}

void SelectManager::runForCodeRob() {
    int cur_id = _manager->getTargetId();
    PlayerData* pdata = _data->getPlayerData(cur_id);
    if (pdata->getOptResult(_xxf::en_call) == 2) {
        log("选择不叫地主的玩家，默认跳过抢地主阶段——选择不抢");
        log("%d号不参与抢地主", cur_id);
        _cur_rob_count++;
        _manager->plusTargetId(1);
        int res = confirmLandlordId();
        res == -1 ? startSelect() : callbackForConfirmLandlord(res);
        return;
    }
    RoomAnimationManager::getInstance()->clearCurPosted(cur_id);
    UIOptPanel* panel = UIOptPanel::create(UIOptPanel::Type::ROB, pdata);
    this->addChild(panel, 0, "panel");
    
    if (pdata->getSeatId() != 0)
         ;  // TODO: need ai action
}

void SelectManager::runForCodeDouble() {
    RoomAnimationManager::getInstance()->clearAllPosted();
    
    for (auto& pair : _data->getPdataMap()) {
        PlayerData* pdata = pair.second;
        if (pdata->getPlayerConfig().gold < _data->getRoomConfig().double_limit)
            SimpleToastManager::getInstance()->playToast("金币不足，无法加倍");
        UIOptPanel* panel = UIOptPanel::create(UIOptPanel::Type::DOUBLE, pdata);
        this->addChild(panel);
        
        if (pdata->getSeatId() != 0)
             ;  // TODO: need ai action
    }
}

int SelectManager::confirmLandlordId() const {
    if (_cur_rob_count < _need_rob_count) {
        // 还有玩家未作出选择，不能确定地主
        return -1;
    } else if (_cur_rob_count == _player_count) {
        // 最后一个“抢地主”玩家成为地主
        return _last_rob_id;
    } else {
        // “叫地主”的玩家成为地主
        return _first_call_id;
    }
}

void SelectManager::callbackForConfirmLandlord(int landlord_id) {
    // debug
    if (_manager->_debug_layer)
        _manager->_debug_layer->updateCode("");
    
    PlayerData* pdata = _data->getPlayerData(landlord_id);
    pdata->setLandlord(true);
    std::string toast = StringUtils::format("这把的地主是%d号", landlord_id);
    SimpleToastManager::getInstance()->playToast(toast);
    _manager->_scene->addIdLogoForPlayer(landlord_id);
    _manager->getCardManager()->callbackForPushRestCard(landlord_id);
    CallFunc* call = CallFunc::create([this, landlord_id]{
        _cur_opt++;
        this->startSelect();
    });
    this->runAction(Sequence::create(DelayTime::create(1.0f), call, nullptr));
}

void SelectManager::debugJudge() {
    if (!_manager->_debug_layer)
        return;
    
    if (*_cur_opt == OptionCode::CALL)
        _manager->_debug_layer->updateCode(_xxf::en_call);
    else if (*_cur_opt == OptionCode::ROB)
        _manager->_debug_layer->updateCode(_xxf::en_rob);
    else if (*_cur_opt == OptionCode::BET)
        _manager->_debug_layer->updateCode("");
    else if (*_cur_opt == OptionCode::DOUBLE)
        _manager->_debug_layer->updateCode("");
}

