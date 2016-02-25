//
//  DBSkin.h
//  dragonbones
//
//  Created by Relvin on 16/2/25.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBSkin_h
#define DBSkin_h

#include "2d/CCSprite.h"
#include "DragonBones.h"
NAME_SPACE_DRAGON_BONES_BEGIN


class DBSkin
: public cocos2d::Sprite
{
public:
    
    DBSkin();
    virtual ~DBSkin();
    
    static DBSkin* create();
    virtual bool init();
    
    
private:
    
    
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBSkin_hpp */
