//
//  UITable.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/12.
//

#include "UITable.hpp"
#include "RoomData.hpp"
#include "ui/UIScale9Sprite.h"
using namespace cocos2d;
using ui::Scale9Sprite;

UITable* UITable::create(Type type) {
    UITable* ret = new UITable();
    if (ret && ret->init(type))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

bool UITable::init(Type type) {
    _table_bg = Sprite::create();
    _table = Sprite::create();
    this->addChild(_table_bg);
    this->addChild(_table);
    return initTableImage(type);
}

void UITable::displayForGameNotBegin() {
    _table_name == nullptr ? createTableNameFont()    : _table_name->setVisible(true);
    _table_des  == nullptr ? createTableDescirbeWin() : _table_des->setVisible(true);
}

void UITable::displayForGameBegin() {
    if (_table_name && _table_name->isVisible())
        _table_name->setVisible(false);
    if (_table_des && _table_des->isVisible())
        _table_des->setVisible(false);
}

bool UITable::initTableImage(Type type) {
    _type = type;
    std::string path_name = "";
    if (_type == Type::Default)
        path_name = "default";
    else if (_type == Type::Ring)
        path_name = "ltx";
    else if (_type == Type::Tollgate)
        path_name = "cg";
    else if (_type == Type::Desert)
        path_name = "sham";
    else if (_type == Type::Waterfall)
        path_name = "pb";
    else if (_type == Type::Glacier)
        path_name = "bc";
    const std::string& path_pre = "image/room/table_bg_";
    _table->initWithFile(path_pre + path_name + "_01.png");
    _table_bg->initWithFile(path_pre + path_name + "_02.jpg");
    _table->setPosition(0, _table->getContentSize().height * -0.235);
    this->setContentSize(_table_bg->getContentSize());
    return true;
}

void UITable::createTableNameFont() {
    _table_name = Label::createWithSystemFont("", "", 35);
    _table_name->setTextColor(Color4B(0x00, 0x00, 0x00, 0x33));
    _table_name->setPosition(0, 70);
    this->addChild(_table_name);
}

void UITable::createTableDescirbeWin() {
    _table_des = Node::create();
    _table_des->setPosition(455, -260);
    this->addChild(_table_des);
    Scale9Sprite* bg_image = Scale9Sprite::createWithSpriteFrameName("room_limit_bg.png");
    bg_image->setCapInsets(Rect(15, 15, 40, 28));
    bg_image->setContentSize(Size(202, 82));
    _table_des->addChild(bg_image);
    // label
    Label* double_limit_font = Label::createWithSystemFont("加倍限制: ", "", 18);
    double_limit_font->setColor(Color3B(0xfe, 0xfe, 0xfe));
    double_limit_font->setPosition(-42, 16);
    _table_des->addChild(double_limit_font);
    Label* settle_limit_font = Label::createWithSystemFont("输赢封顶: ", "", 18);
    settle_limit_font->setColor(Color3B(0xfe, 0xfe, 0xfe));
    settle_limit_font->setPosition(-42, -16);
    _table_des->addChild(settle_limit_font);
    // icon
    Sprite* gold_up = Sprite::createWithSpriteFrameName("icon_small_gold.png");
    gold_up->setPosition(8, 16);
    _table_des->addChild(gold_up);
    Sprite* gold_down = Sprite::createWithSpriteFrameName("icon_small_gold.png");
    gold_down->setPosition(8, -16);
    _table_des->addChild(gold_down);
    // value
    std::string double_val = std::to_string(RoomDataManager::getInstance()->getRoomConfig().double_limit);
    std::string settle_val = std::to_string(RoomDataManager::getInstance()->getRoomConfig().settle_limit);
    Label* double_limit_num = Label::createWithSystemFont(double_val=="0" ? "无上限":double_val, "", 18);
    double_limit_num->setColor(Color3B(0xfe, 0xfe, 0xfe));
    double_limit_num->setAnchorPoint(Vec2::ZERO);
    double_limit_num->setPosition(25, 5);
    _table_des->addChild(double_limit_num);
    Label* settle_limit_num = Label::createWithSystemFont(settle_val=="0" ? "无上限":settle_val, "", 18);
    settle_limit_num->setColor(Color3B(0xfe, 0xfe, 0xfe));
    settle_limit_num->setAnchorPoint(Vec2::ZERO);
    settle_limit_num->setPosition(25, -27);
    _table_des->addChild(settle_limit_num);
}
