//
//  RoomCardManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#include "RoomCardManager.hpp"
#include "RoomData.hpp"
#include "CommonMethodSet.hpp"
#include "SimpleAiActionManager.hpp"
#include "SimpleToastManager.hpp"
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
using namespace cocos2d;

static int   Start_Touch_Card_Index = -1;
static Vec2  CardBirthPos(569, 365);
static std::map<RestCTName, std::string> rest_path = {
    {RestCTName::DoubleKing, "table_gz_sw.png"},
    {RestCTName::SingleKing, "table_gz_dw.png"},
    {RestCTName::Straight,   "table_gz_shunzi.png"},
    {RestCTName::Three,      "table_gz_sanzhang.png"},
    {RestCTName::Pair,       "table_gz_dz.png"},
    {RestCTName::Flush,      "table_gz_hs.png"}
};

RoomCardManager::~RoomCardManager() {
    RoomDataManager::getInstance()->rmAllCardData();
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
    _inside_card.clear();
    _outside_card.clear();
    _selected_card.clear();
    _rest_card_vec.clear();
    _ai_manager->clearAllNumData();
    rmRestCardType();
    
    /**used for debug. */
    clearAllDebugMing();
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
        // ai manager
        _ai_manager->addNumData(vec, index);
        
    };
    for (int i = 0; i < _inside_card.size(); ++i)
        pFun(i);
    
    /**used for debug. */
    if (_debug_ming) {
        updateDebugMingVec(1);
        updateDebugMingVec(2);
    }
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
    } else { /**used for debug. */
        updateDebugMingVec(landlord);
    }
    
    // ai manager
    _ai_manager->addNumData(_rest_card_vec, landlord);
    _ai_manager->setPreId(landlord);
    
    // identify rest card
    auto type = SimpleAiActionManager::identifyRestCard(_rest_card_vec);
    if (type != RestCTName::Common) {
        initRestCardType(rest_path[type],
                         SimpleAiActionManager::getRestCTNameMutiple(type));
    }
}

bool RoomCardManager::callbackForPopCard(int index) {
    /** 出牌流程
     * - 如果开启调试明牌：将调试明牌区引用到选牌区
     * - 判断选牌区牌组是否符合出牌要求
     *   符合
     *      - 将选择的牌 移动到 出牌区 执行出牌动作
     *   不符合
     *      - 返回错误
     */
    
    /**used for debug. */
    if (_debug_ming && index != -1)
        updateSelectedVecByDebugMing(index);
    
    // judge
    auto& selected = _selected_card[index];
    auto type = _ai_manager->doActionPlay(selected, index);
    if (type.getCTName() == CTName::Undef)
        return false;
    
    SimpleToastManager::getInstance()->playToast(type.getDescribe());
    
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
    if (index == 0 && !card->isSelected())
        card->runAction(MoveBy::create(0.1f, Vec2(0, card->getSizeAfterZoom().height*0.15)));
    card->switchSelected();
}

void RoomCardManager::refCardToSelected(const std::vector<int>& num_vec, int index) {
    auto foundX = [](const CardVec& vec, int num){
        size_t ret = 0;
        for (; ret != vec.size(); ++ret)
            if (!vec.at(ret)->isSelected() && vec.at(ret)->getCardData()->getNumber() == num)
                break;
        return ret;
    };
    
    resetSelected(index);

    auto& inside_vec = _inside_card[index];
    for (int num : num_vec) {
        size_t i = foundX(inside_vec, num);
        if (i != inside_vec.size())
            refCardToSelected(inside_vec.at(i), index);
    }

    if (_debug_ming && index != 0)
        updateDebugMingSelected(index);
}

void RoomCardManager::derefCardFromSelected(UICard* card, int index) {
    auto& selected_vec = _selected_card[index];
    auto iter = std::find(selected_vec.begin(), selected_vec.end(), card);
    if (iter == selected_vec.end())
        return;
    selected_vec.erase(iter);
    if (index == 0 && card->isSelected())
        card->runAction(MoveBy::create(0.1f, Vec2(0, -card->getSizeAfterZoom().height*0.15)));
    card->switchSelected();
}

