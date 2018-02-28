//
//  RoomCardManager.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#ifndef RoomCardManager_hpp
#define RoomCardManager_hpp

#include "UICard.hpp"
#include "CardAlignConfig.hpp"

class RoomCardManager : public cocos2d::Node
{
public:
    using CardVec = std::vector<UICard*>;
    
    CREATE_FUNC(RoomCardManager);
    
    // create & clear
    void createCard(const std::vector<int>& vec);
    void clearAllCard();
    
    // card action
    void callbackForDeal();
    void callbackForPushRestCard(int landlord);
    bool callbackForPopCard(int index);
    
    // card data operate
    void pushCardToInside(UICard* card, int index);
    
    void refCardToSelected(UICard* card, int index);
    void derefCardFromSelected(UICard* card, int index);
    void resetSelected(int index);
    
    void pushCardToOutside(int index);
    void clearCardFromOutside(int index);
    
    // listener for touch card
    void openListenerForTouchCard();
    void stopListenerForTouchCard();
    
    bool isEmptyForInsideCard(int index)              { return _inside_card[index].empty(); }
    bool isEmptyForOutsideCard(int index)             { return _outside_card[index].empty(); }
    CardAlignConfig& getInsideAlignConfig(int index)  { return _inside_align_config.at(index); }
    CardAlignConfig& getOutsideAlignConfig(int index) { return _outside_align_config.at(index); }
    CardVec& getTotalCardVec()                        { return _total_card_vec; }
    CardVec& getRestCardVec()                         { return _rest_card_vec; }
    CardVec& getInsideVec(int index)                  { return _inside_card[index]; }
    
    // debug
    void setDebugMing(bool open);
    void openDebugListener();
    void stopDebugListener();
    
    // decons.
    virtual ~RoomCardManager();
    
private:
    bool init() override;
    
    void initCardAlignConfig();
    void initCardCountLabel();
    
    void updateCardCountLabel(int index);
    void updateCardDisplayForInside(int index);
    void updateCardDisplayForOutside(int index);
    
    int  getTouchCardIndex(cocos2d::Touch* touch) const;
    void updateMaskLayerForTouchCard(int first, int last);
    
private:
    std::vector<CardAlignConfig> _inside_align_config;
    std::vector<CardAlignConfig> _outside_align_config;
    std::vector<CardAlignConfig> _ming_align_config;
    
    bool _debug_ming;
    std::vector<CardVec> _ming_card;
    std::vector<cocos2d::EventListenerTouchOneByOne*> _debug_touch_card;
    
    CardVec _total_card_vec;
    CardVec _rest_card_vec;
    
    std::map<int, CardVec> _inside_card;
    std::map<int, CardVec> _selected_card;
    std::map<int, CardVec> _outside_card;
    
    std::vector<cocos2d::Label*> _inside_card_count;
    
    cocos2d::EventListenerTouchOneByOne* _listener_touch_card;
};


#endif /* RoomCardManager_hpp */
