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

/*
ai出牌
playcardmanger 向 aimanager请求 当前可出牌队列
（随机本轮出牌耗时）
playcardmanager 将 出牌队列“激活”的牌 同步给 roomcardmanager 执行选中牌操作
playcardmanager 执行 出牌操作

ai跟牌
	Playcardmanager 向 aimanager 请求 当前可出牌队列
		若返回空 playcardmanager执行“要不起”操作
（随机本轮出牌耗时）
playcardmanager 将 出牌队列“激活”的牌 同步给 roomcardmanager 执行选中牌操作
playcardmanager 执行 出牌操作

提示
   Playcardmanager 向 aimanager 请求 当前可出牌队列中 激活牌组
   Playcardmanager 将 牌组 同步给 roomcardmanager 执行选中牌操作
   激活牌组 标识+1（指向下一个待激活牌组）
*/

class SimpleAiActionManager
{
public:
    using NumVec  = std::vector<int>;
    using CardVec = std::vector<UICard*>;

public:
    static SimpleAiActionManager* getInstance();
    static void destroyInstance();

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

    std::vector<NumVec>& getPlaySeqVec(int id);
    NumVec& getPlayNumVec(int id);
    
    void setPreId(int id) { _pre_id = id; }

    
    // _num_map operate
    void initNumMap(const std::map<int, CardVec>& card_map);
    void addNumData(const CardVec& vec, int id);
    void rmNumData(const CardVec& vec, int id);
    void clearAllNumData();
    
    // helper function
    void sortCardVec(CardVec& vec, const CardType& ct);
    CardType identityCardType(const CardVec& vec);

private:
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
