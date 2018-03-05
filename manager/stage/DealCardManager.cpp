//
//  DealCardManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#include "DealCardManager.hpp"
#include "RoomCardManager.hpp"
#include "RoomData.hpp"
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
using namespace cocos2d;

static const Vec2& BirthPos = Vec2(569, 365);
static const float TotalTime = 1.6f;
static const float DealGap = TotalTime / RoomDataManager::getInstance()->getDealCardNum();
static const float DealDur = DealGap * RoomDataManager::getInstance()->getPlayerNum();

int myrandom (int i) { return std::rand()%i;}

void DealCardManager::startDeal() {
    _manager->getCardManager()->createCard(createCardHeap(ShuffType::RANDOM));
    _manager->resetTargetId();
    _first_target_id = _manager->getTargetId();
    _cur_deal_num = 0;
    _timer_count = 0;
    this->scheduleUpdate();
    
    SimpleAudioEngine::getInstance()->playEffect("sound/effect/dispatch_card.mp3");
}

std::vector<int> DealCardManager::createCardHeap(ShuffType type) {
    if (type == ShuffType::RANDOM)
        return shuffleByRandom();
    return {};
}

std::vector<int> DealCardManager::createCardHeap(const std::vector<int>& vec) {
    return vec;
}

void DealCardManager::initDealConfig() {
    RoomDataManager* data = RoomDataManager::getInstance();
    _total_num  = data->getTotalCardNum();
    _deal_num   = data->getDealCardNum();
    _player_num = data->getPlayerNum();
    // 初始化玩家待发手牌位置参数
    RoomCardManager* card_manager = _manager->getCardManager();
    int each_play_deal = _deal_num / _player_num;
    for (int i = 0; i < _player_num; ++i) {
        std::vector<SingleCardConfig> base;
        base.reserve(each_play_deal);
        const auto& align_config = card_manager->getInsideAlignConfig(i);
        for (int j = 0; j < each_play_deal; ++j) {
            const auto& config = getSingleCardConfig(j, each_play_deal, align_config);
            base.push_back(config);
        }
        _card_info_vec.push_back(base);
    }
    // 初始化随机数种子
    std::srand(std::time(nullptr));
}

void DealCardManager::endDeal() {
    this->unscheduleUpdate();
    CallFunc* c1 = CallFunc::create([&]{_manager->getCardManager()->callbackForDeal();});
    CallFunc* c2 = CallFunc::create([&]{
        _manager->openListenerForTouchCard();
        _manager->setStageCode(GameManager::StageCode::SELECT);
        _manager->run();
    });
    this->runAction(Sequence::create(c1, DelayTime::create(1.0f), c2, nullptr));
}

std::vector<int> DealCardManager::shuffleByRandom() const {
    std::vector<int> ret(_total_num);
    std::iota(ret.begin(), ret.end(), 1);
    std::random_shuffle(ret.begin(), ret.end(), myrandom);
    /* bubble sort
     for(int i = 0; i < _deal_num; ++i)
     for (int j = 0; j < _deal_num - _player_num - i; ++j)
     if (ret[j] < ret[j + _player_num])
     std::swap(ret[j], ret[j + _player_num]);
     */
    return ret;
}

void DealCardManager::update(float dt) {
    _timer_count += dt;
    if (_cur_deal_num >= _deal_num) {
        if (_timer_count > DealDur)
            endDeal();
        return;
    }
    if (_timer_count < DealGap)
        return;
    // 发出一张牌
    _timer_count = 0;
    UICard* card = _manager->getCardManager()->getTotalCardVec().at(_cur_deal_num);
    const auto& target = (_first_target_id + _cur_deal_num) % _player_num;
    const auto& config = _card_info_vec.at(target).at(_cur_deal_num / _player_num);
    runDealAnimation(card, DealDur, BirthPos, config.point, 0.85, config.scale, config.zorder, target == 0);
    _manager->getCardManager()->pushCardToInside(card, target);
    _cur_deal_num++;
//    log("cur deal = %d finished", _cur_deal_num);
}

void DealCardManager::runDealAnimation(UICard* card, float dur, const cocos2d::Vec2& origin, const Vec2& dest,
                                       float begin_scale, float end_scale, float zorder, bool flipped) {
    card->setVisible(true);
    card->setPosition(origin);
    card->setScale(begin_scale);
    CallFunc* zorder_call = CallFunc::create([card, zorder]{card->setLocalZOrder(zorder);});
    Spawn* base = Spawn::create(MoveTo::create(dur, dest), ScaleTo::create(dur, end_scale), nullptr);
    if (!flipped) {
        card->runAction(Sequence::create(zorder_call, base, nullptr));
    } else {
        Vector<FiniteTimeAction*> ac_vec;
        ac_vec.pushBack(zorder_call);
        ac_vec.pushBack(base);
        Vector<FiniteTimeAction*> flop_vec = card->getFlopAnimation();
        for (auto ac : flop_vec) ac_vec.pushBack(ac);
        card->runAction(Sequence::create(ac_vec));
    }
}
