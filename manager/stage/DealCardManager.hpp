//
//  DealCardManager.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#ifndef DealCardManager_hpp
#define DealCardManager_hpp

#include "GameManager.hpp"
#include "CardAlignConfig.hpp"

class UICard;
class DealCardManager : public cocos2d::Node
{
public:
    enum ShuffType {
        RANDOM
    };
    CREATE_FUNC(DealCardManager);
    
    bool isBindManager() const             { return _manager; }
    void bindManager(GameManager* manager) { _manager = manager; }
    
    void startDeal();
    
    std::vector<int> createCardHeap(ShuffType type);
    std::vector<int> createCardHeap(const std::vector<int>& vec);
    
    void initDealConfig();
    
protected:
    void endDeal();
    
    std::vector<int> shuffleByRandom() const;
    
    // 发牌逻辑实现
    void update(float dt) override;
    
    // 发牌动画实现
    void runDealAnimation(UICard* card, float dur, const cocos2d::Vec2& origin, const Vec2& dest,
                          float begin_scale, float end_scale, float zorder, bool flipped);
    
protected:
    GameManager* _manager;
    
    int _total_num;        // 牌堆中纸牌的总数量
    int _deal_num;         // 待发出纸牌的总数量
    int _player_num;       // 等待发牌的玩家数量
    int _cur_deal_num;     // 当前已发纸牌的数量
    int _first_target_id;  // 第一张牌发给座位X
    float _timer_count;    // 发牌计时
    
    // 发牌位置参数
    std::vector<std::vector<SingleCardConfig>> _card_info_vec;
};

#endif /* DealCardManager_hpp */
