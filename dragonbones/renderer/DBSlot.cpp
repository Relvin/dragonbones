//
//  DBSlot.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBSlot.h"
#include "DBArmature.h"
#include "DBBone.h"

NAME_SPACE_DRAGON_BONES_BEGIN


DBSlot* DBSlot::create(SlotData* slotData,const std::string & textureName)
{
    DBSlot* slot = new (std::nothrow) DBSlot();
    if(slot && slot->initWithSlotData(slotData,textureName))
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

bool DBSlot::initWithSlotData(SlotData* slotData,const std::string & textureAtlasName)
{
    if (!Node::init())
    {
        return false;
    }
    
    this->_slotData = slotData;
    int displayIndex = slotData->displayIndex;
    
    displayIndex = displayIndex < 0 ? 0 : displayIndex;
    displayIndex = displayIndex > slotData->displayDataList.size() ? slotData->displayDataList.size() - 1 : displayIndex;
    DisplayData *displayData = nullptr;
    if (displayIndex >= 0)
    {
        displayData = slotData->displayDataList[slotData->displayIndex];
    }
    
    if (displayData)
    {
        if (displayData->type == DisplayType::DT_IMAGE)
        {
            _display = DBSkin::create(displayData->name,textureAtlasName);
        }
        else if (displayData->type == DisplayType::DT_ARMATURE)
        {
//            _display = DBArmature::create("");
        }
        
        if (_display)
        {
            this->addChild(_display);
            float pivotX = 0.f;
            float pivotY = 0.f;
            
            if (displayData)
            {
                pivotX = displayData->pivot.x;
                pivotY = displayData->pivot.y;
            }
            
            if (pivotX > 0.000001f || pivotY != 0.000001f)
            {
                cocos2d::Size size = _display->getContentSize();
                _display->setAnchorPoint(cocos2d::Vec2(pivotX / size.width, 1.f - pivotY / size.height));
            }
            else
            {
                _display->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
            }
        }
        
    }
    
    return true;
}


DBSlot::DBSlot()
: _slotData (nullptr)
{
    
}

DBSlot::~DBSlot()
{
    
}

void DBSlot::update(float delta)
{
    DBBone *parentBone = dynamic_cast<DBBone *>(_parentBone);
    if (!parentBone || (parentBone->getNeedUpdate() <= 0 && _needUpdate == false))
    {
        return;
    }
    
    //	Transform transform;
    //	Matrix matrix;
    //	updateGlobal(transform, matrix);
    //	updateDisplayTransform();
    //    _needUpdate = false;
    //    return;
    Point tweenPivot = parentBone->getTweenPivot();
    const float x = origin.x + offset.x + tweenPivot.x;
    const float y = origin.y + offset.y + tweenPivot.y;
    const Matrix &parentMatrix = parentBone->globalTransformMatrix;
    globalTransformMatrix.tx = global.x = parentMatrix.a * x + parentMatrix.c * y + parentMatrix.tx;
    globalTransformMatrix.ty = global.y = parentMatrix.d * y + parentMatrix.b * x + parentMatrix.ty;
    
    if (inheritRotation)
    {
        global.skewX = origin.skewX + offset.skewX + parentBone->global.skewX;
        global.skewY = origin.skewY + offset.skewY + parentBone->global.skewY;
    }
    else
    {
        global.skewX = origin.skewX + offset.skewX;
        global.skewY = origin.skewY + offset.skewY;
    }
    
    if (inheritScale)
    {
        global.scaleX = origin.scaleX * offset.scaleX * parentBone->global.scaleX;
        global.scaleY = origin.scaleY * offset.scaleY * parentBone->global.scaleY;
    }
    else
    {
        global.scaleX = origin.scaleX * offset.scaleX;
        global.scaleY = origin.scaleY * offset.scaleY;
    }
    
    globalTransformMatrix.a = global.scaleX * cos(global.skewY);
    globalTransformMatrix.b = global.scaleX * sin(global.skewY);
    globalTransformMatrix.c = -global.scaleY * sin(global.skewX);
    globalTransformMatrix.d = global.scaleY * cos(global.skewX);
    updateDisplayTransform();
}

void DBSlot::updateDisplayTransform()
{
    if (_display)
    {
        _display->setScaleX(global.scaleX);
        _display->setScaleY(global.scaleY);
        _display->setRotationSkewX(global.skewX * RADIAN_TO_ANGLE);
        _display->setRotationSkewY(global.skewY * RADIAN_TO_ANGLE);
        _display->setPosition(global.x , -global.y);
    }
}


NAME_SPACE_DRAGON_BONES_END
