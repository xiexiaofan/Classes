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

class SimpleAiActionManager : public cocos2d::Node
{
public:
    using CardVec = std::vector<UICard*>;
    
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
