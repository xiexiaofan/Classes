//
//  RoomScene.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#ifndef RoomScene_hpp
#define RoomScene_hpp

#include "cocos2d.h"

class UITable;
class UISeat;
class UITopInfoBar;
class UIOptPanel;
class PlayerData;
class GameManager;

class RoomScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(RoomScene);
    
    void loadPlayerData(PlayerData* pdata);
    
    void displayUIForGameNotBegin();
    void displayUIForGameBegin();
    
    void addIdLogoForPlayer(int landlord_id);
    void rmIdlogoForPlayer();
    
    // override
    bool init()    override;
    void onEnter() override;
    void onExit()  override;
    
    void addListenerForExitBtn();
private:
    GameManager* _manager;
    CC_SYNTHESIZE_READONLY(UITable*, _table, Table);
    CC_SYNTHESIZE_READONLY(UITopInfoBar*, _top_bar, TopInfoBar);
    CC_SYNTHESIZE_READONLY(std::vector<UISeat*>, _seat_vec, SeatVec);
};

#endif /* RoomScene_hpp */
