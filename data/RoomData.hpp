//
//  RoomData.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/13.
//

#ifndef RoomData_hpp
#define RoomData_hpp

#include "json/document.h"

struct RoomConfig {
    int gold_min;
    int gold_max;
    int ante;
    int cost;
    int double_limit;
    int ming_limit;
    int settle_limit;
    std::string id;
    std::string des;
    std::string image;
};

struct RoomMode {
    bool is_shuffle;    // true:洗牌,   false:不洗牌
    bool is_snatch;     // true:抢地主, false:叫分
    bool is_classic;    // true:经典,   false:癞子
};

struct OneStepInfo {
    int seat;
    float duration;
    std::string option;
    std::vector<int> card_id_vec;
};

class CardData;
class PlayerData;

class RoomDataManager
{
public:
    // 牌局数据管理单例的创建与销毁
    static RoomDataManager* getInstance();
    static void destroyInstance();
    
    /**
     * 读取房间数值配置json文件
     * @param file_path: 配置文件路径
     */
    void loadRoomConfigFile(const std::string& file_path);
    
    /**
     * 获取房间配置的文档const引用
     */
    const rapidjson::Document& getConfigDoc() const { return _doc; }
    
    /**
     * 返回当前房间的数值配置结构
     */
    const RoomConfig& getRoomConfig() const { return _config; }
    
    /**
     * 初始化成room_id对应的房间配值
     * @param room_id: 房间id
     */
    bool initWithRoomId(const std::string& room_id);
    
    /**
     * 初始化成game_id对应的房间数值
     * @param game_id：保存的旧牌局数据id
     */
    bool initWithGameId(const std::string& game_id);
    
    /**
     * 对玩家数据的相关操作
     */
    void addPlayerData(PlayerData* pdata);
    void rmPlayerData(int seat_id);
    void rmAllPlayerData(bool include_myself = true);
    void clearPlayerOptRec();
    
    /**
     * 对手牌数据的相关操作
     */
    void addCardData(CardData* card, int seat_id);
    void rmCardData(CardData* card, int seat_id);
    void rmAllCardData();
    
    /**
     * 对牌局进度的相关操作
     */
    void addOneStepInfo(int seat, float dur, const std::string& opt, const std::vector<int>& vec);
    const OneStepInfo& getStepInfo(int index) const { return _step_info_vec.at(index); }
    void clearStepInfoVec()                         { _step_info_vec.clear(); }
    
    const RoomMode& getRoomMode() const                     { return _mode; }
    PlayerData* getPlayerData(int key)                      { return _pdata_map.at(key); }
    const std::map<int, PlayerData*>& getPdataMap() const   { return _pdata_map; }
    const std::vector<CardData*>& getPCardList(int key) const { return _pcard_map.at(key); }
    
private:
    RoomDataManager();
    ~RoomDataManager();
    
    std::string createGameIdByTime() const;
    
private:
    rapidjson::Document _doc;
    RoomConfig _config;
    RoomMode _mode;
    
    std::string _room_id;
    std::string _game_id;
    
    std::map<int, PlayerData*> _pdata_map;
    std::map<int, std::vector<CardData*>> _pcard_map;
    std::vector<OneStepInfo> _step_info_vec;
    
    CC_SYNTHESIZE(int, _player_num, PlayerNum);         // 玩家数量
    CC_SYNTHESIZE(int, _deal_card_num, DealCardNum);    // 发牌数量
    CC_SYNTHESIZE(int, _total_card_num, TotalCardNum);  // 纸牌数量 = 底牌数量 + 发牌数量
};

#endif /* RoomData_hpp */
