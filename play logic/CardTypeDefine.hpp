//
//  CardTypeDefine.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#ifndef CardTypeDefine_hpp
#define CardTypeDefine_hpp

#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

// 牌型种类定义
enum class CTName
{
    Undef = -1,
    Single,
    Pair,
    Tri,
    Tri_t,
    Four_tt,
    Str_Single,
    Str_Pair,
    Str_Tri,
    Str_Tri_t,
    Bomb,
    Rocket
};

// 牌型级别定义
enum class CTLevel
{
    L0 = -1, // 未定义牌型
    L1,      
    L2,      // 癞子炸弹
    L3,      // 炸弹
    L4,      // 纯癞子炸弹
    L5       // 王炸(火箭)
};

// 牌型比较结果定义
enum class CTCmpRes
{
    Undef_name,    // 无法比较——牌型名称不一致
    Undef_str,     // 无法比较——顺子长度不一致
    Undef_tail,    // 无法比较——带牌方式不一致
    Greater,       // 大于
    Less_equal     // 不大于
};

// using & define
using NumVec  = std::vector<int>;
using NumMap  = std::unordered_map<int, NumVec>;
using NumPair = std::pair<NumVec, NumVec>;
#define sortByOrder(__NUMVEC__) \
            std::sort(__NUMVEC__.begin(), __NUMVEC__.end())
#define sortByReverse(__NUMVEC__) \
            std::sort(__NUMVEC__.begin(), __NUMVEC__.end(), std::greater<int>())
#define countNumVec(__NUMVEC__, __VAL__) \
            std::count(__NUMVEC__.begin(), __NUMVEC__.end(), __VAL__)


class CardType
{
public:
    // level
    static CTLevel getLevel(const CTName& name);
    
    // compare
    static CTCmpRes compare(const CardType& left, const CardType& right);
    
    // judge
    static bool judge(const CTName& name, const NumVec& vec);
    
    // split
    static NumPair split(const CTName& name, const NumVec& vec);
    
    // build
    static CardType build(const CTName& name, const NumVec& vec);
    static CardType build(const CTName& name, const NumVec& stem, const NumVec& tail);
    static CardType build(const CTName& name, const NumPair& pair);
    
public:
    std::string getDescribe() const;
    
    // 起始点数
    int getCmpNumber() const;
    // 顺子长度
    size_t getStrLength() const;
    // 带牌类型: 0表示不带，1表示带单，2表示带对
    size_t getTailType() const;
    
    const CTName& getCTName()  const { return _name; }
    const NumVec& getStemVec() const { return _stem_vec; }
    const NumVec& getTailVec() const { return _tail_vec; }
    
    // cons.
    CardType();
    
private:
    CardType(const CTName& name, const NumPair& pair);
    
private:
    CTName _name;      // 牌型名称
    NumVec _stem_vec;  // 主干点数序列
    NumVec _tail_vec;  // 带牌点数序列
};

#endif /* CardTypeDefine_hpp */
