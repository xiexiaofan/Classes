//
//  RoomCardManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#include "RoomCardManager.hpp"
#include "RoomData.hpp"
#include "CommonMethodSet.hpp"
using namespace cocos2d;

static int   Start_Touch_Card_Index = -1;
static Vec2  CardBirthPos(569, 365);

RoomCardManager::~RoomCardManager() {
    RoomDataManager::getInstance()->rmAllCardData();
    RoomDataManager::getInstance()->rmAllPlayerData();
    clearAllCard();
}

void RoomCardManager::createCard(const std::vector<int>& vec) {
    if (!_total_card_vec.empty()) {
        RoomDataManager::getInstance()->rmAllCardData();
        clearAllCard();
    }
    // 创建 待发牌堆
    auto iter_rest = vec.begin() + RoomDataManager::getInstance()->getDealCardNum();
    std::for_each(vec.begin(), iter_rest, [this](const int& id){
        UICard* card = UICard::create(id);
        card->setVisible(false);
        this->addChild(card);
        _total_card_vec.push_back(card);
    });
    // 创建 底牌
    std::for_each(iter_rest, vec.end(), [this](const int& id){
        UICard* card = UICard::create(id);
        card->setModel(UICard::Model::RESTCARD);
        this->addChild(card);
        const static float x = Director::getInstance()->getVisibleSize().width * 0.5;
        const static float y = Director::getInstance()->getVisibleSize().height - card->getSizeAfterZoom().height * 0.5;
        static int i = 0;
        float px = (i++ % 3 - 1) * (card->getSizeAfterZoom().width + 6.0);
        card->setPosition(Vec2(px + x, y));
        _total_card_vec.push_back(card);
        _rest_card_vec.push_back(card);
    });
}

void RoomCardManager::clearAllCard() {
    auto iter = _total_card_vec.begin();
    while (iter != _total_card_vec.end()) {
        UICard* card = *iter;
        iter = _total_card_vec.erase(iter);
        card->removeFromParent();
    }
    
    for (auto& vec : _ming_card) {
        auto iter = vec.begin();
        while (iter != vec.end()) {
            UICard* card = *iter;
            iter = vec.erase(iter);
            card->removeFromParent();
        }
    }
    _ming_card.clear();
    _inside_card.clear();
    _outside_card.clear();
    _selected_card.clear();
    _rest_card_vec.clear();
}

void RoomCardManager::callbackForDeal() {
    auto pFun = [&](int index){
        auto& vec = _inside_card.at(index);
        // 排序
        std::sort(vec.begin(), vec.end(), [](const UICard* c1, const UICard* c2){
            return c1->getCardData()->getId() > c2->getCardData()->getId();
        });
        // 挪位
        const auto& inside_config = _inside_align_config.at(index);
        for (int i = 0; i < vec.size(); ++i) {
            UICard* card = vec.at(i);
            SingleCardConfig config = getSingleCardConfig(i, vec.size(), inside_config);
            MoveTo* mt_1 = MoveTo::create(0.15f, inside_config.align_point);
            CallFunc* call = CallFunc::create([card, i]{card->setLocalZOrder(i);});
            MoveTo* mt_2 = MoveTo::create(0.15f, config.point);
            card->runAction(Sequence::create(DelayTime::create(0.4f), mt_1, DelayTime::create(0.08f), call, mt_2, nullptr));
        }
    };
    for (int i = 0; i < _inside_card.size(); ++i)
        pFun(i);
    
    // debug
    auto createCard = [&](const CardVec& vec, const CardAlignConfig& align){
        CardVec ming;
        for (int i = 0; i < vec.size(); ++i) {
            UICard* card = UICard::create(vec.at(i)->getCardData());
            card->setModel(UICard::Model::TABLECARD);
            card->setFaceTowardUp(true);
            card->setVisible(_debug_ming);
            this->addChild(card);
            SingleCardConfig config = getSingleCardConfig(i, vec.size(), align);
            card->setPosition(config.point);
            card->setScale(config.scale);
            card->setLocalZOrder(config.zorder);
            ming.push_back(card);
        }
        _ming_card.push_back(ming);
    };
    createCard(_inside_card.at(1), _ming_align_config.at(0));
    createCard(_inside_card.at(2), _ming_align_config.at(1));
}

