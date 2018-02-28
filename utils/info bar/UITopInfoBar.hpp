//
//  UITopInfoBar.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#ifndef UITopInfoBar_hpp
#define UITopInfoBar_hpp

#include "cocos2d.h"

class UITopInfoBar : public cocos2d::Node
{
public:
    CREATE_FUNC(UITopInfoBar);
    
    void displayForGameNotBegin();
    void displayForGameBegin();
    
    void updateAnte();
    void updatemultiple();
    
    void displayClock();
    void displayRestCard();
    
private:
    bool init() override;
    void update(float dt) override;
    
    std::string adjustFormatForClockStr() const;
    
    void runAnimationFromClockToRestCard();
    void runAnimationFromRestCardToClock();
private:
    cocos2d::Label* _ante_num;
    cocos2d::Label* _multiple_num;
    cocos2d::Label* _clock;
    bool _is_gaming;
    bool _is_display_clock;
    cocos2d::EventListenerTouchOneByOne* _listener;
};

#endif /* UITopInfoBar_hpp */
