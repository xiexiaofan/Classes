//
//  RoomDebugLayer.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/26.
//

#ifndef RoomDebugLayer_hpp
#define RoomDebugLayer_hpp

#include "cocos2d.h"
#include "ui/UIButton.h"

class GameManager;
class RoomDebugLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(RoomDebugLayer);
    
    void bindManager(GameManager* manager) { _manager = manager; }
    
    bool init() override;
    
    // callback
    void callbackForReDeal(cocos2d::Ref*);
    void callbackForRePick(cocos2d::Ref*);
    void callbackForUpBtn(cocos2d::Ref*);
    void callbackForDownBtn(cocos2d::Ref*);
    
    void updateCode(const std::string& code);
    
private:
    cocos2d::ui::Button* createLazyBtn(const std::string& name,
                                       const std::function<void(cocos2d::Ref*)>& callback);
    
private:
    cocos2d::ui::Button* _redeal;
    cocos2d::ui::Button* _repick;
    cocos2d::ui::Button* _up_btn;
    cocos2d::ui::Button* _down_btn;
    
    std::string _code;    // { call, bet, rob, play }
    GameManager* _manager;
};

#endif /* RoomDebugLayer_hpp */
