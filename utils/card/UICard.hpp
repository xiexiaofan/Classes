//
//  UICard.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/14.
//

#ifndef UICard_hpp
#define UICard_hpp

#include "cocos2d.h"
#include "CardData.hpp"

class UICard : public cocos2d::Node
{
public:
    using Suit = CardData::Suit;
    
    enum class Model {
        HANDCARD,    // 未打出的手牌(默认状态）
        TABLECARD,   // 已打出的手牌
        RESTCARD     // 底牌
    };
    
    static UICard* create(Suit suit, int number);
    static UICard* create(int id);
    static UICard* create(CardData* data);
    
    CardData* getCardData() const { return _data; }
    const Model& getModel() const { return _model; }
    void setModel(const Model& model);
    
    bool isFaceTowardUp() const   { return _is_face_up; }
    void setFaceTowardUp(bool face_up);
    
    void switchSelected()   { _is_selected = !_is_selected; }
    bool isSelected() const { return _is_selected; }
    
    // mask && logo
    bool hasMask() const         { return _mask && _mask->isVisible(); }
    void addMask();
    void rmMask();
    bool hasMingLogo() const     { return _ming_logo && _ming_logo->isVisible(); }
    void addMingLogo();
    void rmMingLogo();
    bool hasLandlordLogo() const { return _landlord_logo && _landlord_logo->isVisible(); }
    void addLandlordLogo();
    void rmLandlordLogo();
    
    cocos2d::Size getSizeAfterZoom() const;
    cocos2d::Vec2 getCenterPosition() const;
    cocos2d::Vec2 getLeftBottomPosition() const;
    cocos2d::Vec2 getRightBottomPosition() const;
    
    cocos2d::Vector<cocos2d::FiniteTimeAction*> getFlopAnimation();
public:
    UICard();
    virtual ~UICard();
    
private:
    bool init(Suit suit, int number);
    bool init(int id);
    bool init(CardData* data);
    
    void judgeNeedRedraw();
    void drawFaceUp();
    void drawFaceDown();
    void drawFaceUpForHandCard();
    void drawFaceUpForTableCard();
    void drawFaceUpForRestCard();
    
private:
    CardData* _data;
    Model _model;
    bool _is_selected;
    bool _is_face_up;
    bool _need_redraw_faceup;
    bool _need_redraw_facedown;
    cocos2d::Sprite* _suit_big;
    cocos2d::Sprite* _suit_small;
    cocos2d::Sprite* _number_sp;
    cocos2d::Sprite* _mask;
    cocos2d::Sprite* _ming_logo;
    cocos2d::Sprite* _landlord_logo;
    CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _face_up, FaceUp);
    CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _face_down, FaceDown);
};

#endif /* UICard_hpp */
