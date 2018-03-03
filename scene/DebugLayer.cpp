//
//  DebugLayer.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#include "DebugLayer.hpp"
#include "UICard.hpp"
using namespace cocos2d;

bool DebugLayer::init() {
    if (!Layer::init())
        return false;
    
    // card
    Vec2 start_pos(600, 470);
    for (int i = 1; i <= 54; ++i) {
        int row = i > 52 ? 4 : (i - 1) % 4;
        int col = i > 52 ? i - 53 : (i-1) / 4;
        UICard* card = UICard::create(i);
        card->setModel(UICard::Model::RESTCARD);
        card->setFaceTowardUp(true);
        Vec2 delta(card->getSizeAfterZoom().width * col, card->getSizeAfterZoom().height * row);
        card->setPosition(delta + start_pos);
        this->addChild(card);
        Vector<FiniteTimeAction*> ac = card->getFlopAnimation();
        ac.pushBack(DelayTime::create(2.0f));
        card->runAction(RepeatForever::create(Sequence::create(ac)));
    }
    
    return true;
}
