//
//  CommonMethodSet.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/13.
//

#ifndef CommonMethodSet_hpp
#define CommonMethodSet_hpp

#define XXF_NS_BEGIN namespace _xxf {
#define XXF_NS_END }

XXF_NS_BEGIN

/**
 * 将对应数字输入转化为带单位的字符串输出
 * 单位：千、万、亿
 */
std::string convertNumberToUnitStr(int num);

/**
 * 获取包含本地日期或本地时间的字符串
 */
std::string getCurrentLocalDate();
std::string getCurrentLocalTime();

/**
 * 判断触摸点是否在目标节点的有效范围内
 * @param target: 目标节点，锚点是[0.5, 0.5]
 */
bool isTouchPointAtValidScope(cocos2d::Touch* touch, cocos2d::Node* target);
bool isTouchPointAtValidScope(cocos2d::Touch* touch, cocos2d::Node* target, float scale);
bool isTouchPointAtValidScope(cocos2d::Touch* touch, cocos2d::Node* target, const cocos2d::Size& delta);

XXF_NS_END

#endif /* CommonMethodSet_hpp */
