//
//  RoomEntryLayer.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#include "RoomEntryLayer.hpp"
#include "FirstScene.hpp"
#include "RoomScene.hpp"
#include "RoomData.hpp"
#include "CommonMethodSet.hpp"
#include "SimpleToastManager.hpp"
#include "ui/UIScale9Sprite.h"
using namespace cocos2d;
using ui::Scale9Sprite;

// local zorder for this scene
enum zorder {
    z_background,
    z_btn,
    z_room,
    z_toast
};

static const int   RoomCount = 6;
static const float RoomLXDis = 100;
static const float RoomGap   = -20;
static const Size& RoomSize  = Size(280, 410);
static const std::string Str_Normal   = "normal";
static const std::string Str_Selected = "Selected";
static const std::string Str_Exit     = "exit";

/**
 * @class RoomEntryLayer
 *
 */
RoomEntryLayer* RoomEntryLayer::create(Scene* scene) {
    RoomEntryLayer* ret = new RoomEntryLayer();
    if (ret && ret->initWithScene(scene))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

RoomEntryLayer::~RoomEntryLayer() {
    auto iter = _listener_vec.begin();
    while (iter != _listener_vec.end()) {
        auto l = *iter;
        _eventDispatcher->removeEventListener(l);
        iter = _listener_vec.erase(iter);
    }
}

bool RoomEntryLayer::initWithScene(Scene* scene) {
    if (!Layer::init())
        return false;
    _scene = scene;
    Size visible_size = Director::getInstance()->getVisibleSize();
    // 初始化 背景图片
//    Sprite* bg_image = Sprite::create("image/home_bg.jpg");
//    bg_image->setNormalizedPosition(Vec2(0.5, 0.5));
//    this->addChild(bg_image, z_background);
    
    Sprite* bg_title_image = Sprite::createWithSpriteFrameName("room_ui_bg_01.png");
    bg_title_image->setAnchorPoint(Vec2(0, 1));
    bg_title_image->setPositionY(visible_size.height);
    this->addChild(bg_title_image, z_background);
    
//    Scale9Sprite* bg_bottom_image = Scale9Sprite::createWithSpriteFrameName("room_ui_bg_02.png");
//    bg_bottom_image->setCapInsets(Rect(2, 55, 488, 100));
//    bg_bottom_image->setContentSize(Size(488, 155));
//    bg_bottom_image->setAnchorPoint(Vec2(1, 0));
//    bg_bottom_image->setPosition(visible_size.width, 0);
//    this->addChild(bg_bottom_image, z_background);
    
    Sprite* bg_title_font = Sprite::createWithSpriteFrameName("room_fonts_xxms.png");
    bg_title_font->setAnchorPoint(Vec2(0, 1));
    bg_title_font->setPosition(112, visible_size.height - 12);
    this->addChild(bg_title_font, z_background);
    // 初始化 退出按钮
    Sprite* exit = Sprite::create("icon_btn_fanhui.png");
    exit->setPosition(exit->getContentSize().width * 0.5 + 4,
                      visible_size.height - exit->getContentSize().height * 0.5 - 4);
    this->addChild(exit, z_btn, Str_Exit);
    addlistenerForExitBtn();
    // 初始化 癞子玩法按钮
    Sprite* sp_nor = Sprite::createWithSpriteFrameName("btn_ui_1.png");
    Sprite* sp_sel = Sprite::createWithSpriteFrameName("btn_ui_2.png");
//    Sprite* fake_font = Sprite::createWithSpriteFrameName("room_icon_lzc.png");
    Sprite* fake_font = Sprite::createWithSpriteFrameName("room_icon_jdjf.png");
    _fake_btn = Node::create();
    _fake_btn->addChild(sp_nor, 0, Str_Normal);
    _fake_btn->addChild(sp_sel, 0, Str_Selected);
    _fake_btn->addChild(fake_font, 1);
    _fake_btn->setContentSize(sp_nor->getContentSize());
    _fake_btn->setPosition(_fake_btn->getContentSize().width * 1.5 + 40, _fake_btn->getContentSize().height * 0.5);
    this->addChild(_fake_btn, z_btn);
    // 初始化 经典玩法按钮
    Sprite* sp_nor_2 = Sprite::createWithSpriteFrame(sp_nor->getSpriteFrame());
    sp_nor_2->setFlippedX(true);
    Sprite* sp_sel_2 = Sprite::createWithSpriteFrame(sp_sel->getSpriteFrame());
    sp_sel_2->setFlippedX(true);
//    Sprite* classic_font = Sprite::createWithSpriteFrameName("room_icon_jdc.png");
    Sprite* classic_font = Sprite::createWithSpriteFrameName("room_icon_hlqdz.png");
    _classic_btn = Node::create();
    _classic_btn->addChild(sp_nor_2, 0, Str_Normal);
    _classic_btn->addChild(sp_sel_2, 0, Str_Selected);
    _classic_btn->addChild(classic_font, 1);
    _classic_btn->setContentSize(sp_nor_2->getContentSize());
    _classic_btn->setPosition(_classic_btn->getContentSize().width * 0.5 + 40,
                              _classic_btn->getContentSize().height * 0.5);
    this->addChild(_classic_btn, z_btn);
    // 初始化 不洗牌玩法按钮
    Sprite* nodeal_nor = Sprite::createWithSpriteFrameName("room_tab_green.png");
    Sprite* nodeal_sel = Sprite::createWithSpriteFrameName("room_tab_yellow.png");
    Sprite* nodeal_nor_fonts = Sprite::createWithSpriteFrameName("room_tab_green_bxp.png");
    Sprite* nodeal_sel_fonts = Sprite::createWithSpriteFrameName("room_tab_yellow_bxp.png");
    nodeal_nor_fonts->setPosition(nodeal_nor->getContentSize() * 0.5);
    nodeal_nor->addChild(nodeal_nor_fonts);
    nodeal_sel_fonts->setPosition(nodeal_sel->getContentSize() * 0.5);
    nodeal_sel->addChild(nodeal_sel_fonts);
    _nodeal_btn = Node::create();
    _nodeal_btn->addChild(nodeal_nor, 0, Str_Normal);
    _nodeal_btn->addChild(nodeal_sel, 0, Str_Selected);
    _nodeal_btn->setContentSize(nodeal_nor->getContentSize());
    _nodeal_btn->setPosition(_nodeal_btn->getContentSize().width * 0.5 + 2,
                             _nodeal_btn->getContentSize().height * 1.5 + 144);
    this->addChild(_nodeal_btn, z_btn);
    // 初始化 洗牌玩法按钮
    Sprite* deal_nor = Sprite::createWithSpriteFrameName("room_tab_green.png");
    Sprite* deal_sel = Sprite::createWithSpriteFrameName("room_tab_yellow.png");
    Sprite* deal_nor_fonts = Sprite::createWithSpriteFrameName("room_tab_green_xp.png");
    Sprite* deal_sel_fonts = Sprite::createWithSpriteFrameName("room_tab_yellow_xp.png");
    deal_nor_fonts->setPosition(deal_nor->getContentSize() * 0.5);
    deal_nor->addChild(deal_nor_fonts);
    deal_sel_fonts->setPosition(deal_sel->getContentSize() * 0.5);
    deal_sel->addChild(deal_sel_fonts);
    _deal_btn = Node::create();
    _deal_btn->addChild(deal_nor, 0, Str_Normal);
    _deal_btn->addChild(deal_sel, 0, Str_Selected);
    _deal_btn->setContentSize(deal_nor->getContentSize());
    _deal_btn->setPosition(_nodeal_btn->getContentSize().width * 0.5 + 2,
                           _nodeal_btn->getContentSize().height * 0.5 + 144);
    this->addChild(_deal_btn, z_btn);
    // 初始化 快速开始按钮
    _qstart_btn = Sprite::createWithSpriteFrameName("home_btn_ksks.png");
    _qstart_btn->setScale(0.92);
    Sprite* qstart_font = Sprite::createWithSpriteFrameName("home_btn_ksks_fonts.png");
    qstart_font->setPosition(_qstart_btn->getContentSize().width * 0.5, _qstart_btn->getContentSize().height * 0.53);
    _qstart_btn->addChild(qstart_font);
    _qstart_btn->setPosition(_qstart_btn->getContentSize().width * 0.5 + 874,
                             _qstart_btn->getContentSize().height * 0.5 - 4);
    this->addChild(_qstart_btn, z_btn);
    // 初始化 房间
    Size container_size = Size((RoomSize.width + RoomGap) * RoomCount, RoomSize.height);
    _stencil = DrawNode::create();
    _stencil->drawSolidRect(Vec2(RoomGap, RoomGap), Vec2(container_size.width, container_size.height), Color4F::BLACK);
    _room_layer = ClippingNode::create(_stencil);
    _room_layer->setContentSize(container_size);
    this->addChild(_room_layer, z_room);
    _room_container = Node::create();
    _room_container->setContentSize(container_size);
    _room_layer->addChild(_room_container);
    _room_layer->setPosition(RoomLXDis, 140);
    for (int i = 1; i <= RoomCount; ++i) {
        SingleRoomCreator* room = SingleRoomCreator::create(this, i);
        room->setAnchorPoint(Vec2::ZERO);
        room->setVisible(false);
        room->setContentSize(RoomSize);
        _room_container->addChild(room);
        _room_vec.pushBack(room);
    }
    // 注册 触摸事件监听器
    addListenerForClassicBtn();
    addListenerForFakeBtn();
    addListenerForNodealBtn();
    addListenerForDealBtn();
    addListenerForQStartBtn();
    addListenerForScrollRoom();
    
    return true;
}

void RoomEntryLayer::onEnter() {
    this->setVisible(true);
    
    // get type & mode by UserDefault
    if (UserDefault::getInstance()->getIntegerForKey("Type", 0) == 0) {
        _type = Type::CLASSIC;
        lightenTypeBtn(_type);
        extinctTypeBtn(Type::FAKE);
    } else {
        _type = Type::FAKE;
        lightenTypeBtn(_type);
        extinctTypeBtn(Type::CLASSIC);
    }
    if (UserDefault::getInstance()->getIntegerForKey("Mode", 0) == 0) {
        _mode = Mode::NODEAL;
        lightenModeBtn(_mode);
        extinctModeBtn(Mode::DEAL);
    } else {
        _mode = Mode::DEAL;
        lightenModeBtn(_mode);
        extinctModeBtn(Mode::NODEAL);
    }
    
    // 恢复触摸监听
    auto iter = _listener_vec.begin();
    while (iter != _listener_vec.end()) {
        auto l = *iter;
        l->setEnabled(true);
        ++iter;
    }
    
    this->runAction(CallFunc::create([this]{this->runAnimationForScorllRoom(1);}));
    
    Layer::onEnter();
}

void RoomEntryLayer::onExit() {
    this->setVisible(false);
    
    // stop fixed listener about this scene
    auto iter = _listener_vec.begin();
    while (iter != _listener_vec.end()) {
        auto l = *iter;
        l->setEnabled(false);
        ++iter;
    }
    
    Layer::onExit();
}

void RoomEntryLayer::addlistenerForExitBtn() {
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event*)->bool{
        if (_xxf::isTouchPointAtValidScope(touch, this->getChildByName(Str_Exit), 1.2)) {
            this->getChildByName(Str_Exit)->runAction(ScaleTo::create(0.1f, 0.9));
            return true;
        }
        return false;
    };
    listener->onTouchEnded = [&](Touch* touch, Event*){
        this->getChildByName(Str_Exit)->runAction(ScaleTo::create(0.1f, 1.0));
        if (_xxf::isTouchPointAtValidScope(touch, this->getChildByName(Str_Exit), 1.3)) {
            static_cast<FirstScene*>(_scene)->resumeFirstScene();
            this->onExit();
        }
    };
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    _listener_vec.push_back(listener);
}

