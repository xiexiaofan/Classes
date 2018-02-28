//
//  LoadingScene.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#ifndef LoadingScene_hpp
#define LoadingScene_hpp

#include "cocos2d.h"

class LoadingScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(LoadingScene);
    
    virtual ~LoadingScene();
    
    bool init()    override;
    void onEnter() override;
};

#endif /* LoadingScene_hpp */
