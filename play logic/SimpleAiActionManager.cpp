//
//  SimpleAiActionManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/3/4.
//

#include "SimpleAiActionManager.hpp"

static SimpleAiActionManager* Instance = nullptr;

SimpleAiActionManager* SimpleAiActionManager::getInstance() {
    if (!Instance)
        Instance = new SimpleAiActionManager();
    return Instance;
}

void SimpleAiActionManager::destroyInstance() {
    if (Instance) {
        delete Instance;
        Instance = nullptr;
    }
}

 RestCTName SimpleAiActionManager::identifyRestCard(const CardVec& vec) {
    CC_ASSERT(vec.size() == 3);
    int king_count = 0;
    if (vec.at(0)->getCardData()->getSuit() == CardData::Suit::JOKER) ++king_count;
    if (vec.at(1)->getCardData()->getSuit() == CardData::Suit::JOKER) ++king_count;
    if (vec.at(2)->getCardData()->getSuit() == CardData::Suit::JOKER) ++king_count;
    
    CC_ASSERT(king_count != 3);
    if (king_count == 2)
        return RestCTName::DoubleKing;
    if (king_count == 1)
        return RestCTName::SingleKing;

    const int& v0 = vec.at(0)->getCardData()->getNumber();
    const int& v1 = vec.at(1)->getCardData()->getNumber();
    const int& v2 = vec.at(2)->getCardData()->getNumber();
    if (v0 == v1 && v0 == v2)
        return RestCTName::Three;

    if ((vec.at(0)->getCardData()->getSuit() == vec.at(1)->getCardData()->getSuit()) &&
        (vec.at(0)->getCardData()->getSuit() == vec.at(2)->getCardData()->getSuit()))
        return RestCTName::Flush;

    if (v0<15 && v1<15 && v2<15) {
        if ((v0*2 == v1 + v2 && std::abs(v2-v1) == 2) ||
            (v1*2 == v0 + v2 && std::abs(v2-v0) == 2) ||
            (v2*2 == v0 + v1 && std::abs(v1-v0) == 2))
            return RestCTName::Straight;
    }

    if (v0==v1 || v0==v2 || v1==v2)
        return RestCTName::Pair;

    return RestCTName::Common;
 }

int SimpleAiActionManager::getRestCardMutiple(const RestCTName& name) {
    int ret = 1;
    switch(name) {
        case RestCTName::DoubleKing: ret = 4; break;
        case RestCTName::Three:      ret = 4; break;
        case RestCTName::Flush:      ret = 3; break;
        case RestCTName::Straight:   ret = 3; break;
        case RestCTName::SingleKing: ret = 2; break;
        case RestCTName::Pair:       ret = 2; break;
        case RestCTName::Common:     break;
    }
    return ret;
}

bool SimpleAiActionManager::doActionCall(int id) {
    return false;
}

bool SimpleAiActionManager::doActionRob(int id) {
    return false;
}

int SimpleAiActionManager::doActionBet(int id) {
    return 0;
}

bool SimpleAiActionManager::doActionDouble(int id) {
    return false;
}

CardType SimpleAiActionManager::doActionPlay(int id) {
    return CardType();
}

CardType SimpleAiActionManager::doActionPlay(CardVec& vec, int id) {
    auto ret = identityCardType(vec);
    if (id == _pre_id && ret.getCTName() != CTName::Undef) {
        _pre_type = ret;
        sortCardVec(vec, ret);
        rmNumData(vec, id);
        return ret;
    } else if (id != _pre_id && CardType::compare(ret, _pre_type) == CTCmpRes::Greater) {
        _pre_id = id;
        _pre_type = ret;
        sortCardVec(vec, ret);
        rmNumData(vec, id);
        return ret;
    }
    return CardType();
}

const std::vector<NumVec>& SimpleAiActionManager::getPlaySeqVec(int id) {
    if (_play_seq_vec.empty())
        buildPlaySeqVec(id);
    return _play_seq_vec;
}

NumVec SimpleAiActionManager::getPlayNumVec(int id) {
    const auto& seq_vec = getPlaySeqVec(id);
    if (seq_vec.empty())
        return {};
    if (_cur_index == seq_vec.size())
        _cur_index = 0;
    return seq_vec.at(_cur_index++);
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
    initPlaySeqVec();
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
    std::sort(num_vec.begin(), num_vec.end());
    return CardTypeHelper::identifyCardType(num_vec);
}

void SimpleAiActionManager::buildPlaySeqVec(int id) {
    NumVec num_vec = _num_map.at(id);
    std::sort(num_vec.begin(), num_vec.end());
    _play_seq_vec = id == _pre_id ? CardTypeHelper::buildCardType(num_vec)
                                  : CardTypeHelper::buildCardType(num_vec, _pre_type);
    _cur_index = 0;
}

void SimpleAiActionManager::initPlaySeqVec() {
    if (!_play_seq_vec.empty()) {
        std::vector<NumVec> empty_vec;
        std::swap(_play_seq_vec, empty_vec);
    }
    _cur_index = 0;
}