void RoomCardManager::callbackForPushRestCard(int landlord) {
    for (UICard* card : _rest_card_vec) {
        UICard* c2 = UICard::create(card->getCardData());
        c2->setModel(UICard::Model::RESTCARD);
        c2->setFaceTowardUp(true);
        c2->setPosition(card->getPosition());
        _total_card_vec.push_back(c2);
        this->addChild(c2);
        pushCardToInside(card, landlord);
    }
    resetSelected(landlord);
    // 位置更新
    auto& card_vec = _inside_card.at(landlord);
    std::sort(card_vec.begin(), card_vec.end(), [](const UICard* c1, const UICard* c2){
        return c1->getCardData()->getId() > c2->getCardData()->getId();
    });
    int size = static_cast<int>(card_vec.size());
    for (int i = 0; i < size; ++i) {
        UICard* card = card_vec.at(i);
        const auto& info = getSingleCardConfig(i, size, _inside_align_config.at(landlord));
        card->setLocalZOrder(info.zorder);
        card->setScale(info.scale);
        if (std::find(_rest_card_vec.begin(), _rest_card_vec.end(), card) == _rest_card_vec.end())
            card->runAction(MoveTo::create(0.2f, Vec2(info.point.x, card->getPositionY())));
        else
            card->setPosition(info.point);
    }
    if (landlord == 0) {
        // 底牌插入动画
        for (auto card : _rest_card_vec) {
            card->setModel(UICard::Model::HANDCARD);
            float delta_y = card->getSizeAfterZoom().height * 0.3;
            card->setPositionY(card->getPositionY() + delta_y);
            card->runAction(Sequence::create(DelayTime::create(0.7f), MoveBy::create(0.3f, Vec2(0, -delta_y)),  nullptr));
            card->setFaceTowardUp(true);
        }
    } else {
        auto& ming_vec = _ming_card.at(landlord-1);
        for (auto card : _rest_card_vec) {
            UICard* m_card = UICard::create(card->getCardData());
            m_card->setModel(UICard::Model::TABLECARD);
            m_card->setFaceTowardUp(true);
            m_card->setVisible(_debug_ming);
            ming_vec.push_back(m_card);
            this->addChild(m_card);
        }
        std::sort(ming_vec.begin(), ming_vec.end(), [](const UICard* c1, const UICard* c2){
            return c1->getCardData()->getId() > c2->getCardData()->getId();
        });
        const auto& align = _ming_align_config.at(landlord-1);
        for (auto i = 0; i < ming_vec.size(); ++i) {
            UICard* card = ming_vec.at(i);
            SingleCardConfig config = getSingleCardConfig(i, ming_vec.size(), align);
            card->setPosition(config.point);
            card->setScale(config.scale);
            card->setLocalZOrder(config.zorder);
        }
    }
}

bool RoomCardManager::callbackForPopCard(int index) {
    // judge
    auto& selected = _selected_card[index];
    if (selected.empty())
        return false;

    pushCardToOutside(index);
    updateCardDisplayForInside(index);
    updateCardDisplayForOutside(index);
    return true;
}

void RoomCardManager::pushCardToInside(UICard* card, int index) {
//    RoomDataManager::getInstance()->addCardData(card->getCardData(), index);
    _inside_card[index].push_back(card);
    updateCardCountLabel(index);
}

void RoomCardManager::refCardToSelected(UICard* card, int index) {
    auto& selected_vec = _selected_card[index];
    if (std::find(selected_vec.begin(), selected_vec.end(), card) != selected_vec.end())
        return;
    selected_vec.push_back(card);
    // TODD
    if (index == 0 && !card->isSelected()) {
        card->runAction(MoveBy::create(0.1f, Vec2(0, card->getSizeAfterZoom().height*0.15)));
    }
    card->switchSelected();
}

void RoomCardManager::derefCardFromSelected(UICard* card, int index) {
    auto& selected_vec = _selected_card[index];
    auto iter = std::find(selected_vec.begin(), selected_vec.end(), card);
    if (iter == selected_vec.end())
        return;
    selected_vec.erase(iter);
    // TODD
    if (index == 0 && card->isSelected()) {
        card->runAction(MoveBy::create(0.1f, Vec2(0, -card->getSizeAfterZoom().height*0.15)));
    }
    card->switchSelected();
}

