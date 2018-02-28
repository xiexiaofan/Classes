//
//  UISeat.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#include "UISeat.hpp"
#include "PlayerData.hpp"
#include "CommonMethodSet.hpp"
#include "ui/UIScale9Sprite.h"
using namespace cocos2d;
using ui::Scale9Sprite;

static const Size& ImageSize    = Size(126, 126);
static const Size& InfoBarSize  = Size(124, 46);
static std::string Str_Image    = "head_frame";
static std::string Str_Gold     = "gold";
static std::string Str_InfoBar  = "info_bar";

void UISeat::loadPlayerData(PlayerData* pdata) {
    _pdata = pdata;
    updateDisplay();
}

void UISeat::unloadPlayerData() {
    _pdata = nullptr;
    updateDisplay();
}

void UISeat::updateDisplayForImage() {
    CC_ASSERT(_pdata != nullptr);
    if (_image) {
        _image->initWithFile(std::string("image/icon/") + _pdata->getPlayerConfig().image);
        _image->setContentSize(ImageSize);
    } else {
        _image = Sprite::create(std::string("image/icon/") + _pdata->getPlayerConfig().image);
        _image->setContentSize(ImageSize);
        _image->setPosition(69, 116);
        this->addChild(_image, 0);
    }
}

void UISeat::updateDisplayForName() {
    CC_ASSERT(_pdata != nullptr);
    if (_name) {
        _name->setString(_pdata->getPlayerConfig().name);
    } else {
        _name = Label::createWithSystemFont(_pdata->getPlayerConfig().name, "", 17);
        _name->setColor(Color3B(239, 233, 183));
        _name->setPosition(69, 34);
        this->addChild(_name);
    }
}

void UISeat::updateDisplayForGold() {
    CC_ASSERT(_pdata != nullptr);
    if (_gold) {
        _gold->setString(std::to_string(_pdata->getPlayerConfig().gold));
    } else {
        _gold = Label::createWithTTF(std::to_string(_pdata->getPlayerConfig().gold), "fonts/arial.ttf", 18);
        _gold->setColor(Color3B(255, 255, 0));
        _gold->setAnchorPoint(Vec2(0, 0.5));
        _gold->setPosition(50, 14);
        this->addChild(_gold);
        this->getChildByName(Str_InfoBar)->setVisible(true);
        this->getChildByName(Str_Gold)->setVisible(true);
    }
}

void UISeat::displayerIdLogo(bool landlord, bool left) {
    if (landlord)
        _id_logo->initWithSpriteFrameName("table_dizhu.png");
    else
        _id_logo->initWithSpriteFrame(Sprite::create("extra/table_farmer.png")->getSpriteFrame());
    
    // 位置微调
    if (left)
        _id_logo->setPosition(-11, 150);
    else
        _id_logo->setPosition(151, 150);
    
    _id_logo->setVisible(true);
}

void UISeat::rmplayerIdLogo() {
    _id_logo->setVisible(false);
}

bool UISeat::init() {
    // 头像边框
    Scale9Sprite* head_frame = Scale9Sprite::createWithSpriteFrameName("public_head_frame.png");
    head_frame->setContentSize(ImageSize + Size(12, 12));
    head_frame->setPosition(69, 116);
    this->addChild(head_frame, 1, Str_Image);
    // 头像框
    Sprite* head_free = Sprite::create("image/icon/table_head_kong.jpg");
    head_free->setContentSize(ImageSize);
    head_free->setPosition(69, 116);
    this->addChild(head_free, 0);
    // 昵称&金币框
    Scale9Sprite* info_sp = Scale9Sprite::createWithSpriteFrameName("table_name_bg.png");
    info_sp->setCapInsets(Rect(20, 10, 46, 26));
    info_sp->setContentSize(InfoBarSize);
    info_sp->setPosition(69, 23);
    info_sp->setVisible(false);
    this->addChild(info_sp, 0, Str_InfoBar);
    Sprite* gold_sp = Sprite::createWithSpriteFrameName("icon_small_gold.png");
    gold_sp->setContentSize(Size(24, 24));
    gold_sp->setPosition(35, 15);
    gold_sp->setVisible(false);
    this->addChild(gold_sp, 0, Str_Gold);
    
    // 身份logo
    _id_logo = Sprite::create();
    _id_logo->setVisible(false);
    this->addChild(_id_logo, 1);
    
    // 注册点击回调事件：点击有效区域——头像边框内
    addListenerForClicked();
    return true;
}

void UISeat::updateDisplay() {
    if (!_pdata) {
        if (_image) {
            _image->removeFromParent();
            _image = nullptr;
        }
        if (_name) {
            _name->removeFromParent();
            _name = nullptr;
        }
        if (_gold) {
            _gold->removeFromParent();
            _gold = nullptr;
            this->getChildByName(Str_Gold)->setVisible(false);
            this->getChildByName(Str_InfoBar)->setVisible(false);
        }
    } else {
        updateDisplayForImage();
        updateDisplayForName();
        updateDisplayForGold();
    }
}

void UISeat::addListenerForClicked() {
    _listener = EventListenerTouchOneByOne::create();
    _listener->onTouchBegan = [&](Touch* touch, Event*)->bool{
        if (_xxf::isTouchPointAtValidScope(touch, this->getChildByName(Str_Image))) {
            static int count = 0;
            log("touch it %d", count++);
            return true;
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);
}
