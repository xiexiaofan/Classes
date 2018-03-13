//
//  UIOptPanel.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/15.
//

#include "UIOptPanel.hpp"
#include "PlayerData.hpp"
#include "UIOptTimer.hpp"
#include "UIOptButton.hpp"
#include "CommonDefineSet.hpp"
using namespace cocos2d;

// 倒计时器 配置: { 操作盘种类, {初始倒计时长, 倒计时结束时触发事件名}}
static std::map<UIOptPanel::Type, std::pair<float, std::string>> Timer_Config_Ref {
    {UIOptPanel::Type::NONE,       {}},
    {UIOptPanel::Type::CALL,       {5.0f, _xxf::en_nocall}},
    {UIOptPanel::Type::BET,        {5.0f, _xxf::en_nocall}},
    {UIOptPanel::Type::ROB,        {5.0f, _xxf::en_norob}},
    {UIOptPanel::Type::DOUBLE,     {5.0f, _xxf::en_nodouble}},
    {UIOptPanel::Type::MING,       {20.0f, _xxf::en_trust}},
    {UIOptPanel::Type::PASS,       {5.0f, _xxf::en_pass}},
    {UIOptPanel::Type::PLAY_LIMIT, {15.0f, _xxf::en_trust}},
    {UIOptPanel::Type::PLAY,       {15.0f, _xxf::en_trust}}
};

// 按钮 配置: { 操作盘种类， 按钮点击触发事件名 }
static std::map<UIOptPanel::Type, std::vector<std::string>> Button_Config_Ref {
    {UIOptPanel::Type::NONE, {}},
    {UIOptPanel::Type::CALL, {
        "bj","green",_xxf::en_nocall, "jdz","yellow",_xxf::en_call
    }},
    {UIOptPanel::Type::BET, {
        "bj","green",_xxf::en_nocall, "1f","yellow",_xxf::en_bet_one, "2f","yellow",_xxf::en_bet_two, "3f","yellow",_xxf::en_bet_thr
    }},
    {UIOptPanel::Type::ROB, {
        "bq","green",_xxf::en_norob, "qdz","yellow",_xxf::en_rob
    }},
    {UIOptPanel::Type::DOUBLE, {
        "bjb","green",_xxf::en_nodouble, "jb","yellow",_xxf::en_double
    }},
    {UIOptPanel::Type::MING, {
        "mp","yellow",_xxf::en_ming, "ts","green",_xxf::en_tip, "cp","yellow",_xxf::en_play
    }},
    {UIOptPanel::Type::PASS, {
        "ybq","green",_xxf::en_pass
    }},
    {UIOptPanel::Type::PLAY_LIMIT, {
        "bc","green",_xxf::en_pass, "ts","green",_xxf::en_tip, "cp","yellow",_xxf::en_play
    }},
    {UIOptPanel::Type::PLAY, {
        "ts","green",_xxf::en_tip, "cp","yellow",_xxf::en_play
    }}
};

/**
 * OptPanel position 三人牌桌选项盘内UI相对坐标值
 * 外层vec下标表示“btn_vec”对应一排按钮的总数量
 * 内层vec下标表示“btn_vec”对应一排按钮中的第N个按钮（从左往右）
 */
const static std::vector<std::vector<Vec2>> optpanel_pos_vec {
    {Vec2(569, 240), Vec2(888, 390), Vec2(254, 390)},  // _timer的坐标
    {Vec2(624, 240), Vec2(468, 240)},
    {Vec2(417, 240), Vec2(721, 240), Vec2(569, 240)},
    {Vec2(307, 240), Vec2(611, 240), Vec2(818, 240), Vec2(459, 240)},
    {Vec2(249, 240), Vec2(511, 240), Vec2(687, 240), Vec2(863, 240), Vec2(380, 240)}
};