void RoomCardManager::resetSelected(int index) {
    auto& selected_vec = _selected_card[index];
    auto iter = selected_vec.begin();
    while (iter != selected_vec.end())
        derefCardFromSelected(*iter, index);
}

void RoomCardManager::pushCardToOutside(int index) {
    clearCardFromOutside(index);
    /* data process order:
     * 清空上一次出的牌
     * 清理 RoomData
     * 从select区复制到out区
     * 清空 select区
     */
    auto& inside_vec = _inside_card[index];
    auto& selected_vec = _selected_card[index];
    auto& outside_vec = _outside_card[index];
/* 弃用
    for (UICard* card : selected_vec) {
        RoomDataManager::getInstance()->rmCardData(card->getCardData(), index);
        auto iter = std::find(inside_vec.begin(), inside_vec.end(), card);
        if (iter != inside_vec.end())
            inside_vec.erase(iter);
    }
    updateCardCountLabel(index);
    std::swap(outside_vec, selected_vec);
*/
    for (UICard* card : selected_vec)
        outside_vec.push_back(card);
    
    resetSelected(index);
    
    for (UICard* card : outside_vec) {
//        RoomDataManager::getInstance()->rmCardData(card->getCardData(), index);
        auto iter = std::find(inside_vec.begin(), inside_vec.end(), card);
        if (iter != inside_vec.end())
            inside_vec.erase(iter);
    }
    updateCardCountLabel(index);
}

void RoomCardManager::clearCardFromOutside(int index) {
    auto& vec = _outside_card[index];
    auto iter = vec.begin();
    while (iter != vec.end()) {
        UICard* card = *iter;
        auto t_iter = std::find(_total_card_vec.begin(), _total_card_vec.end(), card);
        if (t_iter != _total_card_vec.end())
            _total_card_vec.erase(t_iter);
        iter = vec.erase(iter);
        card->removeFromParent();
    }
}

