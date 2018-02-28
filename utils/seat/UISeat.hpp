//
//  UISeat.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#ifndef UISeat_hpp
#define UISeat_hpp

#include "cocos2d.h"

class PlayerData;
class UISeat : public cocos2d::Node
{
public:
    CREATE_FUNC(UISeat);
    
    void loadPlayerData(PlayerData* pdata);
    void unloadPlayerData();
    
    void updateDisplayForImage();
    void updateDisplayForName();
    void updateDisplayForGold();
    
    void displayerIdLogo(bool landlord, bool left);
    void rmplayerIdLogo();
    
private:
    bool init() override;
    
    void updateDisplay();
    
    void addListenerForClicked();
    
private:
    PlayerData* _pdata;
    cocos2d::Sprite* _image;
    cocos2d::Label*  _name;
    cocos2d::Label*  _gold;
    cocos2d::EventListenerTouchOneByOne* _listener;
    cocos2d::Sprite* _id_logo;
};

#endif /* UISeat_hpp */
