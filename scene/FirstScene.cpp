//
//  FirstScene.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#include "FirstScene.hpp"
#include "RoomEntryLayer.hpp"
#include "SpriteBlur.hpp"
#include "DebugLayer.hpp"
#include "SimpleToastManager.hpp"
#include "audio/include/SimpleAudioEngine.h"
#include "cocostudio/CocoStudio.h"
using namespace cocos2d;
using namespace cocos2d::ui;
using namespace CocosDenshion;

enum zorder {
    z_bg,
    z_btn,
    z_bg_blur,
    z_other
};

Scene* FirstScene::createScene() {
    return FirstScene::create();
}

Scene* FirstScene::createEntryScene() {
    FirstScene* ret = FirstScene::create();
    // TODD
    ret->initEntryLayer();
    return ret;
}

bool FirstScene::init(){
    if (!Scene::init())
        return false;
    initBgLayer();
    initBtnLayer();
    return true;
}

void FirstScene::onEnter() {
    Scene::onEnter();
}

void FirstScene::onExit() {
    Scene::onExit();
}

void FirstScene::resumeFirstScene() {
    auto bg_blur = this->getChildByName("bg_blur");
    if (bg_blur)
        bg_blur->removeFromParent();
    for (auto child : _bg_layer->getChildren())
        child->resume();
    _btn_layer->setVisible(true);
}

void FirstScene::initBgLayer() {
    _bg_layer = Layer::create();
    this->addChild(_bg_layer, zorder::z_bg);
    
    Sprite* sky = Sprite::create("image/home/home_bg_2.jpg");
    sky->setNormalizedPosition(Vec2::ANCHOR_MIDDLE);
    _bg_layer->addChild(sky);
    Sprite* land = Sprite::create("image/home/home_bg_dq.png");
    land->setPosition(420, -400);
    _bg_layer->addChild(land);
    
    auto getAnima = [&](float delay, unsigned int loops){
        Animation* ret = Animation::create();
        ret->setDelayPerUnit(delay);
        ret->setLoops(loops);
        return ret;
    };
    auto dz_anim = getAnima(0.25f, -1);
    dz_anim->addSpriteFrameWithFile("image/home/home_dz_2.png");
    dz_anim->addSpriteFrameWithFile("image/home/home_dz_3.png");
    dz_anim->addSpriteFrameWithFile("image/home/home_dz_2.png");
    dz_anim->addSpriteFrameWithFile("image/home/home_dz_1.png");
    Sprite* landlord = Sprite::create();
    landlord->setAnchorPoint(Vec2::ZERO);
    landlord->setPosition(188, 50);
    _bg_layer->addChild(landlord);
    landlord->runAction(Animate::create(dz_anim));
    
    auto nm_anim = getAnima(0.15f, -1);
    nm_anim->addSpriteFrameWithFile("image/home/home_fj_1.png");
    nm_anim->addSpriteFrameWithFile("image/home/home_fj_2.png");
    MoveBy* up = MoveBy::create(0.5f, Vec2(2.5, 5));
    MoveBy* down = up->reverse();
    Sequence* flow = Sequence::create(up, down, nullptr);
    Sprite* farmer = Sprite::create();
    farmer->setAnchorPoint(Vec2::ZERO);
    farmer->setPosition(100, 415);
    _bg_layer->addChild(farmer);
    farmer->runAction(Animate::create(nm_anim));
    farmer->runAction(RepeatForever::create(flow));
    
    land->runAction(RepeatForever::create(RotateBy::create(1.0f, -2.5)));
}

