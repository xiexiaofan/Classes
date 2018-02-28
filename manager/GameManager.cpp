//
//  GameManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#include "GameManager.hpp"
#include "RoomScene.hpp"
#include "RoomData.hpp"
#include "PlayerData.hpp"
#include "RoomCardManager.hpp"
#include "DealCardManager.hpp"
#include "SelectManager.hpp"
#include "PlayCardManager.hpp"
#include "SettleManager.hpp"
#include "UISeat.hpp"
#include "UIOptButton.hpp"
#include "RoomDebugLayer.hpp"
#include "RoomAnimationManager.hpp"
#include "CommonDefineSet.hpp"
using namespace cocos2d;

// match config
static float Match_Time_Count = 0;
static int   Match_Timer_Arr[2] = {0};
static int   Match_Index[5] = {0, 1, 2, 3, 4};

GameManager::~GameManager() {
    clearListenerForSelectEvent();
    clearListenerForPlayCardEvent();
    RoomAnimationManager::getInstance()->destroyInstance();
}

void GameManager::run() {
    switch (_stage) {
        case StageCode::FREE:   runStageFree();   break;
        case StageCode::MATCH:  runStageMatch();  break;
        case StageCode::DEAL:   runStageDeal();   break;
        case StageCode::SELECT: runStageSelect(); break;
        case StageCode::PLAY:   runStagePlay();   break;
        case StageCode::SETTLE: runStageSettle(); break;
        default: break;
    }
}

int GameManager::plusTargetId(int delta, bool only_ret) {
    int ret = (_target_id + delta) % _data->getPlayerNum();
    if (!only_ret)
        _target_id = ret;
    return ret;
}

int GameManager::minusTargetId(int delta, bool only_ret) {
    int add_delta = _data->getPlayerNum() - delta % _data->getPlayerNum();
    return plusTargetId(add_delta, only_ret);
}

void GameManager::resetTargetId(int val) {
    _target_id = val == -1 ? RandomHelper::random_int(0, _data->getPlayerNum() - 1)
                           : val % _data->getPlayerNum();
}

void GameManager::openListenerForTouchCard() {
    if (_card_manager)
        _card_manager->openListenerForTouchCard();
}

void GameManager::stopListenerForTouchCard() {
    if (_card_manager)
        _card_manager->stopListenerForTouchCard();
}

void GameManager::openListenerForSelectEvent() {
    auto createListener = [&](const std::string& eventName,
                              const std::function<void(EventCustom*)>& callback){
        auto listener = EventListenerCustom::create(eventName, callback);
        _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
        _listener_select_vec.pushBack(listener);
    };
    if (_listener_select_vec.empty()) {
        createListener(_xxf::en_nocall,   CC_CALLBACK_1(SelectManager::callbackForNocall, _select_manager));
        createListener(_xxf::en_call,     CC_CALLBACK_1(SelectManager::callbackForCall, _select_manager));
        createListener(_xxf::en_norob,    CC_CALLBACK_1(SelectManager::callbackForNorob, _select_manager));
        createListener(_xxf::en_rob,      CC_CALLBACK_1(SelectManager::callbackForRob, _select_manager));
        createListener(_xxf::en_nodouble, CC_CALLBACK_1(SelectManager::callbackForNodouble, _select_manager));
        createListener(_xxf::en_double,   CC_CALLBACK_1(SelectManager::callbackForDouble, _select_manager));
        createListener(_xxf::en_bet_one,  CC_CALLBACK_1(SelectManager::callbackForBetOne, _select_manager));
        createListener(_xxf::en_bet_two,  CC_CALLBACK_1(SelectManager::callbackForBetTwo, _select_manager));
        createListener(_xxf::en_bet_thr,  CC_CALLBACK_1(SelectManager::callbackForBetThr, _select_manager));
    } else {
        for (auto& listener : _listener_select_vec)
            listener->setEnabled(true);
    }
}

void GameManager::stopListenerForSelectEvent() {
    for (auto& listener : _listener_select_vec)
        listener->setEnabled(false);
}

void GameManager::clearListenerForSelectEvent() {
    for (auto& listener : _listener_select_vec)
        _eventDispatcher->removeEventListener(listener);
}

