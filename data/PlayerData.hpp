//
//  PlayerData.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/13.
//

#ifndef PlayerData_hpp
#define PlayerData_hpp

#include "base/CCRef.h"
#include <string>

struct PlayerConfig {
    int level;
    int gold;
    int diamond;
    std::string name;
    std::string image;
};

class PlayerData : public cocos2d::Ref
{
public:
    /**
     * 按默认配置文件路径和玩家序号读取玩家数据
     */
    static PlayerData* loadPlayerConfig(const std::string& key, const std::string& path = "config/robot_data.json");
    static PlayerData* getMyHero();
    
    /**
     * 查询玩家在牌局中的选项结果
     * @param opt_event 要查询的选项事件，例如“抢地主”
     * @return 0 没有执行过这个选项
     * @return 1 执行过这个选项，例如“加倍”
     * @return 2 拒绝过这个选项，例如“不加倍”
     * @note 叫分选项中，return 3表示“叫1分”，4表示“叫2分”，5表示“叫3分”
     */
    int getOptResult(const std::string& opt_event);
    
    /**
     * 清除玩家在牌局中的牌局所有选项结果记录
     */
    void clearOptRecord() { _opt_rec = 0x0000; }
    
    /**
     * 更新玩家在牌局中的选项结果
     * @param option 玩家的选项选择操作 例如对“抢地主”，存在“抢”和“不抢”两个操作
     */
    void updateOptRecord(const std::string& option);
    
    PlayerConfig& getPlayerConfig() { return _config;   }
    int  getSeatId() const          { return _seat_id;  }
    void setSeatId(int id)          { _seat_id = id;    }
    int  getMultiple() const        { return _multiple; }
    void setMultiple(int multiple)  { _multiple = multiple;    }
    bool isLandlord() const         { return _is_landlord;     }
    void setLandlord(bool landlord) { _is_landlord = landlord; }
    
protected:
    // constructor and deconstructor
    PlayerData();
    virtual ~PlayerData();
    
    virtual bool init(const std::string& key, const std::string& path);
    
    virtual int getDigitFromOptRecord(int index) const;
    
protected:
    PlayerConfig _config;
    int _opt_rec;
    int _seat_id;
    int _multiple;
    bool _is_landlord;
};

#endif /* PlayerData_hpp */
