//
//  CardTypeHelper.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/8.
//

#ifndef CardTypeHelper_hpp
#define CardTypeHelper_hpp

#include "CardTypeDefine.hpp"

class CardTypeHelper
{
public:
    /**
     * 按最大出现次数拆分
     * 比如：src = {1,1,1}，
     * 数字1出现3次，则只计入 key=1
     * @note src默认升序排列，否则不能正常工作
     */
    static NumMap splitByDepth(const NumVec& src);
    
    /**
     * 按出现次数范围拆分
     * 比如 src = {1,1,1}，
     * 数字1出现3次，则分别计入 key=1，key=2，key=3中
     * @note src默认升序排列，否则不能正常工作
     */
    static NumMap splitByRange(const NumVec& src);
    
    /**
     * @brief 用于拆分顺子
     * @param src 待分析的点数序列：需要先做“去重”,“升序排列”处理
     * @param min_len 满足要求的顺子长度需要大于或等于这个值
     * @param limit_number 顺子的点数上限默认是14(扑克A)
     * @return 返回符合要求的顺子序列
     */
    static std::vector<NumVec> splitStraight(const NumVec& src, int min_len, int limit_number = 14);
    
    /**
     * 识别牌型
     */
    static CardType identifyCardType(const NumVec& src);
    
    /**
     * 寻找牌型
     * @note: 暂时未对带牌部分做处理 
     */
    static std::vector<NumVec> foundGreaterCardType(const NumVec& src, const CardType& target);
    static std::vector<NumVec> foundTheSameCardType(const NumVec& src, const CardType& target);

    /**
     * 构建出牌优先序列
     */
    static std::vector<NumVec> buildCardType(const NumVec& src);
    static std::vector<NumVec> buildCardType(const NumVec& src, const CardType& target);
};

#endif /* CardTypeHelper_hpp */
