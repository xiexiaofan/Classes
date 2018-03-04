//
//  SimpleAiActionManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/3/4.
//

#include "SimpleAiActionManager.hpp"
using namespace cocos2d;

bool SimpleAiActionManager::doActionCall(int id) {
    return false;
}

bool SimpleAiActionManager::doActionRob(int id) {
    return false;
}

bool SimpleAiActionManager::doActionBet(int id) {
    return false;
}

bool SimpleAiActionManager::doActionDouble(int id) {
    return false;
}

CardType SimpleAiActionManager::doActionPlay(int id) {
    return CardType();
}

CardType SimpleAiActionManager::doActionPlay(CardVec& vec, int id) {
    if (id == _pre_id) {
        auto ret = identityCardType(vec);
        if (ret.getCTName() != CTName::Undef) {
            _pre_type = ret;
            sortCardVec(vec, ret);
            rmNumData(vec, id);
            return ret;
        } else {
            return CardType();
        }
    } else {
        auto ret = identityCardType(vec);
        if (CardType::compare(ret, _pre_type) == CTCmpRes::Greater) {
            _pre_id = id;
            _pre_type = ret;
            sortCardVec(vec, ret);
            rmNumData(vec, id);
            return ret;
        } else {
            return CardType();
        }
    }
}

void SimpleAiActionManager::initNumMap(const std::map<int, CardVec>& card_map) {
    addNumData(card_map.at(0), 0);
    addNumData(card_map.at(1), 1);
    addNumData(card_map.at(2), 2);
}

void SimpleAiActionManager::addNumData(const CardVec& vec, int id) {
    auto& NumVec = _num_map[id];
    for (auto& card : vec) NumVec.push_back(card->getCardData()->getNumber());
    std::sort(NumVec.begin(), NumVec.end(), [](const int& n1, const int n2){return n1 > n2;});
}

void SimpleAiActionManager::rmNumData(const CardVec& vec, int id) {
    auto& NumVec = _num_map[id];
    for (auto& card : vec) {
        int rm_num = card->getCardData()->getNumber();
        auto iter = std::find(NumVec.begin(), NumVec.end(), rm_num);
        if (iter != NumVec.end()) NumVec.erase(iter);
    }
}

void SimpleAiActionManager::clearAllNumData() {
    _num_map.clear();
    _pre_id = -1;
    _pre_type = CardType();
}

SimpleAiActionManager::CardVec SimpleAiActionManager::findCardVec(const CardVec& src, const CardType& ct) {
    CardVec ret;
    return ret;
}

void SimpleAiActionManager::sortCardVec(CardVec& vec, const CardType& ct) {
    const auto& stem_vec = ct.getStemVec();
    const auto& tail_vec = ct.getTailVec();
    const size_t& s_size = stem_vec.size();
    const size_t& t_size = tail_vec.size();

    auto findIndex = [&vec](const size_t& first, const size_t& last, int target)->size_t{
        size_t ret = -1;
        for (size_t i = first; first < last; ++i)
            if (vec.at(i)->getCardData()->getNumber() == target)
                return i;
        return ret;
    };

    size_t i = 0;
    for (; i < s_size; ++i) {
        int num = stem_vec.at(i);
        size_t x = findIndex(i, vec.size(), num);
        std::swap(vec.at(i), vec.at(x));
    }
    std::sort(vec.begin(), vec.begin() + i, [](const UICard* c1, const UICard* c2){
        return c1->getCardData()->getId() > c2->getCardData()->getId();
    });
    size_t j = 0;
    for (; j < t_size; ++j) {
        int num = tail_vec.at(j);
        size_t x = findIndex(i+j, vec.size(), num);
        std::swap(vec.at(i+j), vec.at(x));
    }
    std::sort(vec.begin() + i, vec.end(), [](const UICard* c1, const UICard* c2){
        return c1->getCardData()->getId() > c2->getCardData()->getId();
    });
}

CardType SimpleAiActionManager::identityCardType(const CardVec& vec) {
    NumVec num_vec;
    for (auto& card : vec) num_vec.push_back(card->getCardData()->getNumber());
    std::sort(num_vec.begin(), num_vec.end(), [](const int& i1, const int& i2){return i1 < i2;});
    return CardTypeHelper::identifyCardType(num_vec);
}

