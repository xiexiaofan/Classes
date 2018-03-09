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

//std::vector<NumVec> CardTypeHelper::foundGreaterCardType(const NumVec& src, const CardType& target) {
//
//}
//
//std::vector<NumVec> CardTypeHelper::foundTheSameCardType(const NumVec& src, const CardType& target) {
//    const CTName& name = target.getCTName();
//
//    switch (name) {
//        case CTName::Undef: break;
//
//        case CTName::Single:
//            for (auto num : d_map[1]) stem_group.push_back(num);
//            for (auto num : r_map[2]) stem_group.push_back(num);
//            break;
//
//        case CTName::Pair:
//            for (auto num : d_map[2]) stem_group.push_back(NumVec(2, num));
//            for (auto num : r_map[3]) stem_group.push_back(NumVec(2, num));
//            break;
//
//        case CTName::Tri:
//            for (auto num : d_map[3]) stem_group.push_back(NumVec(3, num));
//            for (auto num : r_map[4]) stem_group.push_back(NumVec(3, num));
//            break;
//
//        case CTName::Tri_t:
//            for (auto num : d_map[3]) stem_group.push_back(NumVec(3, num));
//            for (auto num : r_map[4]) stem_group.push_back(NumVec(3, num));
//            need_tail_size = 1;
//            break;
//
//        case CTName::Four_tt:
//            for (auto num : d_map[4]) stem_group.push_back(NumVec(4, num));
//            need_tail_size = target.getTailVec().size();
//            break;
//
//        case CTName::Str_Single:
//            std::sort(r_map[1].begin(), r_map[1].end());
//            stem_group = CardTypeHelper::splitStraight(r_map[1], target.getStrLength());
//            break;
//
//        case CTName::Str_Pair: {
//            std::sort(r_map[2].begin(), r_map[2].end());
//            stem_group = CardTypeHelper::splitStraight(r_map[2], target.getStrLength());
//            need_add_times = 1;
//        }
//            break;
//
//        case CTName::Str_Tri: {
//            std::sort(r_map[3].begin(), r_map[3].end());
//            stem_group = CardTypeHelper::splitStraight(r_map[3], target.getStrLength());
//            need_add_times = 2;
//        }
//            break;
//
//        case CTName::Str_Tri_t: {
//            std::sort(r_map[3].begin(), r_map[3].end());
//            stem_group = CardTypeHelper::splitStraight(r_map[3], target.getStrLength());
//            need_add_times = 2;
//            need_tail_size = target.getTailVec().size();
//        }
//            break;
//
//        case CTName::Bomb:
//            for (auto num : d_map[4]) stem_group.push_back(NumVec(4, num));
//            break;
//
//        case CTName::Rocket: {
//            const auto& vec = r_map[1];
//            if (std::find(vec.begin(), vec.end(), 16) != vec.end() &&
//                std::find(vec.begin(), vec.end(), 17) != vec.end())
//                stem_group.push_back({16, 17});
//        }
//            break;
//    }
//}
//
//
//
//
//
//std::vector<NumVec> CardTypeHelper::foundGreaterCardType(const NumVec& src, const CardType& target) {
//    std::vector<CardType> ret_type;
//    CTLevel lv = CardType::getLevel(target.getCTName());
//    if (lv == CTLevel::L5)
//        return {};  // noboay can beat Rocket, unless you.
//
//    if (lv == CTLevel::L3) {
//        auto bomb_vec = CardTypeHelper::foundSameCardType(src, CTName::Bomb);
//        for (auto& type : bomb_vec)
//            if (CardType::compare(type, target) == CTCmpRes::Greater)
//                           ret_type.push_back(type);
//
//        auto rocket = CardTypeHelper::foundSameCardType(src, CTName::Rocket);
//        for (auto& type : rocket) ret_type.push_back(type);
//    } else if (lv == CTLevel::L1) {
//        auto equal_vec = CardTypeHelper::foundSameCardType(src, target.getCTName());
//        for (auto& type : equal_vec)
//            if (CardType::compare(type, target) == CTCmpRes::Greater)
//                ret_type.push_back(type);
//
//        auto bomb_vec = CardTypeHelper::foundSameCardType(src, CTName::Bomb);
//        for (auto& type : bomb_vec) ret_type.push_back(type);
//
//        auto rocket = CardTypeHelper::foundSameCardType(src, CTName::Rocket);
//        for (auto& type : rocket) ret_type.push_back(type);
//    }
//    std::vector<NumVec> ret;
//    for (auto type : ret_type) {
//        ret.insert(ret.end(), *type.getStemVec.begin(), *type.getStemVec.end());
//        ret.insert(ret.end(), *type.getTailVec.begin(), *type.getTailVec.end());
//    }
//    return ret;
//}

