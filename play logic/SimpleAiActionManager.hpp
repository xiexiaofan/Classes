//
//  SimpleAiActionManager.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/3/4.
//

#ifndef SimpleAiActionManager_hpp
#define SimpleAiActionManager_hpp

#include "cocos2d.h"
#include "UICard.hpp"
#include "CardTypeHelper.hpp"
#include "CardTypeDefine.hpp"

// 底牌种类定义
enum class RestCTName 
{
    DoubleKing,   // 双王
    Three,        // 三张 
    Flush,        // 同花
    Straight,     // 顺子
    SingleKing,   // 单王
    Pair,         // 对子
    Common        // 常规 
};

class SimpleAiActionManager : public cocos2d::Node
{
public:
    using CardVec = std::vector<UICard*>;

    // identify rest_card
    static RestCTName identifyRestCard(const CardVec& vec);
    static int getRestCTNameMutiple(const RestCTName& name);

    CREATE_FUNC(SimpleAiActionManager);
    void setPreId(int id) { _pre_id = id; }
    
    // ai action
    bool doActionCall(int id);
    bool doActionRob(int id);
    bool doActionBet(int id);
    bool doActionDouble(int id);
    CardType doActionPlay(int id);
    CardType doActionPlay(CardVec& vec, int id);
    
    // ai data
    void initNumMap(const std::map<int, CardVec>& card_map);
    void addNumData(const CardVec& vec, int id);
    void rmNumData(const CardVec& vec, int id);
    void clearAllNumData();
    
    // helper function
    CardVec findCardVec(const CardVec& src, const CardType& ct);
    void sortCardVec(CardVec& vec, const CardType& ct);
    CardType identityCardType(const CardVec& vec);
    
protected:
    int _pre_id;
    CardType _pre_type;
    
    std::map<int, NumVec>  _num_map;
};

#endif /* SimpleAiActionManager_hpp */
