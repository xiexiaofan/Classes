//
//  LoadingScene.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#include "LoadingScene.hpp"
#include "RoomData.hpp"
#include "FirstScene.hpp"
using namespace cocos2d;

Scene* LoadingScene::createScene() {
    return LoadingScene::create();
}

LoadingScene::~LoadingScene() {
    RoomDataManager::destroyInstance();
}

bool LoadingScene::init() {
    if (!Scene::init())
        return false;
    // backgroud
    Sprite* bg = Sprite::create("image/loading_bg.jpg");
    this->addChild(bg);
    bg->setNormalizedPosition(Vec2::ANCHOR_MIDDLE);
    
    // loading texture
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/res_card.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/res_optpanel.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/res_roomentry.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/res_seatandinfobar.plist");
    
    // create data single instance
    RoomDataManager::getInstance();
    
    return true;
}

void LoadingScene::onEnter() {
    Scene::onEnter();
    Director::getInstance()->pushScene(FirstScene::createScene());
}
