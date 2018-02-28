//
//  UIOptPanel.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#ifndef UIOptPanel_hpp
#define UIOptPanel_hpp

#include "cocos2d.h"

class PlayerData;
class UIOptTimer;
class UIOptButton;

class UIOptPanel : public cocos2d::Node
{
public:
    enum class Type {
        NONE = 0,
        CALL,        // 不叫 + 倒计时 + 叫地主
        BET,         // 不叫   + 倒计时 + 1分 + 2分 + 3分
        ROB,         // 不抢   + 倒计时 + 抢地主
        DOUBLE,      // 不加倍 + 倒计时 + 加倍
        
        MING,        // 明牌   + 倒计时 + 提示 + 出牌
        PASS,        // 倒计时 + 要不起
        PLAY_LIMIT,  // 不出   + 倒计时 + 提示 + 出牌
        PLAY,        // 提示   + 倒计时 + 出牌
        
        CUSTOM       // 自定义
    };
    
    /**
     * 创建一个带倒计时的按钮盘组件
     * @param type：按钮盘类别
     * @param pdata：按钮盘对于的玩家
     */
    static UIOptPanel* create(Type type, PlayerData* pdata);
    
    /**
     * 按钮盘的事件分发（点击按钮 或者 倒计时截止）
     @ param event_name: 事件名, 具体参考 “CommonDefSet.hpp"中定义
     */
    void dispatchOptEvent(const std::string& event_name);
    
    /**
     * 设置 倒计时器剩余时间
     */
    void setTimerRest(float rest);
    
    /**
     * 设置 特定位置的按钮是否支持点击
     */
    void setBtnEnabled(bool enabled, int index);
    
    /**
     * 设置 点击panel中对应下标按钮后，是否需要移除整个panel
     * @note：index容器为空时，表示点击任意按钮都不需要移除
     */
    void setRemoveIndexAfterTouchEnded(const std::vector<int>& index);
    
protected:
    UIOptPanel();
    virtual ~UIOptPanel();
    
    virtual bool init(Type type, PlayerData* pdata);
    
    virtual void onEnter() override;
    
    // 初始化 倒计时器
    void initTimerAccordingType(Type type);
    
    // 初始化 按钮选项
    void initButtonAccordingType(Type type);
    
    // 自动调整内部按钮位置
    virtual void adjustPosition();
    
    bool isNeedRemoveAfterTouchEnded(const std::string& event_name) const;
protected:
    Type _type;
    PlayerData* _pdata;
    UIOptTimer* _timer;
    cocos2d::Vector<UIOptButton*> _btn_vec;
    std::vector<int> _remove_index_vec;
};

#endif /* UIOptPanel_hpp */
