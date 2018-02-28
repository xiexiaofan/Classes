//
//  SimpleToastManager.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/9.
//

#ifndef SimpleToastManager_hpp
#define SimpleToastManager_hpp

#include "cocos2d.h"

class SingleToastCreator;
class SimpleToastManager : public cocos2d::Ref
{
public:
    static SimpleToastManager* getInstance();
    static void destroyInstance();
    
    cocos2d::Node* getBindCarrierNode() const { return _carrier; }
    void bindCarrierNode(cocos2d::Node* node);
    
    void playToast(const std::string& content);
    void playToast(const std::string& content, float dur, float delay);
    
    ssize_t getCurToastCount() const { return _active_toast_vec.size(); }
    
private:
    SimpleToastManager();
    
    void update(float dt);
    
private:
    bool _need_toast;
    cocos2d::Node* _carrier;
    cocos2d::Vector<SingleToastCreator*> _active_toast_vec;
    cocos2d::Vector<SingleToastCreator*> _delay_toast_vec;
};


class SingleToastCreator : public cocos2d::Node
{
public:
    /**
     * 创建一条toast对象
     * @param content: 显示的内容
     * @param dur: 持续显示的时间
     * @param delay: 延迟显示的时间
     */
    static SingleToastCreator* create(const std::string& content, float dur, float delay);
    
    // animation
    virtual void runAnimationForEnter();
    virtual void runAnimationForRise();
    virtual void runAnimationForDisappear();
    
protected:
    virtual bool init(const std::string& content, float dur, float delay);
    
    CC_SYNTHESIZE(float, _dur, Duration);
    CC_SYNTHESIZE(float, _delay, DelayCount);
};


#endif /* SimpleToastManager_hpp */
