//
//  UIOptTimer.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#ifndef UIOptTimer_hpp
#define UIOptTimer_hpp

#include "cocos2d.h"

class UIOptTimer : public cocos2d::Node
{
    typedef std::function<void(UIOptTimer*)> callbackForEnd;
public:
    /**
     * 创建一个倒计时组件，用于展示牌桌出牌倒计时
     * @param image: 背景图路径
     * @param duration: 倒计时间，单位秒
     * @param callback: 倒计时结束时的回调事件
     */
    static UIOptTimer* create(const std::string& image, float dur, const callbackForEnd& callback);
    
    /**
     * 设置 是否开始倒计时标志
     * @note： 默认值是 false
     */
    bool isStartCountDown() const        { return _enabled; }
    void setStartCountDown(bool enabled) { _enabled = enabled; }
    
    /**
     * 自定义倒计时时间，单位:秒
     */
    void setDuration(float dur) { _dur = dur; }
    
    /**
     * 自定义倒计时结束时的回调事件
     */
    void setCallbackForEnd(const callbackForEnd& callback) { _callback = callback; }
    
protected:
    UIOptTimer();
    virtual ~UIOptTimer();
    
    virtual bool init(const std::string& image, float dur, const callbackForEnd& callback);
    
    virtual void update(float dt) override;
    
protected:
    bool _enabled;
    float _dur;
    callbackForEnd _callback;
    cocos2d::ProgressTimer* _timer;
    cocos2d::Sprite* _move_star;
};
#endif /* UIOptTimer_hpp */
