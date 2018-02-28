//
//  PlayCardManager.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#ifndef PlayCardManager_hpp
#define PlayCardManager_hpp

#include "GameManager.hpp"

class RoomDataManager;
class RoomCardManager;
class UIOptPanel;
class RoomDebugLayer;
class PlayCardManager : public cocos2d::Node
{
public:
    enum class PlayCode {
        Firstly,    // 地主首轮出牌
        Initiative, // 主动出牌
        Passive     // 被动出牌(牌桌有其他玩家的牌，需要接)
    };
    
    CREATE_FUNC(PlayCardManager);
    
    bool isBindManager() const             { return _manager != nullptr; }
    void bindManager(GameManager* manager) { _manager = manager; }
    
    void startPlay();
    
    void updatePlayConfig();
    
    // callback for optpanel
    void callbackForMing(cocos2d::Event*);
    void callbackForTip(cocos2d::Event*);
    void callbackForPlay(cocos2d::Event*);
    void callbackForPass(cocos2d::Event*);
    void callbackForTrust(cocos2d::Event*);
    
    
private:
    bool init() override;
    
    void runCodeFirstly();
    void runCodeInitiative();
    void runCodePassive();
    
    void endPlay();
    
private:
    GameManager* _manager;
    RoomDataManager* _data_manager;
    RoomCardManager* _card_manager;
    UIOptPanel* _cur_panel;
    PlayCode _code;
    
    friend class RoomDebugLayer;
};

#endif /* PlayCardManager_hpp */