void FirstScene::initBtnLayer() {
    _btn_layer = Layer::create();
    this->addChild(_btn_layer, zorder::z_btn);
    
    Size visible_size = Director::getInstance()->getVisibleSize();
    // btn: exit
    MenuItemImage* close_btn = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                                     CC_CALLBACK_1(FirstScene::callbackForExit, this));
    close_btn->setPosition(close_btn->getContentSize().width * 0.5 + 10,
                           visible_size.height - close_btn->getContentSize().height * 0.5 - 10);
    Menu* menu = Menu::create(close_btn, nullptr);
    menu->setPosition(0, 0);
    _btn_layer->addChild(menu);
    // btn: start + review + control
    auto getTextBtn = [&](const std::string& name,
                          const std::function<void(Ref*,Widget::TouchEventType)>& callback){
        Button* btn = Button::create();
        btn->setTitleText(name);
        btn->setTitleFontSize(35);
        btn->getTitleRenderer()->enableShadow();
        btn->setPressedActionEnabled(true);
        btn->setZoomScale(0.25);
        btn->addTouchEventListener(callback);
        return btn;
    };
    Button* start = getTextBtn("开 始", CC_CALLBACK_2(FirstScene::callbackForStart, this));
    start->setPosition(Vec2(visible_size.width * 0.9, start->getContentSize().height * 4 + 20));
    _btn_layer->addChild(start);
    Button* review = getTextBtn("档 案", CC_CALLBACK_2(FirstScene::callbackForReview, this));
    review->setPosition(Vec2(visible_size.width * 0.9, start->getContentSize().height * 2.5 + 20));
    _btn_layer->addChild(review);
    Button* setting = getTextBtn("设 置", CC_CALLBACK_2(FirstScene::callbackForControl, this));
    setting->setPosition(Vec2(visible_size.width * 0.9, start->getContentSize().height * 1 + 20));
    _btn_layer->addChild(setting);
}

void FirstScene::initEntryLayer() {
    // pause
    _btn_layer->setVisible(false);
    for (auto child : _bg_layer->getChildren()) { child->pause(); }
    // screen bg_layer
    RenderTexture* screen = RenderTexture::create(Director::getInstance()->getVisibleSize().width,
                                                  Director::getInstance()->getVisibleSize().height);
    screen->begin();
    _bg_layer->visit();
    screen->end();
    SpriteFrame* texture = screen->getSprite()->getSpriteFrame();
    SpriteBlur* blur = SpriteBlur::create(texture);
    blur->setBlurSampleNum(11);
    blur->setBlurRadius(11);
    blur->setRotationSkewX(180);
    blur->setPosition(Director::getInstance()->getVisibleSize() * 0.5);
    // screen again as entry_layer backgound
    RenderTexture* ret = RenderTexture::create(Director::getInstance()->getVisibleSize().width,
                                               Director::getInstance()->getVisibleSize().height);
    ret->begin();
    blur->visit();
    ret->end();
    Sprite* entry_bg = Sprite::createWithTexture(ret->getSprite()->getTexture());
    entry_bg->setRotationSkewX(180);
    entry_bg->setNormalizedPosition(Vec2::ANCHOR_MIDDLE);
    this->addChild(entry_bg, z_bg_blur, "bg_blur");
    
    // RoomEntry
    if (!_entry_layer) {
        _entry_layer = RoomEntryLayer::create(this);
        this->addChild(_entry_layer, zorder::z_other);
    } else {
        _entry_layer->onEnter();
    }
}

void FirstScene::callbackForExit(Ref*) {
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void FirstScene::callbackForStart(Ref*, Widget::TouchEventType type) {
    if (type == Widget::TouchEventType::ENDED)
        initEntryLayer();
}

void FirstScene::callbackForReview(Ref*, Widget::TouchEventType type) {
    if (type == Widget::TouchEventType::ENDED) {
        if (!this->getChildByName("debug")) {
            DebugLayer* layer = DebugLayer::create();
            this->addChild(layer, 10, "debug");
        } else {
            this->getChildByName("debug")->removeFromParent();
        }
    }
   //     SimpleToastManager::getInstance()->playToast("档案功能还待研究...");
}

void FirstScene::callbackForControl(Ref*, Widget::TouchEventType type) {
    if (type == Widget::TouchEventType::ENDED) {
        SimpleToastManager::getInstance()->playToast("设置功能还待研究...");
        
        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto rootNode = CSLoader::createNode("animation/settle_win/settle_win_wylnnm.csd");
        addChild(rootNode);
        auto ac = CSLoader::createTimeline("animation/settle_win/settle_win_wylnnm.csd");
        ac->setLastFrameCallFunc([rootNode]{
            rootNode->stopAllActions();
            rootNode->removeFromParent();
        });
        ac->gotoFrameAndPlay(0, false);
        rootNode->runAction(ac);
    }
}