UIOptPanel* UIOptPanel::create(UIOptPanel::Type type, PlayerData* pdata) {
    UIOptPanel* ret = new UIOptPanel();
    if (ret && ret->init(type, pdata))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

UIOptPanel::UIOptPanel()
: _type(Type::NONE)
, _pdata(nullptr)
, _timer(nullptr)
{}

UIOptPanel::~UIOptPanel()
{}

void UIOptPanel::dispatchOptEvent(const std::string& event_name) {
    CallFunc* call_1 = CallFunc::create([&]{
        EventCustom event(event_name);
        event.setUserData(_pdata);
        _eventDispatcher->dispatchEvent(&event);
    });
    CallFunc* call_2 = isNeedRemoveAfterTouchEnded(event_name) ? CallFunc::create([this]{this->removeFromParent();}) : nullptr;
    this->runAction(Sequence::create(call_1, call_2, nullptr));
}

void UIOptPanel::setTimerRest(float rest) {
    if (_timer)
        _timer->setDuration(rest > 0 ? rest : 0);
}

void UIOptPanel::setBtnEnabled(bool enabled, int index) {
    if (_btn_vec.size() > index)
        _btn_vec.at(index)->setEnabled(enabled);
}

void UIOptPanel::setRemoveIndexAfterTouchEnded(const std::vector<int>& index) {
    _remove_index_vec = index;
}

bool UIOptPanel::init(UIOptPanel::Type type, PlayerData* pdata) {
    _type = type;
    _pdata = pdata;
    
    if (_type != Type::CUSTOM) {
        if (_pdata && _pdata->getSeatId() == 0)
            initButtonAccordingType(_type);
        initTimerAccordingType(_type);
        
        adjustPosition();
    }
    
    _remove_index_vec = { -1 };
    
    return true;
};

void UIOptPanel::onEnter() {
    Node::onEnter();
    if (_timer)
        _timer->setStartCountDown(true);
}

void UIOptPanel::initTimerAccordingType(Type type) {
    if (Timer_Config_Ref.find(type) == Timer_Config_Ref.end())
        return;
    const auto& pair = Timer_Config_Ref.at(type);
    _timer = UIOptTimer::create(_pdata->getPlayerConfig().image, pair.first,
                                [&](UIOptTimer*){this->dispatchOptEvent(pair.second);});
    this->addChild(_timer);
    _timer->setScale(0.95);
}

void UIOptPanel::initButtonAccordingType(Type type) {
    if (Button_Config_Ref.find(type) == Button_Config_Ref.end())
        return;
    const auto& vec = Button_Config_Ref.at(type);
    CC_ASSERT(vec.size() % 3 == 0);
    for (ssize_t i = 0; i < vec.size(); i += 3) {
        UIOptButton* btn = UIOptButton::create(vec.at(i), vec.at(i+1),
                                               [&, i](UIOptButton*){this->dispatchOptEvent(vec.at(i+2));});
        _btn_vec.pushBack(btn);
    }
}

void UIOptPanel::adjustPosition() {
    ssize_t btn_size = _btn_vec.size();
    const std::vector<Vec2>& opt_pos_vec = optpanel_pos_vec.at(btn_size);
    for (ssize_t i = 0; i < btn_size; ++i) {
        _btn_vec.at(i)->setPosition(opt_pos_vec.at(i));
        // 一排四个按钮时需要单独的缩放比例
        _btn_vec.at(i)->setUserScale(btn_size == 4 ? Vec2(0.77, 0.77) : Vec2(0.85, 0.85));
        this->addChild(_btn_vec.at(i));
    }
    _timer->setPosition(opt_pos_vec.at(btn_size == 0 ? _pdata->getSeatId() : btn_size));
}

bool UIOptPanel::isNeedRemoveAfterTouchEnded(const std::string& event_name) const {
    // TODD
    if (event_name == _xxf::en_tip ||
        event_name == _xxf::en_ming ||
        event_name == _xxf::en_play)
        return false;
    
    return true;
}
