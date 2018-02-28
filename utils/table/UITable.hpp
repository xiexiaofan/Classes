//
//  UITable.hpp
//  demo_ddz
//
//  Created by 谢小凡 on 2018/2/12.
//

#ifndef UITable_hpp
#define UITable_hpp

#include "cocos2d.h"

class UITable : public cocos2d::Node
{
public:
    enum class Type {
        Default,
        Waterfall,
        Tollgate,
        Desert,
        Glacier,
        Ring
    };
    static UITable* create(Type type);
    bool init(Type type);
    
    void displayForGameNotBegin();
    
    void displayForGameBegin();
    
private:
    bool initTableImage(Type type);
    
    void createTableNameFont();
    
    void createTableDescirbeWin();
    
private:
    Type _type;
    cocos2d::Sprite* _table_bg;
    cocos2d::Sprite* _table;
    cocos2d::Label*  _table_name;
    cocos2d::Node*   _table_des;
};

#endif /* UITable_hpp */
