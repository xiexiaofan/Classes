//
//  CardAlignConfig.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/17.
//

#ifndef CardAlignConfig_hpp
#define CardAlignConfig_hpp

typedef cocos2d::Vec2 Vec2;

struct CardAlignConfig {
    CardAlignConfig(const Vec2& p, int t, int l, float x, float y, float s)
    : align_point(p)
    , align_type(t)
    , align_limit(l)
    , gap_x(x)
    , gap_y(y)
    , scale(s)
    {}
    
    Vec2  align_point;  // 对齐点坐标
    int   align_type;   // 水平对齐方式，-1表示左对齐，0表示居中对齐，1表示右对齐
    int   align_limit;  // 一行最大摆放数量限制
    float gap_x;        // 左右相邻牌的水平间隔
    float gap_y;        // 上下相邻牌的垂直间隔
    float scale;        // 整体的缩放系数
};

struct SingleCardConfig {
    SingleCardConfig(const Vec2& p, float z, float s)
    : point(p)
    , zorder(z)
    , scale(s)
    {}
    
    Vec2  point;   // 世界坐标
    float zorder;  // 堆叠序数
    float scale;   // 缩放系数
};

static SingleCardConfig getSingleCardConfig(int index, int size, const CardAlignConfig& config) {
    CC_ASSERT(index < size);
    int row = index / config.align_limit;
    float delta_y = config.gap_y * row;
    int col = config.align_limit > size ? size : config.align_limit;
    float delta_x = 0;
    if (config.align_type == 0)
        delta_x = (index - col/2) * config.gap_x;
    else if (config.align_type == -1)
        delta_x = index % col * config.gap_x;
    else {
        int i = index % col;
        if (index/col == size/col)
            col = size % config.align_limit;
        delta_x = (col - i - 1) * config.gap_x;
    }
    return SingleCardConfig(config.align_point + Vec2(delta_x, delta_y), index, config.scale);
}

#endif /* CardAlignConfig_hpp */
