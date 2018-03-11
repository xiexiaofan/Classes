//
//  CardTypeHelper.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#include "CardTypeHelper.hpp"
#include <algorithm>

NumMap CardTypeHelper::splitByDepth(const NumVec& src) {
    if (src.empty())
        return {};
    NumMap ret;
    int count = 0;
    int num = -1;
    for (size_t i = 0; i != src.size(); ++i) {
        if (num != -1 && num != src[i]) {
            ret[count].push_back(num);
            count = 0;
        }
        num = src[i];
        count++;
    }
    // 处理最后一个元素
    ret[count].push_back(num);
    return ret;
}

NumMap CardTypeHelper::splitByRange(const NumVec& src) {
    if (src.empty())
        return {};
    NumMap ret;
    int count = 0;
    int num = -1;
    for (size_t i = 0; i != src.size(); ++i) {
        if (num != src[i])
            count = 0;
        num = src[i];
        count++;
        ret[count].push_back(num);
    }
    return ret;
}

std::vector<NumVec> CardTypeHelper::splitStraight(const NumVec& src,
                                                  int min_len,
                                                  int limit_number) {
    if (src.empty())
        return {};
    // #1 找到符合要求的最大顺子序列(不同顺子之间不相连）
    std::vector<NumVec> longer_vec;
    int num = 0;  /* vec中不会出现元素0 */
    int count = 0;
    auto iter = src.begin();
    while (iter != src.end() && *iter <= limit_number) {
        if (num == (*iter) - 1 || num == 0)
            ++count;
        else {
            if (count >= min_len)
                longer_vec.push_back(NumVec(iter - count, iter));
            count = 1;
        }
        num = *iter;
        ++iter;
    }
    if (count >= min_len)
        longer_vec.push_back(NumVec(iter - count, iter));
    
    // #2 按顺子长度拆分最大顺子序列
    std::vector<NumVec> ret;
    for (auto& vec : longer_vec) {
        auto len = vec.size();
        while (len >= min_len) {
            auto iter = vec.begin();
            while (len <= vec.end() - iter) {
                ret.push_back(NumVec(iter, iter + len));
                ++iter;
            }
            --len;
        }
    }
    // #3 按顺子长度由小到大排序
    std::stable_sort(ret.begin(), ret.end(), [](const NumVec& v1, const NumVec& v2){
        return v1.size() < v2.size();
    });
    
    return ret;
}

CardType CardTypeHelper::identifyCardType(const NumVec& src) {
    CardType ret;
    bool is_found = false;
    auto itentify = [&](const std::vector<CTName>& order_check_vec){
        for (auto& name : order_check_vec) {
            NumPair pair = CardType::split(name, src);
            if (!pair.first.empty()) {
                ret = CardType::build(name, pair);
                is_found = true;
                break;
            }
        }
    };
    NumMap map = CardTypeHelper::splitByDepth(src);
    if (!is_found && map[4].size() != 0)
        itentify({CTName::Bomb, CTName::Four_tt});
    else if (!is_found && map[3].size() != 0)
        itentify({CTName::Tri, CTName::Tri_t, CTName::Str_Tri, CTName::Str_Tri_t});
    else if (!is_found && map[2].size() != 0)
        itentify({CTName::Pair, CTName::Str_Pair});
    else if (!is_found && map[1].size() != 0)
        itentify({CTName::Single, CTName::Rocket, CTName::Str_Single});
    return ret;
}

