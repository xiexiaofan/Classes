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

std::vector<CardType> CardTypeHelper::foundCardTypeByName(const NumVec& src, const CTName& target) {
    std::vector<NumVec> stem_group;
    std::vector<NumVec> tail_group;
    NumMap r_map = CardTypeHelper::splitByRange(src);
    NumMap d_map = CardTypeHelper::splitByDepth(src);
    bool need_deal_tail = false;
    switch (target) {
        case CTName::Undef: break;
            
        case CTName::Single:
            for (auto& i : r_map[1]) stem_group.push_back({i});
            break;
            
        case CTName::Pair:
            for (auto& i : r_map[2]) stem_group.push_back({i, i});
            break;
            
        case CTName::Tri:
            for (auto& i : r_map[3]) stem_group.push_back({i, i, i});
            break;
            
        case CTName::Tri_t: {
            for (auto& i : r_map[3]) stem_group.push_back({i, i, i});
            need_deal_tail = true;
        }
            break;
            
        case CTName::Four_tt: {
            for (auto& i : r_map[4]) stem_group.push_back({i, i, i, i});
            // 两单
            for (auto iter = d_map[1].begin(); iter != d_map[1].end(); ++iter)
                for (auto iter2 = iter+1; iter2 != d_map[1].end(); ++iter2)
                    tail_group.push_back({*iter, *iter2});
            // 一对
            for (auto iter = d_map[2].begin(); iter != d_map[2].end(); ++iter)
                    tail_group.push_back({*iter, *iter});
            // 两对
            for (auto iter = d_map[2].begin(); iter != d_map[2].end(); ++iter)
                for (auto iter2 = iter+1; iter2 != d_map[2].end(); ++iter2)
                    tail_group.push_back({*iter, *iter, *iter2, *iter2});
            need_deal_tail = true;
        }
            break;
            
        case CTName::Str_Single:
            stem_group = CardTypeHelper::splitStraight(r_map[1], 5);
            break;
            
        case CTName::Str_Pair:
            stem_group = CardTypeHelper::splitStraight(r_map[2], 3);
            break;
            
        case CTName::Str_Tri:
            stem_group = CardTypeHelper::splitStraight(r_map[3], 2);
            break;
            
        case CTName::Str_Tri_t: {
            stem_group = CardTypeHelper::splitStraight(r_map[3], 2);
            need_deal_tail = true;
        }
            break;
            
        case CTName::Bomb:
            for (auto& i : r_map[4]) stem_group.push_back({i, i, i, i});
            break;
            
        case CTName::Rocket: {
            const auto& vec = r_map[1];
            if (std::find(vec.begin(), vec.end(), 16) != vec.end() &&
                std::find(vec.begin(), vec.end(), 17) != vec.end())
                stem_group.push_back({16, 17});
        }
            break;
    }
    
    std::vector<CardType> ret;
    return ret;
}

std::vector<CardType> CardTypeHelper::foundGreaterCardType(const NumVec& src, const CardType& target) {
    std::vector<CardType> ret;
    CTLevel lv = CardType::getLevel(target.getCTName());
   if (lv == CTLevel::L5)
       return {};  // noboay can beat Rocket, unless you.

   if (lv == CTLevel::L3) {
       auto rocket = CardTypeHelper::foundCardType(vec, CTName::Rocket);
       for (auto& type : rocket) ret.push_back(type);

       auto bomb_vec = CardTypeHelper::foundCardType(vec, CTName::Bomb);
       for (auto& type : bomb_vec)
           if (CardType::compare(type, target) == CTCmpRes::Greater)
               ret.push_back(type);
   } else if (lv == CTLevel::L1) {
       auto rocket = CardTypeHelper::foundCardType(vec, CTName::Rocket);
       for (auto& type : rocket) ret.push_back(type);

       auto bomb_vec = CardTypeHelper::foundCardType(vec, CTName::Bomb);
       for (auto& type : bomb_vec) ret.push_back(type);

       auto equal_vec = CardTypeHelper::foundCardType(vec, target.getCTName());
       for (auto& type : equal_vec)
           if (CardType::compare(type, target) == CTCmpRes::Greater)
               ret.push_back(type);
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
        if (isExist(num, d1_vec))      ret.push_back(NumVec(num));
        else if (isExist(num, d2_vec)) ret.push_back(Numvec(2, num)); 
        else if (isExist(num, d3_vec)) ret.push_back(NumVec(3, num))
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
    std::vector<NumVec> ret;

    // 考虑接 单张？对子？三张？三带一？顺子？连对？飞机？飞机带翅膀？四带二？炸弹？火箭？


    return ret;
}