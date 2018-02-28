//
//  RoomAnimationManager.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#include "RoomAnimationManager.hpp"
#include "RoomData.hpp"
#include "CommonDefineSet.hpp"
using namespace cocos2d;

static RoomAnimationManager* Instance = nullptr;

RoomAnimationManager* RoomAnimationManager::getInstance() {
    if (!Instance) {
        Instance = new RoomAnimationManager();
        Instance->init();
    }
    return Instance;
}

void RoomAnimationManager::destroyInstance() {
    if (Instance) {
        Instance->bindCarrierNode(nullptr);
        delete Instance;
        Instance = nullptr;
    }
}

void RoomAnimationManager::bindCarrierNode(Node* node) {
    if (node) {
        _carrier = node;
        // 更新已有“文字提示动画”配置
        updatePostedConfig();
    } else {
        _carrier = nullptr;
    }
}

void RoomAnimationManager::updateMatchTimeDisplay(int cost) {
    if (!_match_name) {
        _match_name = Sprite::createWithSpriteFrameName("fonts_table_bz_zzppwj.png");
        _match_name->setPosition(450, 320);
        _match_name->setAnchorPoint(Vec2::ZERO);
        _carrier->addChild(_match_name);
    }
    if (!_match_label_point) {
        _match_label_point = Label::createWithBMFont("fonts/fonts_table_bz_num.fnt", "");
        _match_label_point->setPosition(622, 320);
        _match_label_point->setAnchorPoint(Vec2::ZERO);
        _carrier->addChild(_match_label_point);
    }
    if (!_match_label_cost) {
        _match_label_cost = Label::createWithBMFont("fonts/fonts_table_bz_num.fnt", "");
        _match_label_cost->setPosition(663, 320);
        _match_label_cost->setAnchorPoint(Vec2::ZERO);
        _carrier->addChild(_match_label_cost);
    }
    // point
    _match_label_point->setString(std::string(cost % 4, '.'));
    // cost
    _match_label_cost->setString(std::to_string(cost));
}

void RoomAnimationManager::clearMatchTimeDisplay() {
    if (_match_name) {
        _match_name->removeFromParent();
        _match_name = nullptr;
    }
    if (_match_label_point) {
        _match_label_point->removeFromParent();
        _match_label_point = nullptr;
    }
    if (_match_label_cost) {
        _match_label_cost->removeFromParent();
        _match_label_cost = nullptr;
    }
}

void RoomAnimationManager::updatePostedConfig() {
    if (RoomDataManager::getInstance()->getPlayerNum() == 3) {
        std::vector<std::pair<Vec2, Vec2>> vec = {
            {Vec2(569, 200), Vec2(0, 55)},
            {Vec2(888, 385), Vec2(-35, 0)},
            {Vec2(254, 385), Vec2(35, 0)}
        };
        std::swap(_posted_pos_config, vec);
    }
    clearAllPosted();
    _posted_creator_vec.reserve(_posted_pos_config.size());
    for (ssize_t i = 0; i < _posted_pos_config.size(); ++i) {
        PostedCreator* cre = PostedCreator::create();
        cre->setOriginPoint(_posted_pos_config.at(i).first);
        cre->setBaseDelta(_posted_pos_config.at(i).second);
        _carrier->addChild(cre);
        _posted_creator_vec.push_back(cre);
    }
}

void RoomAnimationManager::playPosted(const std::string& content, int seat_id) {
    _posted_creator_vec.at(seat_id)->updatePosted(content);
}

void RoomAnimationManager::clearCurPosted(int seat_id) {
    _posted_creator_vec.at(seat_id)->clearPosted();
}

void RoomAnimationManager::clearAllPosted() {
    for (auto& p : _posted_creator_vec)
        p->clearPosted();
}

bool RoomAnimationManager::init() {
    bindCarrierNode(Director::getInstance()->getRunningScene());
    return true;
}

void RoomAnimationManager::clearPostedCreatorVec() {
    auto iter = _posted_creator_vec.begin();
    while (iter != _posted_creator_vec.end()) {
        auto& l = *iter;
        l->removeFromParent();
        iter = _posted_creator_vec.erase(iter);
    }
}

/**
 * PostedCreator
 */
void PostedCreator::updatePosted(const std::string& content) {
    clearPosted();
    Node* node = nullptr;
    std::string sound_path = "";
    if (content == _xxf::en_nocall) {
        node = getPosted("bu", "jiao");
    } else if (content == _xxf::en_call) {
        node = getPosted("jiao", "di", "zhu");
    } else if (content == _xxf::en_norob) {
        node = getPosted("bu", "qiang");
    } else if (content == _xxf::en_rob) {
        node = getPosted("qiang", "di", "zhu");
    } else if (content == _xxf::en_nodouble) {
        node = getPosted("bu", "jia", "bei_01");  // "倍"
    } else if (content == _xxf::en_double) {
        node = getPosted("jia", "bei_01");
    } else if (content == _xxf::en_pass) {
        int ran = RandomHelper::random_int(1, 2);
        if (ran == 1)
            node = getPosted("bu", "yao");
        if (ran == 2)
            node = getPosted("bu", "chu");
    } else if (content == _xxf::en_ready) {
        node = getPosted("zhun", "bei_02");  // "备"
    } else if (content == _xxf::en_bet_one) {
        node = getPosted("1", "fen");
    } else if (content == _xxf::en_bet_two) {
        node = getPosted("2", "fen");
    } else if (content == _xxf::en_bet_thr) {
        node = getPosted("3", "fen");
    }
    
    node->setScale(0.1f);
    node->setPosition(_origin_point);
    this->addChild(node);
    Vec2 delta;
    if (_delta.y != 0)
        delta = _delta;
    else if (_delta.x > 0)
        delta = Vec2(node->getContentSize().width * 0.5 - _delta.x, 0);
    else
        delta = Vec2(-node->getContentSize().width * 0.5 - _delta.x, 0);
    Spawn* ac = Spawn::create(MoveBy::create(0.2f, delta), ScaleTo::create(0.2f, 1.0), nullptr);
    node->runAction(ac);
}

Node* PostedCreator::getPosted(const std::string& word_1, const std::string& word_2) {
    Node* ret = Node::create();
    ret->setContentSize(Size(100, 50));
    Sprite* s1 = Sprite::createWithSpriteFrameName("table_fonts_" + word_1 + ".png");
    Sprite* s2 = Sprite::createWithSpriteFrameName("table_fonts_" + word_2 + ".png");
    s1->setPositionX(-25);
    s2->setPositionX(25);
    ret->addChild(s1);
    ret->addChild(s2);
    return ret;
}

Node* PostedCreator::getPosted(const std::string& word_1, const std::string& word_2, const std::string& word_3) {
    Node* ret = Node::create();
    ret->setContentSize(Size(150, 50));
    Sprite* s1 = Sprite::createWithSpriteFrameName("table_fonts_" + word_1 + ".png");
    Sprite* s2 = Sprite::createWithSpriteFrameName("table_fonts_" + word_2 + ".png");
    Sprite* s3 = Sprite::createWithSpriteFrameName("table_fonts_" + word_3 + ".png");
    s1->setPositionX(-50);
    s2->setPositionX(0);
    s3->setPositionX(50);
    ret->addChild(s1);
    ret->addChild(s2);
    ret->addChild(s3);
    return ret;
}