std::vector<NumVec> CardTypeHelper::foundTheSameCTName(const NumVec& src, const CTName& name, int straight, int tail_type, int tail_size) {
    std::vector<NumVec> stem_group;
    NumMap r_map = CardTypeHelper::splitByRange(src);
    NumMap d_map = CardTypeHelper::splitByDepth(src);
    bool need_deal_tail = false;
    int need_deal_straight = 0;
    
    switch (name) {
        case CTName::Undef: break;
            
        case CTName::Single: {
            for (int num : d_map[1]) stem_group.push_back(NumVec(1, num));
            for (int num : r_map[2]) stem_group.push_back(NumVec(1, num));
        }
            break;
            
        case CTName::Pair: {
            for (int num : d_map[2]) stem_group.push_back(NumVec(2, num));
            for (int num : r_map[2]) stem_group.push_back(NumVec(2, num));
        }
            break;
            
        case CTName::Tri: {
            for (int num : d_map[3]) stem_group.push_back(NumVec(3, num));
            for (int num : r_map[4]) stem_group.push_back(NumVec(4, num));
        }
            break;
        
        case CTName::Tri_t: {
            for (int num : d_map[3]) stem_group.push_back(NumVec(3, num));
            for (int num : r_map[4]) stem_group.push_back(NumVec(4, num));
            need_deal_tail = true;
        }
            break;
        
        case CTName::Four_tt: {
            for (int num : d_map[4]) stem_group.push_back(NumVec(4, num));
            need_deal_tail = true;
        }
            break;
            
        case CTName::Str_Single: {
            sortByOrder(r_map[1]);
            stem_group = CardTypeHelper::splitStraight(r_map[1], straight);
        }
            break;
        
        case CTName::Str_Pair: {
            sortByOrder(r_map[2]);
            stem_group = CardTypeHelper::splitStraight(r_map[2], straight);
            need_deal_straight = 1;
        }
            break;
        
        case CTName::Str_Tri: {
            sortByOrder(r_map[3]);
            stem_group = CardTypeHelper::splitStraight(r_map[3], straight);
            need_deal_straight = 2;
        }
            break;
        
        case CTName::Str_Tri_t: {
            sortByOrder(r_map[3]);
            stem_group = CardTypeHelper::splitStraight(r_map[3], straight);
            need_deal_straight = 2;
            need_deal_tail = true;
        }
            break;
            
        case CTName::Bomb:
            for (int num : d_map[4]) stem_group.push_back(NumVec(4, num));
            break;
        
        case CTName::Rocket: {
            const NumVec& vec = d_map[1];
            if (std::find(vec.begin(), vec.end(), 16) != vec.end() &&
                std::find(vec.begin(), vec.end(), 17) != vec.end())
                stem_group.push_back(NumVec{16, 17});
        }
            break;
    }
    
    // if not found
    if (stem_group.empty())
        return {};
    
    // deal straight
    if (need_deal_straight > 0) {
        for (NumVec& vec : stem_group) {
            size_t len = vec.size();
            for (size_t i = 0; i < need_deal_straight; ++i)
                vec.insert(vec.end(), vec.begin(), vec.begin() + len);
        }
    }
    
    // if doesn't deal tail
    if (!need_deal_tail)
        return stem_group;
    
    std::vector<NumVec> ret;
    // lamad
    auto buildTailVec = [&](const NumVec& stem, const NumVec& src){
        NumVec ret;
        auto iter = src.begin();
        while (iter != src.end() && ret.size() < tail_size/tail_type) {
            if (countNumVec(stem, *iter) == 0)  // 这里意味着像 {33344434} 这类是未定义牌型
                ret.push_back(*iter);
            iter++;
        }
        return iter == src.end() ? NumVec() : ret;
    };
    
    // 带单
    if (tail_type == 1) {
        sortByOrder(r_map[1]);
        for (NumVec& stem : stem_group) {
            NumVec vec = buildTailVec(stem, r_map[1]);
            if (!vec.empty()) {
                vec.insert(vec.end(), stem.begin(), stem.end());
                ret.push_back(vec);
            }
        }
    }
    // 带对
    if (tail_type == 2) {
        sortByOrder(r_map[2]);
        for (NumVec& stem : stem_group) {
            NumVec vec = buildTailVec(stem, r_map[2]);
            if (!vec.empty()) {
                vec.insert(vec.end(), vec.begin(), vec.end());
                vec.insert(vec.end(), stem.begin(), stem.end());
                ret.push_back(vec);
            }
        }
    }
    
    // finish
    return ret;
}

std::vector<NumVec> CardTypeHelper::foundGreaterCardType(const NumVec& src, const CardType& target) {
    const CTName& name = target.getCTName();
    const CTLevel lv   = CardType::getLevel(name);
    std::vector<NumVec> ret;
    // nobody can beat "Rocket"
    if (lv == CTLevel::L5)
        return {};
    
    if (lv == CTLevel::L3) {
        std::vector<NumVec> vec_1 = foundTheSameCTName(src, CTName::Bomb);
        for (NumVec vec : vec_1) {
            sortByOrder(vec);
            CardType type = CardTypeHelper::identifyCardType(vec);
            if (CardType::compare(type, target) == CTCmpRes::Greater)
                ret.push_back(vec);
        }
        
        std::vector<NumVec> vec_2 = foundTheSameCTName(src, CTName::Rocket);
        if (!vec_2.empty())
            ret.push_back(vec_2[0]);
    }
    
    if (lv == CTLevel::L1) {
        std::vector<NumVec> vec_1 = foundTheSameCTName(src, name, target.getStrLength(), target.getTailType(), target.getTailVec().size());
        for (NumVec vec : vec_1) {
            sortByOrder(vec);
            CardType type = CardTypeHelper::identifyCardType(vec);
            if (CardType::compare(type, target) == CTCmpRes::Greater)
                ret.push_back(vec);
        }
        
        std::vector<NumVec> vec_2 = foundTheSameCTName(src, CTName::Bomb);
        for (NumVec vec : vec_2) ret.push_back(vec);
        
        std::vector<NumVec> vec_3 = foundTheSameCTName(src, CTName::Rocket);
        for (NumVec vec : vec_3) ret.push_back(vec);
    }
    
    return ret;
}

std::vector<NumVec> CardTypeHelper::buildCardType(const NumVec& src) {
    // 不考虑带牌
    std::vector<NumVec> ret;
    auto d_map = CardTypeHelper::splitByDepth(src);
    auto r_map = CardTypeHelper::splitByRange(src);

    // 单张、对子、三张
    const auto& d1_vec = d_map[1];
    const auto& d2_vec = d_map[2];
    const auto& d3_vec = d_map[3];
    auto isExist = [](const int& num, const NumVec& src){
        return std::find(src.begin(), src.end(), num) != src.end();
    };
    for (auto num : r_map[1]) {
        if (isExist(num, d1_vec))      ret.push_back(NumVec(1, num));
        else if (isExist(num, d2_vec)) ret.push_back(NumVec(2, num));
        else if (isExist(num, d3_vec)) ret.push_back(NumVec(3, num));
    }

    // 炸弹
    for (auto num : r_map[4]) ret.push_back(NumVec(4, num));

    // 火箭
    if (std::find(d1_vec.begin(), d1_vec.end(), 16) != d1_vec.end() &&
        std::find(d1_vec.begin(), d1_vec.end(), 17) != d1_vec.end())
        ret.push_back({16, 17});

    return ret;
}

std::vector<NumVec> CardTypeHelper::buildCardType(const NumVec& src, const CardType& target) {
    return CardTypeHelper::foundGreaterCardType(src, target);
}
