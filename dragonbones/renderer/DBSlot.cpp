//
//  DBSlot.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBSlot.h"

NAME_SPACE_DRAGON_BONES_BEGIN


DBSlot* DBSlot::create(SlotData* slotData)
{
    DBSlot* slot = new (std::nothrow) DBSlot();
    if(slot && slot->initWithSlotData(slotData))
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

bool DBSlot::initWithSlotData(SlotData* slotData)
{
    
#if 0
    std::vector<std::pair<cocos2d::Node*, DisplayType>> displayList;
    
    for (size_t j = 0, l = slotData->displayDataList.size(); j < l; ++j)
    {
        const DisplayData *displayData = slotData->displayDataList[j];
        
        switch (displayData->type)
        {
            case DisplayType::DT_ARMATURE:
            {
                DisplayData *displayDataCopy = nullptr;
                
                if (skinDataCopy)
                {
                    const SlotData *slotDataCopy = skinDataCopy->getSlotData(slotData->name);
                    
                    if (slotDataCopy)
                    {
                        displayDataCopy = slotDataCopy->displayDataList[i];
                    }
                }
                std::string currentDragonBonesDataName = _currentDragonBonesDataName;
                std::string currentTextureAtlasName = _currentTextureAtlasName;
                Armature *childArmature = buildArmature(displayData->name, "", displayDataCopy ? displayDataCopy->name : "", currentDragonBonesDataName, currentTextureAtlasName);
                displayList.push_back(std::make_pair(childArmature, DisplayType::DT_ARMATURE));
                _currentDragonBonesDataName = currentDragonBonesDataName;
                _currentTextureAtlasName = currentTextureAtlasName;
                break;
            }
            case DisplayType::DT_IMAGE:
            {
                cocos2d::Node *display = getTextureDisplay(displayData->name, _currentTextureAtlasName, displayData);
                displayList.push_back(std::make_pair(display, DisplayType::DT_IMAGE));
                break;
            }
                
                /*
                 case DisplayType::DT_FRAME:
                 {
                 break;
                 }
                 
                 case DisplayType::DT_TEXT:
                 {
                 break;
                 }
                 */
                
            default:
                displayList.push_back(std::make_pair(nullptr, DisplayType::DT_IMAGE));
                break;
        }
    }
    
    if (!displayList.empty())
    {
        slot->setDisplayList(displayList, false);
        
        // change
        slot->changeDisplay(slotData->displayIndex);
    }
    
#endif
    return true;
}


DBSlot::DBSlot()
{
    
}

DBSlot::~DBSlot()
{
    
}


NAME_SPACE_DRAGON_BONES_END
