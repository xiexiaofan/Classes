//
//  FirstScene.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#ifndef FirstScene_hpp
#define FirstScene_hpp

#include "cocos2d.h"
#include "ui/UIButton.h"

class FirstScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    static cocos2d::Scene* createEntryScene();
    
    CREATE_FUNC(FirstScene);
    
    // override
    bool init()    override;
    void onEnter() override;
    void onExit()  override;
    
    void resumeFirstScene();
private:
    void initBgLayer();
    void initBtnLayer();
    void initEntryLayer();
    
    // btn callback
    void callbackForExit(cocos2d::Ref*);
    void callbackForStart(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type);
    void callbackForReview(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type);
    void callbackForControl(cocos2d::Ref*, cocos2d::ui::Widget::TouchEventType type);
    
private:
    cocos2d::Layer* _bg_layer;
    cocos2d::Layer* _btn_layer;
    cocos2d::Layer* _entry_layer;
};

#endif /* FirstScene_hpp */
