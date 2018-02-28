//
//  RoomEntryLayer.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#ifndef RoomEntryLayer_hpp
#define RoomEntryLayer_hpp

#include "cocos2d.h"

class SingleRoomCreator;

class RoomEntryLayer : public cocos2d::Layer {
public:
    enum class Type {
        CLASSIC,
        FAKE
    };
    enum class Mode {
        NODEAL,
        DEAL
    };
    static RoomEntryLayer* create(cocos2d::Scene* scene);
    
    virtual ~RoomEntryLayer();
    
    Type getType() const    { return _type; }
    void setType(Type type) { _type = type; }
    Mode getMode() const    { return _mode; }
    void setMode(Mode mode) { _mode = mode; }
    bool isScorlling() const       { return _is_scrolling; }
    void setScorlling(bool scroll) { _is_scrolling = scroll; }
private:
    bool initWithScene(cocos2d::Scene* scene);
    void onEnter() override;
    void onExit()  override;
    
    // 触摸事件监听器
    void addlistenerForExitBtn();
    void addListenerForClassicBtn();
    void addListenerForFakeBtn();
    void addListenerForNodealBtn();
    void addListenerForDealBtn();
    void addListenerForQStartBtn();
    void addListenerForScrollRoom();
    
    // 按钮UI变化 选中时点亮or取消选中时熄灭
    void lightenTypeBtn(Type type);
    void extinctTypeBtn(Type type);
    void lightenModeBtn(Mode mode);
    void extinctModeBtn(Mode mode);
    
    void runAnimationForScorllRoom(int dir);
    int  getRoomIdForTouchPoint(cocos2d::Touch* touch) const;
private:
    cocos2d::Scene* _scene;
    
    Type _type;
    Mode _mode;
    
    friend class SingleRoomCreator;
    cocos2d::ClippingNode* _room_layer;
    cocos2d::Node* _room_container;
    cocos2d::DrawNode* _stencil;
    cocos2d::Vector<SingleRoomCreator*> _room_vec;
    
    bool _is_scrolling;
    int  _selected_room_id;
    
    CC_SYNTHESIZE_READONLY(cocos2d::Node*, _classic_btn, ClassicBtn);
    CC_SYNTHESIZE_READONLY(cocos2d::Node*, _fake_btn, FakeBtn);
    CC_SYNTHESIZE_READONLY(cocos2d::Node*, _deal_btn, DealBtn);
    CC_SYNTHESIZE_READONLY(cocos2d::Node*, _nodeal_btn, NodealBtn);
    CC_SYNTHESIZE_READONLY(cocos2d::Sprite*, _qstart_btn, QStartBtn);
    
    std::vector<cocos2d::EventListener*> _listener_vec;
};

class SingleRoomCreator : public cocos2d::Node {
public:
    static SingleRoomCreator* create(RoomEntryLayer* scene, int id);
    
    void updateModeLogo();
    
    void runAnimationForScrolling(int dir);
    void runAnimationForPressed(float zoom);
    void callbackForEnterRoom();
    
    cocos2d::Sprite* getRoomSprite() { return _bg; }
private:
    bool init(RoomEntryLayer* scene, int id);
    
    void updateRoomDataDisplay();
private:
    RoomEntryLayer* _scene;
    cocos2d::Sprite* _bg;
    cocos2d::Sprite* _mode_logo;
    cocos2d::Label* _ante_num;
    cocos2d::Label* _gold_range_num;
    
    bool _is_scrolling;
    int  _room_id;
};

#endif /* RoomEntryLayer_hpp */