void RoomCardManager::openListenerForTouchCard() {
    if (_listener_touch_card) {
        _listener_touch_card->setEnabled(true);
        return;
    }
    _listener_touch_card = EventListenerTouchOneByOne::create();
    _listener_touch_card->setSwallowTouches(true);
    _listener_touch_card->onTouchBegan = [this](Touch* touch, Event*){
        int i = this->getTouchCardIndex(touch);
        if (i == -1)
            return false;
        Start_Touch_Card_Index = i;
        this->updateMaskLayerForTouchCard(i, i);
        return true;
    };
    _listener_touch_card->onTouchMoved = [this](Touch* touch, Event*){
        int i = this->getTouchCardIndex(touch);
        if (i != -1)
            this->updateMaskLayerForTouchCard(Start_Touch_Card_Index, i);
    };
    _listener_touch_card->onTouchEnded = [this](Touch* touch, Event*){
        for (UICard* card : _inside_card[0]) {
            if (!card->hasMask())
                continue;
            card->rmMask();
            card->isSelected() ? this->derefCardFromSelected(card, 0) : this->refCardToSelected(card, 0);
            log("selected card size = %lu", _selected_card.at(0).size());
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener_touch_card, this);
}

void RoomCardManager::stopListenerForTouchCard() {
    if (_listener_touch_card)
        _listener_touch_card->setEnabled(false);
}

void RoomCardManager::setDebugMing(bool open) {
    _debug_ming = open;
    for (auto vec : _ming_card)
        for (auto card : vec)
            card->setVisible(_debug_ming);
    _debug_ming ? openDebugListener() : stopDebugListener();
}

void RoomCardManager::openDebugListener() {
    auto getMingCardTouchIndex = [&](Touch* touch, 
                                     CardVec& vec,
                                     const CardAlignConfig& config){
        if (vec.empty() == 0)
            return -1;
        const auto& card_size = vec[0]->getSizeAfterZoom();
        // y3 > y2 > y1 > y0
        const float first_pos = vec[0]->getLeftBottomPosition();
        const float y0 = first_pos.y - card_size.width() - config.gap_y;
        const float y1 = first_pos.y;
        const float y2 = first_pos.y - config.gap_y;
        const float y3 = first_pos.y + card_size.width();
        auto touch_in_node = this->convertTouchToNodeSpace(touch);
        if (touch_in_node.y < y0 || touch_in_node.y > y3)
            return -1;

        bool double_rows = vec.size() > config.align_limit;
        if (touch_in_node.y < y1 && !double_rows)
            return -1;

        int col = MIN((touch_in_node.x - first_pos.x) / config.gap_x, 
                      double_rows ? config.align_limit : vec.size() - 1);
        if (touch_in_node.x < first_pos.x || 
            touch_in_node.x > col*config.gap_x + card.size().width)
            return -1;
        
        if (config.align_type == -1) {
            if (touch_in_node.y < y1 && double_rows)
                return vec.size() > col+config.align_limit ? col+config.align_limit : -1;
            if (touch_in_node.y < y2 && !double_rows)
                return col;
            if (touch_in_node.y < y2 && double_rows)
                return vec.size() > col+config.align_limit ? col+config.align_limit : col;
            return col;  // touch_in_node.y < y3
        } else if (config.align_type == 1) {
            if (touch_in_node.y < y1 && double_rows)
                return vec.size() >= config.align_limit*2 - col ? col+config.align_limit : -1;
            if (touch_in_node.y < y2 && !double_rows)
                return col;
            if (touch_in_node.y < y2 && double_rows)
                return vec.size() >= config.align_limit*2 - col ? col+config.align_limit : col;
            return col;
        }
        return -1;
    };

    if (_debug_touch_card.empty()) {
        auto l1 = EventListenerTouchOneByOne::create();
        auto l2 = EventListenerTouchOneByOne::create();
            l1->onTouchBegan = [&](Touch* touch, Event*){
            int index = getMingCardTouchIndex(touch, _ming_card[0], _ming_align_config[0]);
            if (index != -1) {
                UICard* card = _ming_card[1][index];
                card->switchSelected();
                card->isSelected() ? card->addMask() : card->rmMask();
            }
            return false;
        };
        l2->onTouchBegan = [&](Touch* touch, Event*){
            int index = getMingCardTouchIndex(touch, _ming_card[1], _ming_align_config[1]);
            if (index != -1) {
                UICard* card = _ming_card[1][index];
                card->switchSelected();
                card->isSelected() ? card->addMask() : card->rmMask();
            }
            return false;
        };
        _eventDispatcher->addEventListenerWithFixedPriority(l1, 1);
        _eventDispatcher->addEventListenerWithFixedPriority(l2, 1);
        _debug_touch_card.push_back(l1);
        _debug_touch_card.push_back(l2);
    } else {
        for (auto listener : _debug_touch_card)
            listener->setEnabled(true);
    }
}

void RoomCardManager::stopDebugListener() {
    for (auto listener : _debug_touch_card)
        listener->setEnabled(false);
}

bool RoomCardManager::init() {
    initCardAlignConfig();
    initCardCountLabel();
    
    return true;
}

void RoomCardManager::initCardAlignConfig() {
    // 座位0，1，2的手牌位置参数
    CardAlignConfig zero_inside(Vec2(595, 86),   0, 99,  50, 0, 1.00);
    CardAlignConfig one_inside( Vec2(960, 385),  1, 99,  0,  0, 0.40);
    CardAlignConfig two_inside( Vec2(178, 385), -1, 99,  0,  0, 0.40);
    _inside_align_config.push_back(zero_inside);
    _inside_align_config.push_back(one_inside);
    _inside_align_config.push_back(two_inside);

    // 座位0，1，2的出牌位置参数
    CardAlignConfig zero_outside(Vec2(589, 275),  0, 99, 36, 0, 0.60);
    CardAlignConfig one_outside (Vec2(850, 353),  1, 99, 25, 0, 0.50);
    CardAlignConfig two_outside (Vec2(217, 353), -1, 99, 25, 0, 0.50);
    _outside_align_config.push_back(zero_outside);
    _outside_align_config.push_back(one_outside);
    _outside_align_config.push_back(two_outside);
    
    // 座位1，2的调试显示手牌位置参数
    CardAlignConfig one_ming(Vec2(960, 550),  1, 10, -25, -45, 0.40);
    CardAlignConfig two_ming(Vec2(178, 550), -1, 10,  25, -45, 0.40);
    _ming_align_config.push_back(one_ming);
    _ming_align_config.push_back(two_ming);
}

void RoomCardManager::initCardCountLabel() {
    for (int i = 0; i < _inside_align_config.size(); ++i) {
        auto label = Label::createWithBMFont("fonts/fonts_yellow_num.fnt", "");
        label->setPosition(_inside_align_config.at(i).align_point + Vec2(0, 5));
        label->setScale(0.82);
        label->setLocalZOrder(RoomDataManager::getInstance()->getTotalCardNum() + 1);
        this->addChild(label);
        _inside_card_count.push_back(label);
    }
}

void RoomCardManager::updateCardCountLabel(int index) {
    if (index == 0)
        return;
    ssize_t val = _inside_card[index].size();
    std::string count = val == 0 ? "" : std::to_string(val);
    auto& label = _inside_card_count.at(index);
    label->setString(count);
}

void RoomCardManager::updateCardDisplayForInside(int index) {
    auto& vec = _inside_card.at(index);
    int size = vec.size();
    for (int i = 0; i < size; ++i) {
        UICard* card = vec.at(i);
        SingleCardConfig info = getSingleCardConfig(i, size, _inside_align_config[index]);
        card->runAction(MoveTo::create(0.1f, info.point));
    }
}

void RoomCardManager::updateCardDisplayForOutside(int index) {
    auto& vec = _outside_card[index];
    int size = vec.size();
    if (index == 0) {
        for (int i = 0; i < size; ++i) {
            UICard* card = vec.at(i);
            const SingleCardConfig& config = getSingleCardConfig(i, size, _outside_align_config[index]);
            MoveBy* mb = MoveBy::create(0.12f, Vec2(0, card->getSizeAfterZoom().height * 0.4));
            CallFunc* fade_out = CallFunc::create([card]{
                card->getFaceUp()->runAction(FadeOut::create(0.12f));
                for (auto& child : card->getFaceUp()->getChildren()) child->runAction(FadeOut::create(0.12f));
            });
            Spawn* sp = Spawn::create(ScaleTo::create(0.12f, config.scale), MoveTo::create(0.12f, config.point), nullptr);
            CallFunc* call = CallFunc::create([card, config]{
                card->setLocalZOrder(config.zorder);
                card->setModel(UICard::Model::TABLECARD);
            });
            CallFunc* fade_in = CallFunc::create([card]{
                card->getFaceUp()->runAction(FadeIn::create(0.12f));
                for (auto& child : card->getFaceUp()->getChildren()) child->runAction(FadeIn::create(0.12f));
            });
            card->runAction(Sequence::create(fade_out, mb, call, sp, fade_in, nullptr));
        }
    } else {
        for (int i = 0; i < size; ++i) {
            UICard* card = vec.at(i);
            card->setModel(UICard::Model::TABLECARD);
            card->setFaceTowardUp(true);
            const SingleCardConfig& config = getSingleCardConfig(i, size, _outside_align_config[index]);
            card->setLocalZOrder(config.zorder);
            card->setScale(config.scale);
            card->runAction(MoveTo::create(0.15f, config.point));
        }
    }
}

int RoomCardManager::getTouchCardIndex(Touch* touch) const {
    const auto& vec = _inside_card.at(0);
    if (vec.empty())
        return -1;
    
    Vec2 first_card_pos = vec.front()->getLeftBottomPosition();
    Size card_size = vec.front()->getSizeAfterZoom();
    Vec2 touch_in_card = this->convertTouchToNodeSpace(touch);
    
    if (touch_in_card.x < first_card_pos.x ||
        touch_in_card.x > first_card_pos.x +
        (vec.size() - 1) * _inside_align_config.at(0).gap_x +
        card_size.width)
        return -1;
    
    int ret = MIN((touch_in_card.x - first_card_pos.x) / _inside_align_config.at(0).gap_x,
                  static_cast<int>(vec.size() - 1));
    Vec2 target_pos = vec.at(ret)->getLeftBottomPosition();
    if (touch_in_card.y < target_pos.y ||
        touch_in_card.y > target_pos.y + card_size.height)
        return -1;
    
    return ret;
}

void RoomCardManager::updateMaskLayerForTouchCard(int first, int last) {
    if (first > last)
        std::swap(first, last);
    auto& vec = _inside_card[0];
    for (int i = 0; i != vec.size(); ++i)
        i >= first && i <= last ? vec.at(i)->addMask() : vec.at(i)->rmMask();
}
