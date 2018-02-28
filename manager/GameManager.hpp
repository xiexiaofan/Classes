//
//  GameManager.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#ifndef GameManager_hpp
#define GameManager_hpp

#include "cocos2d.h"

class RoomScene;
class RoomDataManager;
class RoomCardManager;
class DealCardManager;
class SelectManager;
class PlayCardManager;
class SettleManager;
class RoomDebugLayer;

class GameManager : public cocos2d::Node
{
public:
    enum class StageCode {
        FREE,
        MATCH,
        DEAL,
        SELECT,
        PLAY,
        SETTLE
    };
    CREATE_FUNC(GameManager);
    
    // associated scene
    bool isBindScene() const         { return _scene != nullptr; }
    void bindScene(RoomScene* scene) { _scene = scene; }
    
    // stage code
    StageCode getCurStageCode() const  { return _stage; }
    void setStageCode(StageCode stage) { _stage = stage; }
    
    RoomCardManager* getCardManager()  { return _card_manager; }
    
    void run();
    
    // taget id for run
    int  getTargetId() const { return _target_id; }
    int  plusTargetId(int delta, bool only_ret = false);
    int  minusTargetId(int delta, bool only_ret = false);
    void resetTargetId(int val = -1);
    
    // listener
    void openListenerForTouchCard();
    void stopListenerForTouchCard();
    
    void openListenerForSelectEvent();
    void stopListenerForSelectEvent();
    void clearListenerForSelectEvent();
    
    void openListenerForPlayCardEvent();
    void stopListenerForPlayCardEvent();
    void clearListenerForPlayCardEvent();
    
protected:
    GameManager() = default;
    virtual ~GameManager();
    
    bool init() override;
    
    void runStageFree();
    void runStageMatch();
    void runStageDeal();
    void runStageSelect();
    void runStagePlay();
    void runStageSettle();
    
    void displayMatchInfo(float dt);
    
    void createDebugSwitch();
    void callbackForDebugSwitch(bool open);
    
protected:
    RoomScene* _scene;
    RoomDataManager* _data;
    StageCode _stage;
    
    // seat_id, used for method "run()"
    int _target_id;
    
    // stage manager
    DealCardManager* _deal_manager;
    SelectManager* _select_manager;
    PlayCardManager* _play_manager;
    SettleManager* _settle_manager;
    
    // card manager
    RoomCardManager* _card_manager;
    
    // listener
    cocos2d::Vector<cocos2d::EventListenerCustom*> _listener_select_vec;
    cocos2d::Vector<cocos2d::EventListenerCustom*> _listener_play_vec;
    
    // friend class
    friend class DealCardManager;
    friend class SelectManager;
    friend class PlayCardManager;
    friend class SettleManager;
    friend class RoomDebugLayer;
    
    // debug
    bool _debug;
    RoomDebugLayer* _debug_layer;
};

#endif /* GameManager_hpp */
