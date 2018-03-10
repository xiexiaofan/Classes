//
//  SimpleAiActionManager.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/3/4.
//

#ifndef SimpleAiActionManager_hpp
#define SimpleAiActionManager_hpp

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

class SimpleAiActionManager
{
public:
    using NumVec  = std::vector<int>;
    using CardVec = std::vector<UICard*>;

public:
    static SimpleAiActionManager* getInstance();
    void destroyInstance();

    // identify rest_card
    RestCTName identifyRestCard(const CardVec& vec);
    int getRestCardMutiple(const RestCTName& name);
    
    // ai action
    bool doActionCall(int id);
    bool doActionRob(int id);
    int  doActionBet(int id);
    bool doActionDouble(int id);
    CardType doActionPlay(int id);
    CardType doActionPlay(CardVec& vec, int id);

    const std::vector<NumVec>& getPlaySeqVec(int id);
    NumVec getPlayNumVec(int id);
    
    void setPreId(int id) { _pre_id = id; }

    // _num_map operate
    void initNumMap(const std::map<int, CardVec>& card_map);
    void addNumData(const CardVec& vec, int id);
    void rmNumData(const CardVec& vec, int id);
    void clearAllNumData();
    
    // helper function
    void sortCardVec(CardVec& vec, const CardType& ct);
    CardType identityCardType(const CardVec& vec);

    void buildPlaySeqVec(int id);
    void initPlaySeqVec();

protected:
    int _pre_id;
    CardType _pre_type;
    std::map<int, NumVec> _num_map;
    std::vector<NumVec> _play_seq_vec;
    size_t _cur_index;
};

#endif /* SimpleAiActionManager_hpp */
