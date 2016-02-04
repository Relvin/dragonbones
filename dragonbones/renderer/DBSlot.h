//
//  DBSlot.hpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBSlot_hpp
#define DBSlot_hpp

#include "DragonBones.h"
#include "2d/CCSprite.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class DBSlot
: public cocos2d::Sprite
{
public:
    static DBSlot* create();
    bool init();
    
    DBSlot();
    virtual ~DBSlot();
    
private:
    
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBSlot_h */
