//
//  SpriteBlur.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/11.
//

#include "SpriteBlur.hpp"
using namespace cocos2d;

SpriteBlur* SpriteBlur::create(const char *pszFileName) {
    SpriteBlur* pRet = new (std::nothrow) SpriteBlur();
    if (pRet) {
        bool result = pRet->initWithFile("");
        log("Test call Sprite::initWithFile with bad file name result is : %s", result ? "true" : "false");
    }
    
    if (pRet && pRet->initWithFile(pszFileName)) {
        pRet->autorelease();
    } else {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}

SpriteBlur* SpriteBlur::create(SpriteFrame* frame) {
    SpriteBlur* pRet = new (std::nothrow) SpriteBlur();
    
    if (pRet && pRet->initWithTexture(frame->getTexture(), frame->getRect())) {
        pRet->autorelease();
    } else {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}

bool SpriteBlur::initWithTexture(Texture2D* texture, const Rect& rect) {
    if( Sprite::initWithTexture(texture, rect) ) {
#if CC_ENABLE_CACHE_TEXTURE_DATA
        auto listener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [this](EventCustom* event){
            setGLProgram(nullptr);
            initGLProgram();
        });
        
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
#endif
        
        initGLProgram();
        
        return true;
    }
    
    return false;
}

void SpriteBlur::initGLProgram() {
    std::string fragSource = FileUtils::getInstance()->getStringFromFile(
                                                                         FileUtils::getInstance()->fullPathForFilename("example_Blur.fsh"));
    auto program = GLProgram::createWithByteArrays(ccPositionTextureColor_noMVP_vert, fragSource.data());
    
    auto glProgramState = GLProgramState::getOrCreateWithGLProgram(program);
    setGLProgramState(glProgramState);
    
    auto size = getTexture()->getContentSizeInPixels();
    getGLProgramState()->setUniformVec2("resolution", size);
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WINRT)
    getGLProgramState()->setUniformFloat("blurRadius", _radius);
    getGLProgramState()->setUniformFloat("sampleNum", _sample_num);
#endif
}

void SpriteBlur::setBlurRadius(float radius) {
    _radius = radius;
    getGLProgramState()->setUniformFloat("blurRadius", _radius);
}

void SpriteBlur::setBlurSampleNum(float num) {
    _sample_num = num;
    getGLProgramState()->setUniformFloat("sampleNum", _sample_num);
}