void RoomCardManager::resetSelected(int index) {
    auto& selected_vec = _selected_card[index];
    auto iter = selected_vec.begin();
    while (iter != selected_vec.end())
        derefCardFromSelected(*iter, index);
    
    /** used for debug **/
    if (_debug_ming && index != 0) {
        auto& debug_vec = _debug_card[index];
        for (auto& card : debug_vec)
            if (card->isSelected()) {
                card->rmMask();
                card->switchSelected();
            }
    }
}

void RoomCardManager::pushCardToOutside(int index) {
    /* data process order:
     * 清空上一次出的牌
     * 清理 RoomData
     * 从select区复制到out区
     * 清空 select区
     */
    clearCardFromOutside(index);
    
    auto& inside_vec   = _inside_card[index];
    auto& selected_vec = _selected_card[index];
    auto& outside_vec  = _outside_card[index];
    for (UICard* card : selected_vec) outside_vec.push_back(card);
    
    for (UICard* card : outside_vec) {
//        RoomDataManager::getInstance()->rmCardData(card->getCardData(), index);
        auto iter = std::find(inside_vec.begin(), inside_vec.end(), card);
        if (iter != inside_vec.end()) inside_vec.erase(iter);
    }
    updateCardCountLabel(index);
    
    resetSelected(index);
    
    /**used for debug. */
    if (_debug_ming && index != -1)
        updateDebugMingVec(index);
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
        SimpleAudioEngine::getInstance()->playEffect("sound/effect/select_card.mp3");
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
//            log("selected card size = %lu", _selected_card.at(0).size());
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
    
    if (_debug_ming) {
        updateDebugMingVec(1);
        updateDebugMingVec(2);
    } else {
        clearAllDebugMing();
    }
}

void RoomCardManager::clearAllDebugMing() {
    for (auto& pair : _debug_card) {
        auto iter = pair.second.begin();
        while (iter != pair.second.end()) {
            UICard* card = *iter;
            iter = pair.second.erase(iter);
            card->removeFromParent();
        }
    }
    _debug_card.clear();
}

bool RoomCardManager::init() {
    initCardAlignConfig();
    initCardCountLabel();
    
    _ai_manager = SimpleAiActionManager::create();
    this->addChild(_ai_manager);
    
    return true;
}

void RoomCardManager::initCardAlignConfig() {
    // 座位0，1，2的手牌位置参数
    CardAlignConfig zero_inside(Vec2(595, 86),   0, 99,  50, 0, 1.00);
    CardAlignConfig one_inside( Vec2(960, 383),  1, 99,  0,  0, 0.35);
    CardAlignConfig two_inside( Vec2(178, 383), -1, 99,  0,  0, 0.35);
    _inside_align_config.push_back(zero_inside);
    _inside_align_config.push_back(one_inside);
    _inside_align_config.push_back(two_inside);

    // 座位0，1，2的出牌位置参数
    CardAlignConfig zero_outside(Vec2(589, 275),  0, 99,  36, 0, 0.60);
    CardAlignConfig one_outside (Vec2(890, 393),  1, 99, -30, 0, 0.50);
    CardAlignConfig two_outside (Vec2(248, 393), -1, 99,  30, 0, 0.50);
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
        label->setPosition(_inside_align_config.at(i).align_point + Vec2(0, 4));
        label->setScale(0.75);
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
    int size = static_cast<int>(vec.size());
    for (int i = 0; i < size; ++i) {
        UICard* card = vec.at(i);
        SingleCardConfig info = getSingleCardConfig(i, size, _inside_align_config[index]);
        card->runAction(MoveTo::create(0.1f, info.point));
    }
}

