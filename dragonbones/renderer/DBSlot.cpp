//
//  DBSlot.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBSlot.h"

NAME_SPACE_DRAGON_BONES_BEGIN


DBSlot* DBSlot::create()
{
    DBSlot* slot = new (std::nothrow) DBSlot();
    if(slot && slot->init())
    {
        slot->autorelease();
        return slot;
    }
    CC_SAFE_DELETE(slot);
    return nullptr;
}

bool DBSlot::init()
{
    return true;
}

DBSlot::DBSlot()
{
    
}

DBSlot::~DBSlot()
{
    
}


NAME_SPACE_DRAGON_BONES_END
