//
//  SpriteBlur.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/11.
//

#ifndef SpriteBlur_hpp
#define SpriteBlur_hpp

#include "cocos2d.h"

class SpriteBlur : public cocos2d::Sprite
{
public:
    static SpriteBlur* create(const char* psz_file);
    static SpriteBlur* create(cocos2d::SpriteFrame* frame);
    
    bool initWithTexture(cocos2d::Texture2D* texture, const cocos2d::Rect& rect) override;
    
    void initGLProgram();
    
    void setBlurRadius(float radius);
    void setBlurSampleNum(float num);
    
protected:
    float _radius;
    float _sample_num;
};

#endif /* SpriteBlur_hpp */
