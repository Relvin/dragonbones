//
//  DBSlot.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "renderer/DBSlot.h"
#include "renderer/DBArmature.h"
#include "renderer/DBBone.h"
#include "animation/DBSlotTimelineState.h"
#include "animation/DBAnimationState.h"
#include "objects/SlotFrame.h"

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
        origin = displayData->transform;
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
            
            if ((pivotX > 0.000001f && pivotX >= -0.000001f) || (pivotY <= 0.000001f && pivotY >= -0.000001f))
            {
                _display->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
            }
            else
            {
                cocos2d::Size size = _display->getContentSize();
                _display->setAnchorPoint(cocos2d::Vec2(pivotX / size.width, 1.f - pivotY / size.height));
            }
        }
        
    }
    
    return true;
}


DBSlot::DBSlot()
: _display (nullptr)
, _slotData (nullptr)
, _needUpdate (true)
, _tweenZOrder(0.f)
, _isShowDisplay (false)
, _isColorChanged (false)
//,_displayIndex(-1)
//,_originZOrder(0.f)
//,_offsetZOrder(0.f)
//,_blendMode(BlendMode::BM_NORMAL)
//,_childArmature(nullptr)


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

void DBSlot::arriveAtFrame( Frame *frame, const DBSlotTimelineState *timelineState, DBAnimationState *animationState, bool isCross )
{
    // TODO:
    bool displayControl = animationState->displayControl && animationState->containsBoneMask(this->getName());
    if (displayControl)
    {
        SlotFrame *slotFrame = static_cast<SlotFrame*>(frame);
        const int displayIndex = slotFrame->displayIndex;
        DBSlot *childSlot = nullptr;
        
        changeDisplay(displayIndex);
        updateDisplayVisible(slotFrame->visible);
        if (displayIndex >= 0)
        {
            if (slotFrame->zOrder != _tweenZOrder)
            {
                _tweenZOrder = slotFrame->zOrder;
                DBArmature* armeture = dynamic_cast<DBArmature*>(_armature);
                if (armeture)
                {
                    //modify by Relvin todo
//                    _armature->_slotsZOrderChanged = true;
                }
                
            }
        }
        
        if (!frame->action.empty())
        {
            //modify by Relvin
//            if (_childArmature)
//            {
//                _childArmature->_animation->gotoAndPlay(frame->action);
//            }
        }
    }
}

void DBSlot::changeDisplay(int displayIndex)
{
#if 0
    if (displayIndex < 0)
    {
        if (_isShowDisplay)
        {
            _isShowDisplay = false;
            removeDisplayFromContainer();
            updateChildArmatureAnimation();
        }
    }
    else if (!_displayList.empty())
    {
        if (displayIndex >= (int)(_displayList.size()))
        {
            displayIndex = _displayList.size() - 1;
        }
        
        if (_displayIndex != displayIndex)
        {
            _isShowDisplay = true;
            _displayIndex = displayIndex;
            updateSlotDisplay(false);
            
            if (
                _slotData &&
                !_slotData->displayDataList.empty() &&
                _displayIndex < (int)(_slotData->displayDataList.size())
                )
            {
                origin = _slotData->displayDataList[_displayIndex]->transform;
            }
        }
        else if (!_isShowDisplay)
        {
            _isShowDisplay = true;
            
            if (_armature)
            {
                _armature->_slotsZOrderChanged = true;
                addDisplayToContainer(_armature->_display, -1);
            }
            
            updateChildArmatureAnimation();
        }
    }
#endif
}


void DBSlot::updateDisplayVisible(bool visible)
{
    DBBone* _bone = dynamic_cast<DBBone *>(_parentBone);
    if (_display && _bone)
    {
        _display->setVisible(_bone->isVisible() && this->isVisible());
    }
}

void DBSlot::updateDisplayColor(int aOffset, int rOffset, int gOffset, int bOffset, float aMultiplier, float rMultiplier, float gMultiplier, float bMultiplier, bool colorChanged)
{
    if (_display)
    {
        _colorTransform.alphaOffset = aOffset;
        _colorTransform.redOffset = rOffset;
        _colorTransform.greenOffset = gOffset;
        _colorTransform.blueOffset = bOffset;
        _colorTransform.alphaMultiplier = aMultiplier;
        _colorTransform.redMultiplier = rMultiplier;
        _colorTransform.greenMultiplier = gMultiplier;
        _colorTransform.blueMultiplier = bMultiplier;
        
        _isColorChanged = colorChanged;
        
        // cocos2dx does not support offset of color.
        _display->setOpacity(aMultiplier * 255.f);
        _display->setColor(cocos2d::Color3B(rMultiplier * 255.f , gMultiplier * 255.f , bMultiplier * 255.f));
    }
}

NAME_SPACE_DRAGON_BONES_END
