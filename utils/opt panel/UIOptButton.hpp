//
//  UIOptButton.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#ifndef UIOptButton_hpp
#define UIOptButton_hpp

#include "cocos2d.h"

class UIOptButton : public cocos2d::Node
{
    typedef std::function<void(UIOptButton*)> callbackForClicked;
public:
    /**
     * 创建一个牌桌按钮
     * @param fn: 按钮功能名称
     * @param bg: 按钮背景图名称
     * @param call_back: 按钮成功点击时的回调事件
     */
    static UIOptButton* create(const std::string& fn,
                               const std::string& bg,
                               const callbackForClicked& callback);
    
    /**
     * 设置 按钮是否可以响应点击
     * @param enabled = true，支持点击
     * @note：默认值是true
     */
    bool isEnabled() const { return _enabled; }
    void setEnabled(bool enabled);
    
    /**
     * 设置 按钮在成功点击之后是否需要从父节点清除
     * @param need = true，需要清除
     * @note：默认值是false
     */
    bool isNeedRemoveAfterClicke() const      { return _need_remove; }
    void setNeedRemoveAfterClicked(bool need) { _need_remove = need; }
    
    /**
     * 按照用户的自定义的缩放比例向量缩放按钮
     * @note: 默认的缩放比例向量是 Vec2(1.0, 1.0)
     */
    void setUserScale(const cocos2d::Vec2& scale);
    
protected:
    UIOptButton();
    virtual ~UIOptButton();
    
    virtual bool init(const std::string& fn,
                      const std::string& bg,
                      const callbackForClicked& callback);
    
    /**
     * 按钮被按下时的动画（缩放动画）
     * @prame zoom: 缩放比例
     */
    virtual void runAnimationForPressed(float zoom);
    
    /**
     * 按钮支持点击和不支持点击时的动画（颜色改变）
     * @note: “灰色”表示此时不响应点击事件
     */
    virtual void runAnimationForTouchEnabled(bool enabled);
    
    /**
     * 触摸监听器回调
     */
    virtual bool touchBegan(cocos2d::Touch* touch, cocos2d::Event*);
    virtual void touchEnded(cocos2d::Touch* touch, cocos2d::Event*);
    
protected:
    cocos2d::Sprite* _fc;
    cocos2d::Sprite* _bg;
    callbackForClicked _callback;
    bool _enabled;
    bool _need_remove;
    cocos2d::Vec2 _user_scale;
};

#endif /* UIOptButton_hpp */