void GameManager::openListenerForPlayCardEvent() {
    auto createListener = [&](const std::string& eventName,
                              const std::function<void(EventCustom*)>& callback){
        auto listener = EventListenerCustom::create(eventName, callback);
        _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
        _listener_play_vec.pushBack(listener);
    };
    if (_listener_play_vec.empty()) {
        createListener(_xxf::en_ming,  CC_CALLBACK_1(PlayCardManager::callbackForMing, _play_manager));
        createListener(_xxf::en_tip,   CC_CALLBACK_1(PlayCardManager::callbackForTip, _play_manager));
        createListener(_xxf::en_pass,  CC_CALLBACK_1(PlayCardManager::callbackForPass, _play_manager));
        createListener(_xxf::en_play,  CC_CALLBACK_1(PlayCardManager::callbackForPlay, _play_manager));
        createListener(_xxf::en_trust, CC_CALLBACK_1(PlayCardManager::callbackForTrust, _play_manager));
    } else {
        for (auto& listener : _listener_play_vec)
            listener->setEnabled(true);
    }
}

void GameManager::stopListenerForPlayCardEvent() {
    for (auto& listener : _listener_play_vec)
        listener->setEnabled(false);
}

void GameManager::clearListenerForPlayCardEvent() {
    for (auto& listener : _listener_play_vec)
        _eventDispatcher->removeEventListener(listener);
}

bool GameManager::init() {
    _data = RoomDataManager::getInstance();
    _card_manager = RoomCardManager::create();
    this->addChild(_card_manager);
    
    _debug = false;
    createDebugSwitch();
    
    return true;
}

void GameManager::runStageFree() {
    _scene->displayUIForGameNotBegin();
    UIOptButton* start_btn = UIOptButton::create("ks", "yellow", [&](UIOptButton*){
        RoomAnimationManager::getInstance()->playPosted(_xxf::en_ready, 0);
        this->setStageCode(GameManager::StageCode::MATCH);
        this->run();
    });
    start_btn->setNeedRemoveAfterClicked(true);
    start_btn->setPosition(569, 243);
    this->addChild(start_btn);
}

void GameManager::runStageMatch() {
    // 随机匹配时间
    Match_Timer_Arr[0] = RandomHelper::random_int(1, 1);
    Match_Timer_Arr[1] = RandomHelper::random_int(1, 1);
    // 随机匹配玩家
    int rand_1 = RandomHelper::random_int(0, 4);
    int rand_2 = RandomHelper::random_int(1, 4);
    std::swap(Match_Index[rand_1], Match_Index[0]);
    std::swap(Match_Index[rand_2], Match_Index[1]);
    Match_Time_Count = 0;
    RoomAnimationManager::getInstance()->updateMatchTimeDisplay(Match_Time_Count);
    this->schedule(CC_SCHEDULE_SELECTOR(GameManager::displayMatchInfo), 1.0f);
}

void GameManager::runStageDeal() {
    if (!_deal_manager) {
        _deal_manager = DealCardManager::create();
        _deal_manager->bindManager(this);
        _deal_manager->initDealConfig();
        this->addChild(_deal_manager);
    }
    _deal_manager->startDeal();
}

void GameManager::runStageSelect() {
    if (!_select_manager) {
        _select_manager = SelectManager::create();
        _select_manager->bindManager(this);
        this->addChild(_select_manager);
    }
    _select_manager->updateSelectConfig();
    _select_manager->startSelect();
}

void GameManager::runStagePlay() {
    if (!_play_manager) {
        _play_manager = PlayCardManager::create();
        _play_manager->bindManager(this);
        this->addChild(_play_manager);
        openListenerForPlayCardEvent();
    }
    _play_manager->updatePlayConfig();
    _play_manager->startPlay();
}

void GameManager::runStageSettle() {
    
}