void RoomEntryLayer::addListenerForClassicBtn() {
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event*)->bool{
        if (_xxf::isTouchPointAtValidScope(touch, _classic_btn) && _type != Type::CLASSIC) {
            this->lightenTypeBtn(Type::CLASSIC);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [&](Touch* touch, Event*){
        _xxf::isTouchPointAtValidScope(touch, _classic_btn) ? this->lightenTypeBtn(Type::CLASSIC)
        : this->extinctTypeBtn(Type::CLASSIC);
    };
    listener->onTouchEnded = [&](Touch* touch, Event*){
        if (_xxf::isTouchPointAtValidScope(touch, _classic_btn)) {
            UserDefault::getInstance()->setIntegerForKey("Type", 0);
            this->setType(Type::CLASSIC);
            this->extinctTypeBtn(Type::FAKE);
            this->runAnimationForScorllRoom(1);
        }
    };
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    _listener_vec.push_back(listener);
}

void RoomEntryLayer::addListenerForFakeBtn() {
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event*)->bool{
        if (_xxf::isTouchPointAtValidScope(touch, _fake_btn) && _type != Type::FAKE) {
            this->lightenTypeBtn(Type::FAKE);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [&](Touch* touch, Event*){
        _xxf::isTouchPointAtValidScope(touch, _fake_btn) ? this->lightenTypeBtn(Type::FAKE)
        : this->extinctTypeBtn(Type::FAKE);
    };
    listener->onTouchEnded = [&](Touch* touch, Event*){
        if (_xxf::isTouchPointAtValidScope(touch, _fake_btn)) {
            UserDefault::getInstance()->setIntegerForKey("Type", 1);
            this->setType(Type::FAKE);
            this->extinctTypeBtn(Type::CLASSIC);
            this->runAnimationForScorllRoom(1);
        }
    };
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    _listener_vec.push_back(listener);
}

void RoomEntryLayer::addListenerForNodealBtn() {
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event*)->bool{
        if (_xxf::isTouchPointAtValidScope(touch, _nodeal_btn) && _mode != Mode::NODEAL) {
            this->lightenModeBtn(Mode::NODEAL);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [&](Touch* touch, Event*){
        _xxf::isTouchPointAtValidScope(touch, _nodeal_btn) ? this->lightenModeBtn(Mode::NODEAL)
        : this->extinctModeBtn(Mode::NODEAL);
    };
    listener->onTouchEnded = [&](Touch* touch, Event*){
        if (_xxf::isTouchPointAtValidScope(touch, _nodeal_btn)) {
            UserDefault::getInstance()->setIntegerForKey("Mode", 0);
            this->setMode(Mode::NODEAL);
            this->extinctModeBtn(Mode::DEAL);
            this->runAnimationForScorllRoom(1);
        }
    };
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    _listener_vec.push_back(listener);
}

void RoomEntryLayer::addListenerForDealBtn() {
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event*)->bool{
        if (_xxf::isTouchPointAtValidScope(touch, _deal_btn) && _mode != Mode::DEAL) {
            this->lightenModeBtn(Mode::DEAL);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [&](Touch* touch, Event*){
        _xxf::isTouchPointAtValidScope(touch, _deal_btn) ? this->lightenModeBtn(Mode::DEAL)
        : this->extinctModeBtn(Mode::DEAL);
    };
    listener->onTouchEnded = [&](Touch* touch, Event*){
        if (_xxf::isTouchPointAtValidScope(touch, _deal_btn)) {
            UserDefault::getInstance()->setIntegerForKey("Mode", 1);
            this->setMode(Mode::DEAL);
            this->extinctModeBtn(Mode::NODEAL);
            this->runAnimationForScorllRoom(1);
        }
    };
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    _listener_vec.push_back(listener);
}

void RoomEntryLayer::addListenerForQStartBtn() {
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event*)->bool{
        if (_xxf::isTouchPointAtValidScope(touch, _qstart_btn)) {
            _qstart_btn->runAction(ScaleTo::create(0.1f, 0.8));  // 0.83 = 0.92 * 0.9
            return true;
        }
        return false;
    };
    listener->onTouchEnded = [&](Touch* touch, Event*){
        _qstart_btn->runAction(ScaleTo::create(0.1f, 0.92));
        if (_xxf::isTouchPointAtValidScope(touch, _qstart_btn, 1.1)) {
            int real_id = 110 + RandomHelper::random_int(1, 6) + (_type == RoomEntryLayer::Type::FAKE ? 10 : 0);
            log("enter room id = %d", real_id);
            std::string room_id = std::to_string(real_id);
            RoomDataManager::getInstance()->initWithRoomId(room_id);
            CallFunc* call = CallFunc::create([&]{Director::getInstance()->replaceScene(RoomScene::createScene());});
            this->runAction(Sequence::create(DelayTime::create(0.1f), call, nullptr));
        }
    };
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    _listener_vec.push_back(listener);
}

void RoomEntryLayer::addListenerForScrollRoom() {
    EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* touch, Event*)->bool{
        int index = this->getRoomIdForTouchPoint(touch);
        if (index != -1 && !_is_scrolling) {
            _selected_room_id = index;
            _room_vec.at(_selected_room_id)->runAnimationForPressed(0.9);
            return true;
        }
        return false;
    };
    listener->onTouchMoved = [&](Touch* touch, Event*){
        if (!_is_scrolling && std::fabsf(touch->getDelta().x) > 2) {
            _is_scrolling = true;
            _room_vec.at(_selected_room_id)->runAnimationForPressed(1.0);
        }
        if (_is_scrolling) {
            float dest_x = _room_container->getPositionX() + touch->getDelta().x;
            float max_dest_x = 0;
            float min_dest_x = Director::getInstance()->getVisibleSize().width -
            _room_container->getContentSize().width - RoomLXDis;
            dest_x = std::min(dest_x, max_dest_x);
            dest_x = std::max(dest_x, min_dest_x);
            _room_container->setPositionX(dest_x);
        }
    };
    listener->onTouchEnded = [&](Touch* touch, Event*){
        _room_vec.at(_selected_room_id)->runAnimationForPressed(1.0);
        if (!_is_scrolling) {
            CallFunc* call = CallFunc::create([&]{_room_vec.at(_selected_room_id)->callbackForEnterRoom();});
            this->runAction(Sequence::create(DelayTime::create(0.1), call, nullptr));
        } else {
            _is_scrolling = false;
        }
    };
    _eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
    _listener_vec.push_back(listener);
}

inline void RoomEntryLayer::lightenTypeBtn(RoomEntryLayer::Type type) {
    if (type == Type::CLASSIC) {
        _classic_btn->getChildByName(Str_Selected)->setVisible(true);
        _classic_btn->getChildByName(Str_Normal)->setVisible(false);
    } else {
        _fake_btn->getChildByName(Str_Selected)->setVisible(true);
        _fake_btn->getChildByName(Str_Normal)->setVisible(false);
    }
}

inline void RoomEntryLayer::extinctTypeBtn(RoomEntryLayer::Type type) {
    if (type == Type::CLASSIC) {
        _classic_btn->getChildByName(Str_Normal)->setVisible(true);
        _classic_btn->getChildByName(Str_Selected)->setVisible(false);
    } else {
        _fake_btn->getChildByName(Str_Normal)->setVisible(true);
        _fake_btn->getChildByName(Str_Selected)->setVisible(false);
    }
}

inline void RoomEntryLayer::lightenModeBtn(RoomEntryLayer::Mode mode) {
    if (mode == Mode::DEAL) {
        _deal_btn->getChildByName(Str_Selected)->setVisible(true);
        _deal_btn->getChildByName(Str_Normal)->setVisible(false);
    } else {
        _nodeal_btn->getChildByName(Str_Selected)->setVisible(true);
        _nodeal_btn->getChildByName(Str_Normal)->setVisible(false);
    }
}

inline void RoomEntryLayer::extinctModeBtn(RoomEntryLayer::Mode mode) {
    if (mode == Mode::DEAL) {
        _deal_btn->getChildByName(Str_Normal)->setVisible(true);
        _deal_btn->getChildByName(Str_Selected)->setVisible(false);
    } else {
        _nodeal_btn->getChildByName(Str_Normal)->setVisible(true);
        _nodeal_btn->getChildByName(Str_Selected)->setVisible(false);
    }
}

void RoomEntryLayer::runAnimationForScorllRoom(int dir) {
    setScorlling(true);
    std::for_each(_room_vec.begin(), _room_vec.end(),
                  [dir](SingleRoomCreator* room){room->runAnimationForScrolling(dir);});
}

int RoomEntryLayer::getRoomIdForTouchPoint(cocos2d::Touch* touch) const {
    int ret = -1;
    Vec2 touch_in_scene = _room_layer->convertTouchToNodeSpace(touch);
    if (touch_in_scene.x < RoomGap + 5.0)
        return ret;
    Vec2 touch_in_node = _room_container->convertTouchToNodeSpace(touch);
    if ( touch_in_node.y < 0 || touch_in_node.y > _room_container->getContentSize().height)
        return ret;
    ret = touch_in_node.x / (RoomSize.width + RoomGap);
    ret = std::min(ret, RoomCount - 1);
    return ret;
}

/**
 * @class SingleRoomCreator
 *
 */
SingleRoomCreator* SingleRoomCreator::create(RoomEntryLayer* scene, int id) {
    SingleRoomCreator* ret = new SingleRoomCreator();
    if (ret && ret->init(scene, id))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

void SingleRoomCreator::updateModeLogo() {
    if (_scene->getMode() == RoomEntryLayer::Mode::DEAL) {
        _mode_logo->setVisible(false);
    } else if (_scene->getMode() == RoomEntryLayer::Mode::NODEAL) {
        _mode_logo->setVisible(true);
    }
}

void SingleRoomCreator::runAnimationForScrolling(int dir) {
    updateRoomDataDisplay();
    updateModeLogo();
    this->stopAllActions();
    CallFunc* call = CallFunc::create([&]{
        setVisible(false);
        this->setPosition((RoomSize.width + RoomGap) * (dir ? _room_id: _room_id - 2), 0);
    });
    DelayTime* delay = DelayTime::create((_room_id - 1) * 0.03);
    MoveBy* mb = MoveBy::create(0.05, Vec2(RoomSize.width * (dir ? - 0.5 : 0.5), 0));
    CallFunc* call_2 = CallFunc::create([&]{ setVisible(true); });
    MoveBy* mb_2 = MoveBy::create(0.2, Vec2(RoomSize.width * (dir ? - 0.5 : 0.5), 0));
    CallFunc* call_3 = CallFunc::create([&]{ if (_room_id == RoomCount) _scene->setScorlling(false); });
    Sequence* sq = Sequence::create(call, delay, mb, call_2, mb_2, call_3, nullptr);
    this->runAction(sq);
}

void SingleRoomCreator::runAnimationForPressed(float zoom) {
    for (Node* node : this->getChildren())
        node->runAction(ScaleTo::create(0.1f, zoom));
}

void SingleRoomCreator::callbackForEnterRoom() {
    int real_id = 110 + _room_id + ((_scene->_type == RoomEntryLayer::Type::FAKE) ? 10 : 0);
    std::string room_id = std::to_string(real_id);
    RoomDataManager::getInstance()->initWithRoomId(room_id);
    Director::getInstance()->replaceScene(RoomScene::createScene());
}

bool SingleRoomCreator::init(RoomEntryLayer* scene, int id) {
    if (id < 1 || id > RoomCount)
        return false;
    _scene = scene;
    _room_id = id;
    // 添加 背景图
    std::string str_bg;
    if (_room_id == 1)       str_bg = "room_cc_cjc_2.png";
    else if (_room_id == 2)  str_bg = "room_cc_zjc_2.png";
    else if (_room_id == 3)  str_bg = "room_cc_gjc_2.png";
    else if (_room_id == 4)  str_bg = "room_cc_jyc_2.png";
    else if (_room_id == 5)  str_bg = "room_cc_dsc_2.png";
    else if (_room_id == 6)  str_bg = "room_cc_zzc_2.png";
    _bg = Sprite::createWithSpriteFrameName(str_bg);
    _bg->setPosition(_bg->getContentSize() * 0.5);
    this->addChild(_bg);
    this->setContentSize(_bg->getContentSize());
    Size s = _bg->getContentSize();
    // 添加 房间模式标签
    _mode_logo = Sprite::createWithSpriteFrameName("public_jb_bxp.png");
    _mode_logo->setPosition(_mode_logo->getContentSize().width * 0.5 + 10,
                            _mode_logo->getContentSize().height * 0.5 + 110);
    _bg->addChild(_mode_logo);
    updateModeLogo();
    // 添加 房间底分标签
    _ante_num = Label::createWithSystemFont("", "", 23);
    _ante_num->setTextColor(Color4B(0xff, 0xff, 0xff, 0xff * 0.86));
    _ante_num->setPosition(_bg->getContentSize().width * 0.5, 44);
    _bg->addChild(_ante_num);
    // 添加 房间入场金币标签
    _gold_range_num = Label::createWithSystemFont("", "", 23);
    _gold_range_num->setTextColor(Color4B(0xff, 0xff, 0xff, 0xff * 0.86));
    _gold_range_num->setPosition(_bg->getContentSize().width * 0.5, 72);
    _bg->addChild(_gold_range_num);
    return true;
}

void SingleRoomCreator::updateRoomDataDisplay() {
    int real_id = 110 + _room_id + ((_scene->_type == RoomEntryLayer::Type::FAKE) ? 10 : 0);
    std::string room_id = std::to_string(real_id);
    const rapidjson::Value& value = (RoomDataManager::getInstance()->getConfigDoc())[room_id.c_str()];
    const int ante = value["ante"].GetInt();
    const int gold_min = value["gold_min"].GetInt();
    const int gold_max = value["gold_max"].GetInt();
    std::string min_str, link_str, max_str;
    // TODD_xxf: 最好对数值做严格性校正——值是否能被1千整除
    if (gold_min < 10000)
        min_str = std::to_string(gold_min / 1000) + "千";
    else
        min_str = std::to_string(gold_min / 10000) + "万";
    if (gold_max == 0) {   // 0表示无上限
        max_str = "以上";
        link_str = "";
    } else if (gold_max < 10000) {
        max_str = std::to_string(gold_max / 1000) + "千";
        link_str = "-";
    } else {
        max_str = std::to_string(gold_max / 10000) + "万";
        link_str = "-";
    }
    _ante_num->setString("底分 " + std::to_string(ante));
    _gold_range_num->setString(min_str + link_str + max_str);
}
