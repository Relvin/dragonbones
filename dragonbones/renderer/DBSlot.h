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
#include "core/DBBase.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class DBSlot
: public cocos2d::Node
, public DBBase
{
public:
    static DBSlot* create(SlotData* slotData,const std::string & textureName);
    bool init();
    bool initWithSlotData(SlotData* slotData,const std::string & textureName);
    
    DBSlot();
    virtual ~DBSlot();
    
    virtual void update(float delta);
    void updateDisplayTransform();
    
private:
    cocos2d::Node* _display;
    SlotData* _slotData;
    bool _needUpdate;
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBSlot_h */
