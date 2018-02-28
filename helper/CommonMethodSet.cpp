//
//  CommonMethodSet.cpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/13.
//

#include "CommonMethodSet.hpp"
#include <ctime>

XXF_NS_BEGIN

std::string convertNumberToUnitStr(int num) {
    return std::to_string(num);
}

std::string getCurrentLocalDate() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    std::string year_str = std::to_string((lt->tm_year));
    std::string mon_str  = std::to_string(lt->tm_mon + 1);
    std::string day_str  = std::to_string(lt->tm_mday);
    mon_str = (mon_str.size() == 1 ? '0' + mon_str : mon_str);
    day_str = (day_str.size() == 1 ? '0' + day_str : day_str);
    return year_str + mon_str + day_str;
}

std::string getCurrentLocalTime() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    std::string hour_str = std::to_string(lt->tm_hour);
    std::string min_str  = std::to_string(lt->tm_min);
    std::string sec_str  = std::to_string(lt->tm_sec);
    hour_str = (hour_str.size() == 1 ? '0' + hour_str : hour_str);
    min_str  = (min_str.size()  == 1 ? '0' + min_str  : min_str);
    sec_str  = (sec_str.size()  == 1 ? '0' + sec_str  : sec_str);
    return hour_str + min_str + sec_str;
}

bool isTouchPointAtValidScope(cocos2d::Touch* touch, cocos2d::Node* target) {
    return isTouchPointAtValidScope(touch, target, 1.0);
}

bool isTouchPointAtValidScope(cocos2d::Touch* touch, cocos2d::Node* target, float scale) {
    cocos2d::Vec2 touch_in_node = target->convertTouchToNodeSpaceAR(touch);
    cocos2d::Size valid_touch_scope = target->getContentSize() * scale;
    return touch_in_node.x >= valid_touch_scope.width  * -0.5 &&
    touch_in_node.x <= valid_touch_scope.width  *  0.5 &&
    touch_in_node.y >= valid_touch_scope.height * -0.5 &&
    touch_in_node.y <= valid_touch_scope.height *  0.5;
}

bool isTouchPointAtValidScope(cocos2d::Touch* touch, cocos2d::Node* target, const cocos2d::Size& delta) {
    cocos2d::Vec2 touch_in_node = target->convertTouchToNodeSpaceAR(touch);
    cocos2d::Size valid_touch_scope = target->getContentSize() + delta;
    return touch_in_node.x >= valid_touch_scope.width  * -0.5 &&
    touch_in_node.x <= valid_touch_scope.width  *  0.5 &&
    touch_in_node.y >= valid_touch_scope.height * -0.5 &&
    touch_in_node.y <= valid_touch_scope.height *  0.5;
}

XXF_NS_END