void RoomCardManager::updateCardDisplayForOutside(int index) {
    auto& vec = _outside_card[index];
    int size = static_cast<int>(vec.size());
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
    SimpleAudioEngine::getInstance()->playEffect("sound/effect/play_card.mp3");
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

void RoomCardManager::adjustCardAlign(UICard* card, int idx, int size, const CardAlignConfig& align) {
    SingleCardConfig config = getSingleCardConfig(idx, size, align);
    card->setScale(config.scale);
    card->setPosition(config.point);
    card->setLocalZOrder(config.zorder);
}

void RoomCardManager::updateDebugMingVec(int index) {
    if (!_debug_ming || index == 0)
        return;

    const auto& inside_vec = _inside_card[index];
    auto& debug_vec = _debug_card[index];
    if (inside_vec.size() == debug_vec.size())
        return;
    
    // remove from debug_vec
    auto d_iter = debug_vec.begin();
    while (inside_vec.size() < debug_vec.size() && d_iter != debug_vec.end()) {
        int id = (*d_iter)->getCardData()->getId();
        bool need_remove = true;
        for (auto& card : inside_vec) {
            int cmp = card->getCardData()->getId();
            if (cmp > id)  continue;
            if (cmp == id) need_remove = false;
            break;
        }
        if (need_remove) {
            UICard* card = *d_iter;
            d_iter = debug_vec.erase(d_iter);
            card->removeFromParent();
        }
        else 
            ++d_iter;
    }

    // add to debug_vec
    auto i_iter = inside_vec.begin();
    while (inside_vec.size() > debug_vec.size() && i_iter != inside_vec.end()) {
        int id = (*i_iter)->getCardData()->getId();
        bool need_add = true;
        auto d_iter = debug_vec.begin();
        for ( ; d_iter != debug_vec.end(); ++d_iter) {
            int cmpId = (*d_iter)->getCardData()->getId();
            if (cmpId > id)  continue;
            if (cmpId == id) need_add = false;
            break;
        }
        if (need_add) {
            UICard* card = UICard::create(id);
            card->setModel(UICard::Model::TABLECARD);
            card->setFaceTowardUp(true);
            debug_vec.insert(d_iter, card);
            card->openTouchListenerForDebug();
            this->addChild(card);
        }
        ++i_iter;
    }
    
    // adjust debug_card position
    int size = static_cast<int>(debug_vec.size());
    const auto& config = _ming_align_config.at(index - 1);
    for (int i = 0; i < size; ++i)
        adjustCardAlign(debug_vec.at(i), i, size, config);
}

void RoomCardManager::updateSelectedVecByDebugMing(int index) {
    if (!_debug_ming || index == 0)
        return;
    
    auto& debug_vec  = _debug_card[index];
    auto& inside_vec = _inside_card[index];
    for (size_t i = 0; i < debug_vec.size(); ++i)
        debug_vec[i]->isSelected() ? refCardToSelected(inside_vec[i], index)
                                   : derefCardFromSelected(inside_vec[i], index);
}

void RoomCardManager::updateDebugMingSelected(int index) {
    if (!_debug_ming || index == 0)
        return;

    auto& debug_vec  = _debug_card[index];
    auto& inside_vec = _inside_card[index];
    for (size_t i = 0; i < inside_vec.size(); ++i)
        if (inside_vec[i]->isSelected()) {
            debug_vec[i]->addMask();
            debug_vec[i]->switchSelected();
        }
}

void RoomCardManager::initRestCardType(const std::string& path, int x) {
    Sprite* sp = Sprite::createWithSpriteFrameName("table_gz_frame.png");
    Sprite* type = Sprite::createWithSpriteFrameName(path);
    Sprite* val  = Sprite::createWithSpriteFrameName("table_gz_x" + std::to_string(x) + ".png");
    type->setPosition(20, sp->getContentSize().height*0.5);
    sp->addChild(type);
    val->setPosition(47, sp->getContentSize().height*0.5);
    sp->addChild(val);
    this->addChild(sp, 10, "RestCarType");
    sp->setPosition(Director::getInstance()->getVisibleSize().width * 0.5,
                    Director::getInstance()->getVisibleSize().height - 40);
    sp->setRotation(-15);
}

void RoomCardManager::rmRestCardType() {
    auto child = this->getChildByName("RestCarType");
    if (child) child->removeFromParent();
}
