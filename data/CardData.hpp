//
//  CardData.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/9.
//

#ifndef CardData_hpp
#define CardData_hpp

#include "base/CCRef.h"
#include <string>

class CardData : public cocos2d::Ref
{
public:
    enum class Suit {
        INVALID,
        DIAMOND,
        CLUB,
        HEART,
        SPADE,
        JOKER
    };
    static CardData* create(Suit suit, int number);
    static CardData* create(int id);
    
    std::string getSuitStr() const;
    std::string getNumberStr() const;
    void print() const;
    
private:
    CardData() : _suit(Suit::INVALID), _number(0), _id(0) {}
    
    bool init(Suit suit, int number);
    bool init(int id);
    
    CC_SYNTHESIZE_READONLY(Suit, _suit, Suit);
    CC_SYNTHESIZE_READONLY(int, _number, Number);
    CC_SYNTHESIZE_READONLY(int, _id, Id);
};

#endif /* CardData_hpp */
