//
//  DBSkin.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/25.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBSkin.h"


NAME_SPACE_DRAGON_BONES_BEGIN


DBSkin* DBSkin::create()
{
    DBSkin* skin = new (std::nothrow) DBSkin();
    if (skin && skin->init())
    {
        skin->autorelease();
        return skin;
    }
    CC_SAFE_DELETE(skin);
    return nullptr;
}

bool DBSkin::init()
{
    if(!Sprite::init())
    {
        return false;
    }
    return true;
}

DBSkin::DBSkin()
{
    
}

DBSkin::~DBSkin()
{
    
}

NAME_SPACE_DRAGON_BONES_END