void GameManager::displayMatchInfo(float dt) {
    Match_Time_Count += 1;
    RoomAnimationManager::getInstance()->updateMatchTimeDisplay(Match_Time_Count);
    if (Match_Time_Count == Match_Timer_Arr[0] + Match_Timer_Arr[1]) {
        std::string id = std::to_string(Match_Index[1]);
        log("p2 = %s", id.c_str());
        PlayerData* p2 = PlayerData::loadPlayerConfig(id);
        _data->addPlayerData(p2);
        _scene->loadPlayerData(p2);
        RoomAnimationManager::getInstance()->playPosted(_xxf::en_ready, p2->getSeatId());
        CallFunc* call = CallFunc::create([&]{
            RoomAnimationManager::getInstance()->clearAllPosted();
            RoomAnimationManager::getInstance()->clearMatchTimeDisplay();
            this->unschedule(SEL_SCHEDULE(&GameManager::displayMatchInfo));
            _scene->displayUIForGameBegin();
            this->setStageCode(StageCode::DEAL);
            this->run();
        });
        this->runAction(Sequence::create(DelayTime::create(1.0f), call, nullptr));
    } else if (Match_Time_Count == Match_Timer_Arr[0]) {
        std::string id = std::to_string(Match_Index[0]);
        log("p1 = %s", id.c_str());
        PlayerData* p1 = PlayerData::loadPlayerConfig(id);
        _data->addPlayerData(p1);
        _scene->loadPlayerData(p1);
        RoomAnimationManager::getInstance()->playPosted(_xxf::en_ready, p1->getSeatId());
    }
}

void GameManager::createDebugSwitch() {
    Node* node = Node::create();
    this->addChild(node);
    
    Sprite* bg_on = Sprite::create("extra/settings_btn_on.png");
    bg_on->setAnchorPoint(Vec2::ZERO);
    node->addChild(bg_on, 0, "ON");
    Sprite* bg_off = Sprite::create("extra/settings_btn_off.png");
    bg_off->setAnchorPoint(Vec2::ZERO);
    node->addChild(bg_off, 0, "OFF");
    node->setContentSize(bg_on->getContentSize());
    const Size& visib_size = Director::getInstance()->getVisibleSize();
    node->setPosition(Vec2(visib_size.width  - node->getContentSize().width + 20,
                           visib_size.height - node->getContentSize().height));
    
    Sprite* circle = Sprite::create("extra/settings_white_btn.png");
    circle->setPositionX(node->getContentSize().width * (_debug ? 0.8 : 0.15));
    circle->setPositionY(bg_on->getContentSize().height * 0.5);
    node->getChildByName("ON")->setVisible(_debug);
    node->getChildByName("OFF")->setVisible(!_debug);
    node->addChild(circle, 1, "POINT");
    node->setScale(0.8);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&, node](Touch* touch, Event*){
        Vec2 touch_in_node = node->convertTouchToNodeSpaceAR(touch);
        const auto& size = node->getContentSize();
        if (touch_in_node.x < 0 ||
            touch_in_node.x > size.width ||
            touch_in_node.y < 0 ||
            touch_in_node.y > size.height)
            return false;
        return true;
    };
    listener->onTouchEnded = [&, node](Touch* touch, Event*){
        Vec2 touch_in_node = node->convertTouchToNodeSpaceAR(touch);
        const auto& size = node->getContentSize();
        if (touch_in_node.x < 0 ||
            touch_in_node.x > size.width ||
            touch_in_node.y < 0 ||
            touch_in_node.y > size.height)
            return;
        // switch
        _debug = !_debug;
        node->getChildByName("ON")->setVisible(_debug);
        node->getChildByName("OFF")->setVisible(!_debug);
        float x = node->getContentSize().width * (_debug ? 0.8 : 0.15);
        node->getChildByName("POINT")->setPositionX(x);
        // callback
        callbackForDebugSwitch(_debug);
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, node);
}

void GameManager::callbackForDebugSwitch(bool open) {
    _card_manager->setDebugMing(open);

    if (open) {
        if (!_debug_layer) {
            _debug_layer = RoomDebugLayer::create();
            _debug_layer->bindManager(this);
            this->addChild(_debug_layer);
        } else {
            _debug_layer->setVisible(true);
        }
    } else {
        if (_debug_layer)
            _debug_layer->setVisible(false);
    }
}
