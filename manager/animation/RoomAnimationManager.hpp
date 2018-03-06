//
//  RoomAnimationManager.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/16.
//

#ifndef RoomAnimationManager_hpp
#define RoomAnimationManager_hpp

#include "cocos2d.h"
#include "CardTypeDefine.hpp"

class PostedCreator;
class RoomAnimationManager {
    using Vec2 = cocos2d::Vec2;
public:
    static RoomAnimationManager* getInstance();
    static void destroyInstance();
    
    void bindCarrierNode(cocos2d::Node* node);
    
    // 匹配文字提示
    void updateMatchTimeDisplay(int cost);
    void clearMatchTimeDisplay();
    
    // 叫地主等文字提示
    void updatePostedConfig();
    void playPosted(const std::string& content, int seat_id);
    void clearCurPosted(int seat_id);
    void clearAllPosted();
    
    // 倍数改变动画
    // 特定牌型动画
    // 动画名称， 农民or地主， 男or女， void*
    void playCardTypeAnima(const CTName& name,
                           bool is_landlord,
                           bool is_male,
                           cocos2d::Ref* arg = nullptr);
    
    // 剩牌提示报警灯
    void playAlertAnima(int index, int rest);


    // 结算界面动画
    
private:
    bool init();
    
    void clearPostedCreatorVec();
    
    cocos2d::Node* _carrier;
    
    // posted config
    std::vector<PostedCreator*> _posted_creator_vec;
    std::vector<std::pair<Vec2, Vec2>> _posted_pos_config;
    
    // match config
    cocos2d::Sprite* _match_name;
    cocos2d::Label*  _match_label_point;
    cocos2d::Label*  _match_label_cost;
};

class PostedCreator : public cocos2d::Node
{
    using Vec2 = cocos2d::Vec2;
public:
    CREATE_FUNC(PostedCreator);
    
    void setOriginPoint(const Vec2& origin) { _origin_point = origin; }
    
    void setBaseDelta(const Vec2& delta) { _delta = delta; }
    
    void clearPosted() { this->removeAllChildren(); }
    
    void updatePosted(const std::string& content);
protected:
    cocos2d::Node* getPosted(const std::string& word);
    cocos2d::Node* getPosted(const std::string& word_1,
                             const std::string& word_2);
    cocos2d::Node* getPosted(const std::string& word_1,
                             const std::string& word_2,
                             const std::string& word_3);
    
    Vec2 _origin_point;
    Vec2 _delta;
};

#endif /* RoomAnimationManager_hpp */
