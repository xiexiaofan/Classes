//
//  UICard.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/14.
//

#include "UICard.hpp"
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;

using namespace cocos2d;

enum zorder {
    z_base,
    z_suit_number,
    z_logo,
    z_mask
};

UICard::UICard()
: _data(nullptr)
, _model(Model::HANDCARD)
, _is_selected(false)
, _is_face_up(false)
, _need_redraw_faceup(true)
, _need_redraw_facedown(true)
, _suit_big(nullptr)
, _suit_small(nullptr)
, _number_sp(nullptr)
, _mask(nullptr)
, _ming_logo(nullptr)
, _landlord_logo(nullptr)
, _face_up(nullptr)
, _face_down(nullptr)
{}

UICard::~UICard() {
    CC_SAFE_RELEASE_NULL(_data);
}

UICard* UICard::create(Suit suit, int number) {
    UICard* ret = new UICard();
    if (ret && ret->init(suit, number))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

UICard* UICard::create(int id) {
    UICard* ret = new UICard();
    if (ret && ret->init(id))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

UICard* UICard::create(CardData* data) {
    UICard* ret = new UICard();
    if (ret && ret->init(data))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

void UICard::setModel(const Model& model) {
    if (_model == model)
        return;
    _model = model;
    _need_redraw_facedown = true;
    _need_redraw_faceup = true;
    judgeNeedRedraw();
}

void UICard::setFaceTowardUp(bool face_up) {
    _is_face_up = face_up;
    judgeNeedRedraw();
}

void UICard::addMask() {
    if (!_mask) {
        _mask = Sprite::createWithSpriteFrameName("poker_top.png");
        _mask->setColor(Color3B(0x20, 0x41, 0x34));
        _mask->setOpacity(102);
        this->addChild(_mask, z_mask);
    } else if (!_mask->isVisible()) {
        _mask->setVisible(true);
    }
}

void UICard::rmMask() {
    if (!hasMask())
        return;
    _mask->setVisible(false);
}

void UICard::addMingLogo() {
    
}

void UICard::rmMingLogo() {
    if (!hasMingLogo())
        return;
    _ming_logo->setVisible(false);
}

void UICard::addLandlordLogo() {
    
}

void UICard::rmLandlordLogo() {
    if (!hasLandlordLogo())
        return;
    _landlord_logo->setVisible(false);
}

Size UICard::getSizeAfterZoom() const {
    const Size& size = _is_face_up ? _face_up->getContentSize()
                                   : _face_down->getContentSize();
    return size * this->getScale();
}

Vec2 UICard::getCenterPosition() const {
    return this->getPosition();
}

Vec2 UICard::getLeftBottomPosition() const {
    return this->getPosition() - getSizeAfterZoom() * 0.5;
}

Vec2 UICard::getRightBottomPosition() const {
    const Size& size = getSizeAfterZoom();
    return getCenterPosition() + Size(size.width * 0.5, 0);
}

Vector<FiniteTimeAction*> UICard::getFlopAnimation() {
    RotateBy* rb = RotateBy::create(0.15, 0, 90);
    CallFunc* call = CallFunc::create([this]{
        this->setFaceTowardUp(true);
        this->setRotationSkewY(270);
    });
    RotateBy* rb_2 = RotateBy::create(0.15, 0, 90);
    return {rb, call, rb_2};
}

void UICard::openTouchListenerForDebug() {
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [&](Touch* touch, Event*){
        auto touch_in_node = this->convertTouchToNodeSpaceAR(touch);
        auto size = this->getSizeAfterZoom() / this->getScale();
        if (touch_in_node.x < size.width  * -0.5 ||
            touch_in_node.x > size.width  *  0.5 ||
            touch_in_node.y < size.height * -0.5 ||
            touch_in_node.y > size.height *  0.5)
            return false;
        this->switchSelected(!_is_selected);
        this->isSelected() ? this->addMask() : this->rmMask();
        SimpleAudioEngine::getInstance()->playEffect("sound/effect/select_card.mp3");
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool UICard::init(Suit suit, int number) {
    return init(CardData::create(suit, number));
}

bool UICard::init(int id) {
    return init(CardData::create(id));
}

bool UICard::init(CardData* data) {
    _data = data;
    CC_SAFE_RETAIN(_data);
    _face_down = Sprite::create();
    _face_up = Sprite::create();
    _face_down->setAnchorPoint(Vec2::ZERO);
    _face_up->setAnchorPoint(Vec2::ZERO);
    this->addChild(_face_down, z_base);
    this->addChild(_face_up, z_base);
    judgeNeedRedraw();
    
    return true;
}

void UICard::judgeNeedRedraw() {
    if (_is_face_up && _need_redraw_faceup) {
        _face_up->setVisible(true);
        _face_down->setVisible(false);
        drawFaceUp();
    }
    else if (!_is_face_up && _need_redraw_facedown) {
        _face_up->setVisible(false);
        _face_down->setVisible(true);
        drawFaceDown();
    }
}

void UICard::drawFaceUp() {
    std::string path_name = (_model == Model::RESTCARD ? "poker_top_s.png"
                             : "poker_top.png");
    _face_up->initWithSpriteFrameName(path_name);
    // number
    if (!_number_sp) {
        const std::string& num_str = StringUtils::format("num_b_%s.png", _data->getNumberStr().c_str());
        _number_sp = Sprite::createWithSpriteFrameName(num_str);
        _face_up->addChild(_number_sp, z_suit_number);
    }
    // suit small
    if (!_suit_small) {
        const Suit& suit = _data->getSuit();
        if (suit == Suit::JOKER) {
            if (_data->getNumber() == 16) {  // num = 16 定义成“小王”
                _number_sp->setColor(Color3B(0x1d, 0x0a, 0x01));
                _suit_small = Sprite::createWithSpriteFrameName("poker_joker_s.png");
            } else {
                _number_sp->setColor(Color3B(0xb5, 0x0c, 0x03));
                _suit_small = Sprite::createWithSpriteFrameName("poker_joker_d.png");
            }
        } else {
            if (suit == Suit::SPADE || suit == Suit::CLUB)
                _number_sp->setColor(Color3B(0x1d, 0x0a, 0x01));
            else
                _number_sp->setColor(Color3B(0xb5, 0x0c, 0x03));
            _suit_small = Sprite::createWithSpriteFrameName("poker_" + _data->getSuitStr() + "_s.png");
        }
        _face_up->addChild(_suit_small, z_suit_number);
    }
    
    if (_model == Model::HANDCARD)
        drawFaceUpForHandCard();
    else if (_model == Model::TABLECARD)
        drawFaceUpForTableCard();
    else if (_model == Model::RESTCARD)
        drawFaceUpForRestCard();
    
    _need_redraw_faceup = false;
}

void UICard::drawFaceDown() {
    std::string path_name = (_model == Model::RESTCARD ? "poker_back_s.png"
                                                       : "poker_back.png");
    _face_down->initWithSpriteFrameName(path_name);
    _need_redraw_facedown = false;
}

void UICard::drawFaceUpForHandCard() {
    // suit big
    if (_data->getSuit() != Suit::JOKER && !_suit_big) {
        _suit_big = Sprite::createWithSpriteFrameName("poker_" + _data->getSuitStr() + "_d.png");
        _face_up->addChild(_suit_big, z_suit_number);
    }
    
    _number_sp->setScale(1.0);
    float n_x = _number_sp->getContentSize().width*0.5 + 10;
    float n_y = _face_up->getContentSize().height - _number_sp->getContentSize().height*0.5 - 14;
    _number_sp->setPosition(n_x, n_y);
    
    if (_data->getSuit() != Suit::JOKER) {
        _suit_big->setScale(1.0);
        float sb_x = _face_up->getContentSize().width - _suit_big->getContentSize().width*0.5 - 8;
        float sb_y = _suit_big->getContentSize().height*0.5 + 10;
        _suit_big->setPosition(sb_x, sb_y);
        
        _suit_small->setScale(1.0);
        float ss_x = _number_sp->getPositionX();
        float ss_y = _number_sp->getPositionY() - _number_sp->getContentSize().height*0.5 - _suit_small->getContentSize().height*0.5 - 5;
        _suit_small->setPosition(ss_x, ss_y);
    } else {
        _suit_small->setScale(1.0);
        float ss_x = _face_up->getContentSize().width - _suit_small->getContentSize().width*0.5 - 5;
        float ss_y = _suit_small->getContentSize().height*0.5 + 10;
        _suit_small->setPosition(ss_x, ss_y);
    }
}

void UICard::drawFaceUpForTableCard() {
    if (_suit_big)
        _suit_big->removeFromParent();
    
    _number_sp->setScale(1.15);
    float n_x = _number_sp->getContentSize().width*0.5 + 14;
    float n_y = _face_up->getContentSize().height - _number_sp->getContentSize().height*0.5 - 14;
    _number_sp->setPosition(n_x, n_y);
    
    if (_data->getSuit() != Suit::JOKER) {
        _suit_small->setScale(1.15);
        float ss_x = _number_sp->getPositionX();
        float ss_y = _number_sp->getPositionY() - _number_sp->getContentSize().height*0.5 - _suit_small->getContentSize().height*0.5 - 10;
        _suit_small->setPosition(ss_x, ss_y);
    } else {
        _suit_small->setScale(1.0);
        float ss_x = _face_up->getContentSize().width - _suit_small->getContentSize().width*0.5 - 10;
        float ss_y = _suit_small->getContentSize().height*0.5 + 10;
        _suit_small->setPosition(ss_x, ss_y);
        _number_sp->setPositionY(_number_sp->getPositionY() - 5);
    }
}

void UICard::drawFaceUpForRestCard() {
    if (_suit_big)
        _suit_big->removeFromParent();
    
    if (_data->getSuit() != Suit::JOKER) {
        _number_sp->setScale(0.42);
        float n_x = _number_sp->getBoundingBox().size.width*0.5 + 4;
        float n_y = _face_up->getContentSize().height - _number_sp->getBoundingBox().size.height*0.5 - 4;
        _number_sp->setPosition(n_x, n_y);
        
        _suit_small->setScale(0.42);
        float ss_x = _face_up->getContentSize().width - _suit_small->getBoundingBox().size.width*0.5 - 4;
        float ss_y = _suit_small->getBoundingBox().size.height*0.5 + 4;
        _suit_small->setPosition(ss_x, ss_y);
    } else {
        _number_sp->setScale(0.3, 0.24);
        float n_x = _number_sp->getBoundingBox().size.width*0.5 + 3;
        float n_y = _face_up->getContentSize().height - _number_sp->getBoundingBox().size.height*0.5 - 3;
        _number_sp->setPosition(n_x, n_y);
        
        _suit_small->setScale(0.25);
        float ss_x = _face_up->getContentSize().width - _suit_small->getBoundingBox().size.width*0.5 - 4;
        float ss_y = _suit_small->getBoundingBox().size.height*0.5 + 4;
        _suit_small->setPosition(ss_x, ss_y);
    }
}