//std::vector<CardType> CardTypeHelper::foundSameCardType(const NumVec& src, const CardType& target) {
//    std::vector<NumVec> stem_group;
//    NumMap r_map = CardTypeHelper::splitByRange(src);
//    NumMap d_map = CardTypeHelper::splitByDepth(src);
//    int  need_tail_size = 0;
//    int  need_add_times = 0;
//    switch (target.getCTName()) {
//        case CTName::Undef: break;
//
//        case CTName::Single:
//            for (auto num : d_map[1]) stem_group.push_back(num);
//            for (auto num : r_map[2]) stem_group.push_back(num);
//            break;
//
//        case CTName::Pair:
//            for (auto num : d_map[2]) stem_group.push_back(NumVec(2, num));
//            for (auto num : r_map[3]) stem_group.push_back(NumVec(2, num));
//            break;
//
//        case CTName::Tri:
//            for (auto num : d_map[3]) stem_group.push_back(NumVec(3, num));
//            for (auto num : r_map[4]) stem_group.push_back(NumVec(3, num));
//            break;
//
//        case CTName::Tri_t:
//            for (auto num : d_map[3]) stem_group.push_back(NumVec(3, num));
//            for (auto num : r_map[4]) stem_group.push_back(NumVec(3, num));
//            need_tail_size = 1;
//            break;
//
//        case CTName::Four_tt:
//            for (auto num : d_map[4]) stem_group.push_back(NumVec(4, num));
//            need_tail_size = target.getTailVec().size();
//            break;
//
//        case CTName::Str_Single:
//            std::sort(r_map[1].begin(), r_map[1].end());
//            stem_group = CardTypeHelper::splitStraight(r_map[1], target.getStrLength());
//            break;
//
//        case CTName::Str_Pair: {
//            std::sort(r_map[2].begin(), r_map[2].end());
//            stem_group = CardTypeHelper::splitStraight(r_map[2], target.getStrLength());
//            need_add_times = 1;
//        }
//            break;
//
//        case CTName::Str_Tri: {
//            std::sort(r_map[3].begin(), r_map[3].end());
//            stem_group = CardTypeHelper::splitStraight(r_map[3], target.getStrLength());
//            need_add_times = 2;
//        }
//            break;
//
//        case CTName::Str_Tri_t: {
//            std::sort(r_map[3].begin(), r_map[3].end());
//            stem_group = CardTypeHelper::splitStraight(r_map[3], target.getStrLength());
//            need_add_times = 2;
//            need_tail_size = target.getTailVec().size();
//        }
//            break;
//
//        case CTName::Bomb:
//            for (auto num : d_map[4]) stem_group.push_back(NumVec(4, num));
//            break;
//
//        case CTName::Rocket: {
//            const auto& vec = r_map[1];
//            if (std::find(vec.begin(), vec.end(), 16) != vec.end() &&
//                std::find(vec.begin(), vec.end(), 17) != vec.end())
//                stem_group.push_back({16, 17});
//        }
//            break;
//    }
//
//    if (need_add_times != 0) {
//        for (auto& NumVec : stem_group) {
//            for (int i = 0; i < nee_add_times; ++i)
//                NumVec.insert(NumVec.end(), NumVec.begin(), NumVec.end());
//            std::sort(NumVec.begin(), NumVec.end());
//        }
//    }
//
//    std::vector<CardType> ret;
//    return ret;
//}


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
        if (isExist(num, d1_vec))      ret.push_back(NumVec({num}));
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
 //   return CardTypeHelper::foundGreaterCardType(src, target);
    std::vector<NumVec> ret;
    return ret;
}
