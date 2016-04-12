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
#include "objects/MeshData.h"

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
    this->_name = this->_slotData->name;
    this->_textureAtlasName = textureAtlasName;
    _originZOrder = this->_slotData->zOrder;
    int displayIndex = slotData->displayIndex;
    
    displayIndex = displayIndex < 0 ? 0 : displayIndex;
    displayIndex = displayIndex > slotData->displayDataList.size() ? slotData->displayDataList.size() - 1 : displayIndex;
    DisplayData *displayData = nullptr;
    this->_displayIndex = displayIndex;
    if (displayIndex >= 0)
    {
        displayData = slotData->displayDataList[displayIndex];
        
    }
    
    if (displayData)
    {
        origin = displayData->transform;
        Node* displayNode = nullptr;
        switch (displayData->type) {
            case DisplayType::DT_IMAGE:
            case DisplayType::DT_MESH:
                _display = DBSkin::create(displayData,textureAtlasName);
                displayNode = _display;
                break;
            case DisplayType::DT_ARMATURE:
                _childArmature = DBArmature::create(displayData->name/*,""*/,textureAtlasName);
                displayNode = _childArmature;
                break;
            default:
                break;
        }
        
        if (displayNode)
        {
            
            this->addChild(displayNode);
        }
    }
    
    return true;
}


DBSlot::DBSlot()
: _display (nullptr)
, _slotData (nullptr)
, _needUpdate (true)
, _isColorChanged (false)
, _displayIndex(-1)
, _originZOrder(0.f)
, _tweenZOrder(0.f)
//, _blendMode(BlendMode::BM_NORMAL)
, _childArmature(nullptr)
, _textureAtlasName ("")

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

    Point tweenPivot = parentBone->getTweenPivot();
    const float x = origin.x + offset.x + tweenPivot.x;
    const float y = origin.y + offset.y + tweenPivot.y;
    const Matrix &parentMatrix = parentBone->getGlobalTransformMatrix();
    globalTransformMatrix.tx = global.x = parentMatrix.a * x + parentMatrix.c * y + parentMatrix.tx;
    globalTransformMatrix.ty = global.y = parentMatrix.d * y + parentMatrix.b * x + parentMatrix.ty;
    
    if (inheritRotation)
    {
        global.skewX = origin.skewX + offset.skewX + parentBone->getGlobalTransform().skewX;
        global.skewY = origin.skewY + offset.skewY + parentBone->getGlobalTransform().skewY;
    }
    else
    {
        global.skewX = origin.skewX + offset.skewX;
        global.skewY = origin.skewY + offset.skewY;
    }
    
    if (inheritScale)
    {
        global.scaleX = origin.scaleX * offset.scaleX * parentBone->getGlobalTransform().scaleX;
        global.scaleY = origin.scaleY * offset.scaleY * parentBone->getGlobalTransform().scaleY;
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
    bool displayControl = animationState->displayControl /*&& animationState->containsBoneMask(this->getName())*/;
    if (displayControl)
    {
        SlotFrame *slotFrame = static_cast<SlotFrame*>(frame);
        const int displayIndex = slotFrame->displayIndex;
        
        changeDisplay(displayIndex);
        updateDisplayVisible(slotFrame->visible);
        if (displayIndex >= 0)
        {
            if (slotFrame->zOrder != _tweenZOrder)
            {
                this->setTweenZOrder(slotFrame->zOrder);
            }
        }
        
        if (!frame->action.empty())
        {
            if (_childArmature)
            {
                _childArmature->getAnimation()->gotoAndPlay(frame->action);
            }
        }
    }
}

void DBSlot::changeDisplay(int displayIndex)
{

    if (displayIndex < 0)
    {
        this->setVisible(false);
        if (this->_childArmature)
        {
            this->_childArmature->stopAllActions();
            this->_childArmature->unscheduleUpdate();
        }
    }
    else
    {
        
        displayIndex = displayIndex > this->_slotData->displayDataList.size() ? this->_slotData->displayDataList.size() - 1 : displayIndex;
        
        if (_displayIndex != displayIndex)
        {
            _displayIndex = displayIndex;
            updateSkinDisplay();
            
            if (
                _slotData &&
                !_slotData->displayDataList.empty() &&
                _displayIndex < (int)(_slotData->displayDataList.size())
                )
            {
                origin = _slotData->displayDataList[_displayIndex]->transform;
            }
        }
//        else if (!_isShowDisplay)
//        {
//            _isShowDisplay = true;
//            
//            if (_armature)
//            {
//                _armature->_slotsZOrderChanged = true;
//                addDisplayToContainer(_armature->_display, -1);
//            }
//            
//            updateChildArmatureAnimation();
//        }
        this->setVisible(true);
        
    }

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

void DBSlot::updateSkinDisplay()
{
    if (this->_display)
    {
        this->_display->setDisplayData(_slotData->displayDataList[_displayIndex]);
    }
}

void DBSlot::setTweenZOrder(float zOrder)
{
    this->_tweenZOrder = zOrder;
    this->setLocalZOrder(this->_originZOrder + this->_tweenZOrder);
}

MeshData* DBSlot::getMeshData(const std::string &name)
{
    if (_slotData && _displayIndex >= 0 && _displayIndex < _slotData->displayDataList.size())
    {
        return dynamic_cast<MeshData* >(_slotData->displayDataList[_displayIndex]);
    }
    return nullptr;
}

NAME_SPACE_DRAGON_BONES_END
