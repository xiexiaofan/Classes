//
//  RoomScene.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#include "RoomScene.hpp"
#include "FirstScene.hpp"
#include "RoomData.hpp"
#include "PlayerData.hpp"
#include "GameManager.hpp"
#include "UITable.hpp"
#include "UISeat.hpp"
#include "UITopInfoBar.hpp"
#include "CommonDefineSet.hpp"
#include "CommonMethodSet.hpp"
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
using namespace cocos2d;

enum zorder {
    z_table,
    z_seat,
    z_btn,
    z_infobar,
    z_manager
};

/**
 * 牌桌背景索引
 */
static const std::map<std::string, int> table_ref {
    {"default", 0},
    {"pb",      1},
    {"cg",      2},
    {"sham",    3},
    {"ltx",     4},
    {"bc",      5}
};

/**
 * UISeat position 三人牌桌座位相对坐标值
 * 数组下标表示座位id
 */
static const Vec2 seat_pos_arr[3] {
    Vec2(25, 10),
    Vec2(989, 350),  // ref = 982
    Vec2(25, 350)
};

Scene* RoomScene::createScene() {
    return RoomScene::create();
}

void RoomScene::loadPlayerData(PlayerData* pdata) {
    if (!pdata)
        return;
    int seat_id = pdata->getSeatId();
    if (seat_id >= _seat_vec.size())
        return;
    _seat_vec.at(seat_id)->loadPlayerData(pdata);
}

void RoomScene::displayUIForGameNotBegin() {
    _table->displayForGameNotBegin();
    _top_bar->displayForGameNotBegin();
}

void RoomScene::displayUIForGameBegin() {
     _table->displayForGameBegin();
     _top_bar->displayForGameBegin();
}

void RoomScene::addIdLogoForPlayer(int landlord_id) {
    for(int i = 0; i < _seat_vec.size(); ++i) {
        UISeat* seat = _seat_vec.at(i);
        seat->displayerIdLogo(i == landlord_id, i != 1);
    }
}

void RoomScene::rmIdlogoForPlayer() {
    for (auto& seat : _seat_vec)
        seat->rmplayerIdLogo();
}

bool RoomScene::init() {
    if (!Scene::init())
        return false;
    Size visible_size = Director::getInstance()->getVisibleSize();
    //
    _manager = GameManager::create();
    _manager->bindScene(this);
    this->addChild(_manager, z_manager);
    
    // 返回按钮
    Sprite* exit = Sprite::create("icon_btn_fanhui.png");
    exit->setPosition(exit->getContentSize().width * 0.5 + 4,
                      visible_size.height - exit->getContentSize().height * 0.5 - 4);
    this->addChild(exit, z_btn, "Str_Exit");
    addListenerForExitBtn();
    
    // 牌桌
    int table_index = table_ref.at(RoomDataManager::getInstance()->getRoomConfig().image);
    _table = UITable::create(static_cast<UITable::Type>(table_index));
    this->addChild(_table, z_table);
    _table->setNormalizedPosition(Vec2(0.5, 0.5));
    
    // 座位
    for (int i = 0; i < 3; ++i) {
        UISeat* seat = UISeat::create();
        seat->setPosition(seat_pos_arr[i]);
        seat->setScale(0.9);
        _seat_vec.push_back(seat);
        this->addChild(seat, z_seat);
    }
    PlayerData* my_hero = PlayerData::getMyHero();
    RoomDataManager::getInstance()->addPlayerData(my_hero);
    loadPlayerData(my_hero);
    
    // 顶部信息栏
    _top_bar = UITopInfoBar::create();
    _top_bar->setPosition(visible_size.width * 0.5, visible_size.height - 26);
    this->addChild(_top_bar, z_infobar);
    
    // test
    displayUIForGameNotBegin();
    
    return true;
}

void RoomScene::onEnter() {
    Scene::onEnter();
    _manager->run();
}

void RoomScene::onExit() {
    Scene::onExit();
}

void RoomScene::addListenerForExitBtn() {
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event*)->bool{
        if (_xxf::isTouchPointAtValidScope(touch, this->getChildByName("Str_Exit"), 1.2)) {
            this->getChildByName("Str_Exit")->runAction(ScaleTo::create(0.1f, 0.9));
            SimpleAudioEngine::getInstance()->playEffect("sound/effect/button.mp3");
            return true;
        }
        return false;
    };
    listener->onTouchEnded = [&](Touch* touch, Event*){
        this->getChildByName("Str_Exit")->runAction(ScaleTo::create(0.1f, 1.0));
        if (_xxf::isTouchPointAtValidScope(touch, this->getChildByName("Str_Exit"), 1.3))
            Director::getInstance()->replaceScene(FirstScene::createEntryScene());
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}
