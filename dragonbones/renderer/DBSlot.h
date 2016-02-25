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
#include "2d/CCNode.h"
#include "objects/SlotData.h"
#include "renderer/DBSkin.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class DBSlot
: public cocos2d::Node
{
public:
    static DBSlot* create(SlotData* slotData);
    bool init();
    bool initWithSlotData(SlotData* slotData);
    
    DBSlot();
    virtual ~DBSlot();
    
private:
    cocos2d::Node* _display;
    SlotData* _slotData;
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBSlot_h */
