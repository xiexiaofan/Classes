//
//  DebugLayer.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#ifndef DebugLayer_hpp
#define DebugLayer_hpp

#include "cocos2d.h"

class DebugLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(DebugLayer);
    
    bool init() override;
};

#endif /* DebugLayer_hpp */
