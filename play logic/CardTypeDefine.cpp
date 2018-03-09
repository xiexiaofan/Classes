//
//  CardTypeDefine.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#include "CardTypeDefine.hpp"
#include "CardTypeHelper.hpp"

static const std::map<CTName, std::string> DesMap {
    {CTName::Undef,      "未定义"},
    {CTName::Single,     "单张"},
    {CTName::Pair,       "对子"},
    {CTName::Tri,        "三张"},
    {CTName::Tri_t,      "三带一"},
    {CTName::Four_tt,    "四带二"},
    {CTName::Str_Single, "顺子"},
    {CTName::Str_Pair,   "连对"},
    {CTName::Str_Tri,    "飞机"},
    {CTName::Str_Tri_t,  "飞机带翅膀"},
    {CTName::Bomb,       "炸弹"},
    {CTName::Rocket,     "火箭"},
};

CTLevel CardType::getLevel(const CTName& name) {
    CTLevel ret = CTLevel::L0;
    switch (name) {
        case CTName::Undef: break;
        case CTName::Rocket: ret = CTLevel::L5; break;
        case CTName::Bomb:   ret = CTLevel::L3; break;
        default: ret = CTLevel::L1; break;
    }
    return ret;
}

CTCmpRes CardType::compare(const CardType& left, const CardType& right) {
    CTLevel lv1 = CardType::getLevel(left._name);
    CTLevel lv2 = CardType::getLevel(right._name);
    if (lv1 > lv2)
        return CTCmpRes::Greater;
    if (lv1 < lv2)
        return CTCmpRes::Less_equal;
    
    if (left._name != right._name)
        return CTCmpRes::Undef_name;
    if (left.getStrLength() != right.getStrLength())
        return CTCmpRes::Undef_str;
    if (left.getTailType() != right.getTailType())
        return CTCmpRes::Undef_tail;
    
    return left.getCmpNumber() > right.getCmpNumber() ? CTCmpRes::Greater : CTCmpRes::Less_equal;
}

bool CardType::judge(const CTName& name, const NumVec& vec) {
    NumPair pair = CardType::split(name, vec);
    return !pair.first.empty();
}

