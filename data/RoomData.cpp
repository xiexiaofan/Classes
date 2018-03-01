//
//  RoomData.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/13.
//

#include "RoomData.hpp"
#include "CardData.hpp"
#include "PlayerData.hpp"
#include "CommonMethodSet.hpp"
#include "json/rapidjson.h"
#include "platform/CCFileUtils.h"

static RoomDataManager* Instance = nullptr;

RoomDataManager* RoomDataManager::getInstance() {
    if (!Instance) {
        Instance = new RoomDataManager();
        Instance->loadRoomConfigFile("config/room_config.json");
    }
    return Instance;
}

void RoomDataManager::destroyInstance() {
    if (Instance)
        CC_SAFE_DELETE(Instance);
}

RoomDataManager::RoomDataManager()
: _config(RoomConfig())
, _mode(RoomMode())
, _game_id("")
, _room_id("")
, _player_num(3)
, _deal_card_num(51)
, _total_card_num(54)
{}

RoomDataManager::~RoomDataManager() {
    rmAllCardData();
    rmAllPlayerData();
}

void RoomDataManager::loadRoomConfigFile(const std::string& file_path) {
    std::string content = cocos2d::FileUtils::getInstance()->getStringFromFile(file_path);
    _doc.Parse<0>(content.c_str());
    CCASSERT(!_doc.HasParseError(), "room config file parse error.");
}

bool RoomDataManager::initWithRoomId(const std::string& room_id) {
    if (!_doc.HasMember(room_id.c_str()))
        return false;
    _room_id = room_id;
    const rapidjson::Value& value =_doc[room_id.c_str()];
    _config.id = value["name"].GetString();
    //  _config.des    = value["des"].GetString();
    _config.image = value["background"].GetString();
    _config.gold_min = value["gold_min"].GetInt();
    _config.gold_max = value["gold_max"].GetInt();
    _config.ante = value["ante"].GetInt();
    _config.cost = value["cost"].GetInt();
    _config.double_limit = value["double_limit"].GetInt();
    _config.ming_limit   = value["ming_limit"].GetInt();
    _config.settle_limit = value["settle_limit"].GetInt();
    return true;
}

bool RoomDataManager::initWithGameId(const std::string& id) {
    ; // TODD
    return true;
}

void RoomDataManager::addPlayerData(PlayerData* pdata) {
    int i = -1;
    while (_pdata_map.find(++i) != _pdata_map.end()) continue;
    pdata->setSeatId(i);
    CC_SAFE_RETAIN(pdata);
    _pdata_map[i] = pdata;
}

void RoomDataManager::rmPlayerData(int seat_id) {
    auto iter = _pdata_map.find(seat_id);
    if (iter != _pdata_map.end()) {
        PlayerData* pdata = iter->second;
        _pdata_map.erase(iter);
        CC_SAFE_RELEASE(pdata);
    }
}

void RoomDataManager::rmAllPlayerData(bool include_myself) {
    if (_pdata_map.empty())
        return;
    // myself data is front of the map
    auto first_iter = include_myself ? _pdata_map.begin() : ++_pdata_map.begin();
    for (auto iter = first_iter; iter != _pdata_map.end(); ){
        PlayerData* pdata = iter->second;
        iter = _pdata_map.erase(iter);
        CC_SAFE_RELEASE(pdata);
    }
}

void RoomDataManager::clearPlayerOptRec() {
    for (auto iter : _pdata_map) {
        iter.second->clearOptRecord();
        iter.second->setLandlord(false);
    }
}

void RoomDataManager::addCardData(CardData* card, int seat_id) {
    _pcard_map[seat_id].push_back(card);
    CC_SAFE_RETAIN(card);
}

void RoomDataManager::rmCardData(CardData* card, int seat_id) {
    auto pair = _pcard_map.find(seat_id);
    if (pair == _pcard_map.end())
        return;
    auto vec = pair->second;
    auto iter = std::find(vec.begin(), vec.end(), card);
    if (iter == vec.end())
        return;
    vec.erase(iter);
    CC_SAFE_RELEASE(card);
}

void RoomDataManager::rmAllCardData() {
    for (auto iter = _pcard_map.begin(); iter != _pcard_map.end(); ++iter) {
        auto& vec = iter->second;
        auto iter2 = vec.begin();
        while (iter2 != vec.end()) {
            CardData* card = *iter2;
            iter2 = vec.erase(iter2);
            CC_SAFE_RELEASE(card);
        }
    }
    _pcard_map.clear();
}

void RoomDataManager::addOneStepInfo(int seat, float dur, const std::string& opt, const std::vector<int>& vec) {
    OneStepInfo info;
    info.seat = seat;
    info.duration = dur;
    info.option = opt;
    info.card_id_vec = vec;
    _step_info_vec.push_back(info);
}

std::string RoomDataManager::createGameIdByTime() const {
    return _xxf::getCurrentLocalDate() + _xxf::getCurrentLocalTime();
}
