//
//  SelectManager.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#ifndef SelectManager_hpp
#define SelectManager_hpp

#include "GameManager.hpp"
#include <forward_list>

class RoomDataManager;
class RoomDebugLayer;
class SelectManager : public cocos2d::Node
{
public:
    enum class OptionCode {
        CALL,
        BET,
        ROB,
        DOUBLE
    };
    CREATE_FUNC(SelectManager);
    
    bool isBindManager()                   { return _manager != nullptr; }
    void bindManager(GameManager* manager) { _manager = manager; }
    
    void startSelect();
    
    void updateSelectConfig();
    
    // panel事件回调函数
    void callbackForCall(cocos2d::Event*);
    void callbackForNocall(cocos2d::Event*);
    
    void callbackForBetOne(cocos2d::Event*);
    void callbackForBetTwo(cocos2d::Event*);
    void callbackForBetThr(cocos2d::Event*);
    
    void callbackForRob(cocos2d::Event*);
    void callbackForNorob(cocos2d::Event*);
    
    void callbackForDouble(cocos2d::Event*);
    void callbackForNodouble(cocos2d::Event*);
    
    //
    void debugJudge();
private:
    bool init() override;
    
    void endSelect();
    
    void runForCodeCall();
    void runForCodeBet();
    void runForCodeRob();
    void runForCodeDouble();
    
    int  confirmLandlordId() const;
    void callbackForConfirmLandlord(int landlord_id);
    
private:
    GameManager* _manager;
    RoomDataManager* _data;
    
    std::forward_list<OptionCode> _opt_list;
    std::forward_list<OptionCode>::iterator _cur_opt;
    
    int _player_count;      // 玩家人数
    int _need_rob_count;    // 需要抢地主的次数（在确定地主前）
    int _cur_call_count;    // 当前已进行叫地主（或者叫分）流程的次数
    int _cur_rob_count;     // 当前已进行抢地主流程的次数
    int _cur_double_count;  // 当前已进行加倍流程的次数
    int _cur_bet_val;       // 当前最高的叫分值
    int _first_call_id;     // 第一个叫地主玩家的座位id
    int _last_rob_id;       // 最后一个抢地主玩家的座位id
    int _last_bet_id;       // 最后一个叫分玩家的座位id
    
    // friend
    friend class RoomDebugLayer;
};


#endif /* SelectManager_hpp */