NumPair CardType::split(const CTName& name, const NumVec& vec) {
    // @brief： 依次将src中的每个元素复制times份插入到dest末尾位置
    auto insertVal = [](const NumVec& src, NumVec& dest, const size_t& times){
        for (size_t i = 0; i < src.size(); ++i)
            for (size_t j = 0; j < times; ++j)
                dest.push_back(src[i]);
    };
    NumPair ret;
    size_t size = vec.size();
    NumMap map = CardTypeHelper::splitByDepth(vec);
    switch (name) {
        case CTName::Undef: break;
            
        case CTName::Single:
            if (size == 1)
                ret.first = NumVec(1, map[1][0]);
            break;
            
        case CTName::Pair:
            if (size == 2 && map[2].size() == 1)
                ret.first = NumVec(2, map[2][0]);
            break;
            
        case CTName::Tri:
            if (size == 3 && map[3].size() == 1)
                ret.first = NumVec(3, map[3][0]);
            break;
            
        case CTName::Tri_t:
            if (map[4].size() == 0 &&
                map[3].size() == 1 &&
                map[2].size() + map[1].size() == 1) {
                ret.first = NumVec(3, map[3][0]);
                ret.second = map[2].size() == 0 ? NumVec(1, map[1][0])
                                                : NumVec(2, map[2][0]);
            }
            break;
            
        case CTName::Four_tt:
            if (map[4].size() == 1 && map[3].size() == 0) {
                if ((map[2].size() == 0 && map[1].size() == 2) ||
                    (map[2].size() == 1 && map[1].size() == 0) ||
                    (map[2].size() == 2 && map[1].size() == 0)) {
                    ret.first = NumVec(4, map[4][0]);
                    if (!map[2].empty()) insertVal(map[2], ret.second, 2);
                    if (!map[1].empty()) insertVal(map[1], ret.second, 1);
                }
            }
            break;
            
        case CTName::Str_Single:
            if (map[4].size() == 0 &&
                map[3].size() == 0 &&
                map[2].size() == 0 &&
                map[1].size() >= 5) {
                size_t s = map[1].size();
                if (map[1][s-1] - map[1][0] == s - 1 && map[1][s-1] < 15)
                    insertVal(map[1], ret.first, 1);
            }
            break;
            
        case CTName::Str_Pair:
            if (map[4].size() == 0 &&
                map[3].size() == 0 &&
                map[2].size() >= 3 &&
                map[1].size() == 0) {
                size_t s = map[2].size();
                if (map[2][s-1] - map[2][0] == s - 1 && map[2][s-1] < 15)
                    insertVal(map[2], ret.first, 2);
            }
            break;
            
        case CTName::Str_Tri:
            if (map[4].size() == 0 &&
                map[3].size() >= 2 &&
                map[2].size() == 0 &&
                map[1].size() == 0) {
                size_t s = map[3].size();
                if (map[3][s-1] - map[2][0] == s - 1 && map[3][s-1] < 15)
                    insertVal(map[3], ret.first, 3);
            }
            break;
            
        case CTName::Str_Tri_t:
            if (map[4].size() == 0 && map[3].size() >= 2) {
                size_t s1 = map[1].size();
                size_t s2 = map[2].size();
                size_t s3 = map[3].size();
                auto str_vec = CardTypeHelper::splitStraight(map[3], 2);
                for (auto iter = str_vec.rbegin(); iter != str_vec.rend(); ++iter){
                    const NumVec& vec = *iter;
                    size_t len = vec.size();
                    if (len == (s3-len)*3 + s2*2 + s1 || (s3 == len && s2 == len && s1 == 0)) {
                        insertVal(vec, ret.first, 3);
                        if (s1 != 0) insertVal(map[1], ret.second, 1);
                        if (s2 != 0) insertVal(map[2], ret.second, 2);
                        if (s3 != len) {
                            for (int& i : map[3])
                                if (std::find(ret.first.begin(), ret.first.end(), i) == ret.first.end()) {
                                    ret.second.push_back(i);
                                    ret.second.push_back(i);
                                    ret.second.push_back(i);
                                }
                        }
                        sortByOrder(ret.second);
                        break;
                    }
                }
            }
            break;
            
        case CTName::Bomb:
            if (size == 4 && map[4].size() == 1)
                ret.first = NumVec(4, map[4][0]);
            break;
            
        case CTName::Rocket:
            if (size == 2 && map[1].size() == 2)
                if (map[1][0] == 16 && map[1][1] == 17)
                    ret.first = NumVec{16, 17};
            break;
    }
    return ret;
}

CardType CardType::build(const CTName& name, const NumVec& vec) {
    return CardType::build(name, CardType::split(name, vec));
}

CardType CardType::build(const CTName& name, const NumVec& stem, const NumVec& tail) {
    return CardType::build(name, std::make_pair(stem, tail));
}

CardType CardType::build(const CTName& name, const NumPair& pair) {
    return pair.first.empty() ? CardType() : CardType(name, pair);
}

std::string CardType::getDescribe() const {
    std::string ret = DesMap.at(this->_name);
    ret += std::to_string(this->getCmpNumber());
    ret += "-";
    ret += std::to_string(this->getStrLength());
    ret += "-";
    ret += std::to_string(this->getTailType());
    return ret;
}

int CardType::getCmpNumber() const {
    return _stem_vec.empty() ? 0 : _stem_vec[0];
}

size_t CardType::getStrLength() const {
    std::set<int> unique_set(_stem_vec.begin(), _stem_vec.end());
    return unique_set.size();
}

size_t CardType::getTailType() const {
    size_t ret = _tail_vec.size() / getStrLength();
    return _name == CTName::Four_tt ? ret / 2 : ret;
}

CardType::CardType()
: _name(CTName::Undef)
, _stem_vec(NumVec())
, _tail_vec(NumVec())
{}

CardType::CardType(const CTName& name, const NumPair& pair)
: _name(name)
, _stem_vec(pair.first)
, _tail_vec(pair.second)
{}
