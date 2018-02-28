//
//  PlayerData.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/13.
//

#include "PlayerData.hpp"
#include "CommonDefineSet.hpp"
#include "platform/CCFileUtils.h"
#include "json/rapidjson.h"
#include "json/document.h"
using namespace cocos2d;

// ues list initialization
const std::unordered_map<std::string, unsigned int> opt_res_ref {
    {_xxf::en_call,     0x1},
    {_xxf::en_nocall,   0x2},
    {_xxf::en_bet_one,  0x3},
    {_xxf::en_bet_two,  0x4},
    {_xxf::en_bet_thr,  0x5},
    {_xxf::en_rob,      0x10},
    {_xxf::en_norob,    0x20},
    {_xxf::en_double,   0x100},
    {_xxf::en_nodouble, 0x200},
    {_xxf::en_ming,     0x1000}
};

PlayerData::PlayerData()
: _config(PlayerConfig())
, _opt_rec(0x0000)
, _seat_id(0)
, _multiple(1)
, _is_landlord(false)
{}

PlayerData::~PlayerData()
{}

PlayerData* PlayerData::loadPlayerConfig(const std::string& key, const std::string& path) {
    PlayerData* ret = new PlayerData();
    if (ret && ret->init(key, path))
        ret->autorelease();
    else
        CC_SAFE_DELETE(ret);
    return ret;
}

PlayerData* PlayerData::getMyHero() {
    return PlayerData::loadPlayerConfig("hero", "config/hero_data.json");
}

int PlayerData::getOptResult(const std::string& opt_event) {
    if (opt_event == _xxf::en_call)
        return getDigitFromOptRecord(0);
    else if (opt_event == _xxf::en_rob)
        return getDigitFromOptRecord(4);
    else if (opt_event == _xxf::en_double)
        return getDigitFromOptRecord(8);
    else if (opt_event == _xxf::en_ming)
        return getDigitFromOptRecord(12);
    else
        return -1;   /* wrong input */
}

void PlayerData::updateOptRecord(const std::string& option) {
    auto iter = opt_res_ref.find(option);
    if (iter != opt_res_ref.end())
        _opt_rec += iter->second;
    else
        log("no res for this opt: %s", option.c_str());
}

bool PlayerData::init(const std::string& key, const std::string& path) {
    std::string content = FileUtils::getInstance()->getStringFromFile(path);
    rapidjson::Document doc;
    doc.Parse<0>(content.c_str());
    if (doc.HasParseError() || !doc.HasMember(key.c_str()))
        return false;
    const rapidjson::Value& value = doc[key.c_str()];
    _config.name  = value["name"].GetString();
    _config.image = value["image"].GetString();
    _config.gold  = value["gold"].GetInt();
    _config.level = value["level"].GetInt();
    return true;
}

int PlayerData::getDigitFromOptRecord(int index) const {
    return 0x000f & (_opt_rec >> index);
}
