//
//  CardData.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/9.
//

#include "CardData.hpp"

CardData* CardData::create(Suit suit, int number) {
    CardData* ret = new CardData();
    if (ret && ret->init(suit, number))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

CardData* CardData::create(int id) {
    CardData* ret = new CardData();
    if (ret && ret->init(id))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

std::string CardData::getSuitStr() const {
    if (_suit == Suit::JOKER)
        return "joker";
    else if (_suit == Suit::SPADE)
        return "spade";
    else if (_suit == Suit::HEART)
        return "heart";
    else if (_suit == Suit::CLUB)
        return "club";
    else if (_suit == Suit::DIAMOND)
        return "diamond";
    else
        return "";  // Suit::INVALID
}

std::string CardData::getNumberStr() const {
    if (_number == 11)
        return "j";
    else if (_number == 12)
        return "q";
    else if (_number == 13)
        return "k";
    else if (_number == 14)
        return "a";
    else if (_number == 15)
        return "2";
    else if (_number == 16 || _number == 17)
        return "joker";
    else
        return std::to_string(_number);  // 3 ~ 10
}

void CardData::print() const {
    ; // TODD
}

bool CardData::init(Suit suit, int number) {
    if (number < 3 || number > 17)
        return false;
    _suit = suit;
    _number = number;
    if (_suit == Suit::JOKER)
        _id = _number == 16 ? 53 : 54;
    else
        _id = (_number-3) * 4 + static_cast<int>(_suit);
    return true;
}

bool CardData::init(int id) {
    if (id < 1 || id > 54)
        return false;
    _id = id;
    if (_id > 52) {
        _suit = Suit::JOKER;
        _number = _id == 53 ? 16 : 17;
    } else {
        _suit = static_cast<Suit>((_id-1)%4 + 1);
        _number = (_id - 1) / 4 + 3;
    }
    return true;
}
