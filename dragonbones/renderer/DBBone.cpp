//
//  DBBone.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBBone.h"
#include "DBArmature.h"
#include "DBSlot.h"
#include "animation/DBAnimationState.h"
#include "events/DBEventData.h"
NAME_SPACE_DRAGON_BONES_BEGIN

DBBone* DBBone::create(BoneData* boneData)
{
    DBBone* bone = new (std::nothrow) DBBone();
    if (bone && bone->initWithBoneData(boneData))
    {
        bone->autorelease();
        return bone;
    }
    CC_SAFE_DELETE(bone);
    return nullptr;
}

bool DBBone::initWithBoneData(BoneData* boneData)
{
    if (!Node::init())
    {
        return false;
    }
    _name = boneData->name;
    this->inheritRotation = boneData->inheritRotation;
    this->inheritScale = boneData->inheritScale;
    this->_length = boneData->length;
    origin = boneData->transform;
    _boneData = boneData;
    
    return true;
}

DBBone::DBBone()
: _boneData (nullptr)
, _pArmature (nullptr)
, displayController("")
,applyOffsetTranslationToChild(true)
,applyOffsetRotationToChild(true)
,applyOffsetScaleToChild(false)
,_isColorChanged(false)
,_needUpdate(2)
,_tween()
,_length(0.f)
,_rotationIK(0.f)
,_isIKConstraint(false)
{

    inheritScale = false;
    _timelineStateList.clear();
    _childBones.clear();
    _slotList.clear();
}


DBBone::~DBBone()
{
    _timelineStateList.clear();
    _childBones.clear();
    _slotList.clear();
}


void DBBone::addChildBone(DBBone *child)
{
    CCASSERT( nullptr != child, "Argument must be non-nil");
    CCASSERT( nullptr == child->_parentBone, "child already added. It can't be added again");
    //need rewrite (not use _childern)
    if(_childBones.empty())
    {
        _childBones.reserve(4);
    }
    
    if (_childBones.getIndex(child) == cocos2d::CC_INVALID_INDEX)
    {
        _childBones.pushBack(child);
        child->setParentBone(this);
    }
}

void DBBone::removeChildBone(DBBone *bone, bool recursion)
{
    if (!_childBones.empty() && _childBones.getIndex(bone) != cocos2d::CC_INVALID_INDEX )
    {
        if(recursion)
        {
            auto ccbones = bone->_childBones;
            
            for(auto& object : ccbones)
            {
                DBBone *ccBone = static_cast<DBBone*>(object);
                bone->removeChildBone(ccBone, recursion);
            }
        }
        
        bone->setParentBone(nullptr);
        
        _childBones.eraseObject(bone);
    }
}


void DBBone::addSlot(DBSlot* slot)
{
    if (!slot)
    {
        return;
    }
    if(_slotList.empty())
    {
        _slotList.reserve(4);
    }
    
    if (_slotList.getIndex(slot) == cocos2d::CC_INVALID_INDEX)
    {
        _slotList.pushBack(slot);
        slot->setParentBone(this);
    }
    
}

void DBBone::removeSlot(DBSlot* slot)
{
    if (_slotList.empty() || _slotList.getIndex(slot) != cocos2d::CC_INVALID_INDEX)
    {
        return;
    }
    slot->setParentBone(nullptr);
    _slotList.eraseObject(slot);
}

void DBBone::setArmature(DBArmature *armature)
{
    if (_pArmature == armature)
        return;
    if (_armature)
    {
        
    }
    
    _pArmature = armature;
}

void DBBone::calculateParentTransform( Transform &transform, Matrix &matrix )
{
    DBBone* parentBone = dynamic_cast<DBBone* >(_parentBone);
    if (parentBone && (inheritTranslation || inheritRotation || inheritScale))
    {
        
        transform = parentBone->_globalTransformForChild;
        matrix = parentBone->_globalTransformMatrixForChild;

        if (!inheritTranslation || !inheritRotation || !inheritScale)
        {
            if (!inheritTranslation)
            {
                transform.x = 0.f;
                transform.y = 0.f;
            }
            if (!inheritScale)
            {
                transform.scaleX = 1.f;
                transform.scaleY = 1.f;
            }
            if (!inheritRotation)
            {
                transform.skewX = 0.f;
                transform.skewY = 0.f;
            }
            transform.toMatrix(matrix, true);
        }
    }
}

void DBBone::update(bool isFading)
{
    
    static int boneCount = 0;
    _needUpdate --;
    
    DBBone* parentBone = dynamic_cast<DBBone*>(_parentBone);
    if (isFading || _needUpdate > 0 || (parentBone && parentBone->_needUpdate > 0))
    {
        _needUpdate = 1;
    }
    else
    {
        return;
    }
    
    this->setPosition(global.x,-global.y);
    
//#define showBone 1
#ifdef showBone
    cocos2d::Sprite* boneSprite = dynamic_cast<cocos2d::Sprite*>(this->getChildByTag(1001));
    if (!boneSprite)
    {
        boneSprite = cocos2d::Sprite::create("res/bone.png");
        this->addChild(boneSprite);
        boneSprite->setGlobalZOrder(100);
        boneSprite->setTag(1001);
        boneSprite->setAnchorPoint(cocos2d::Vec2(0,0));
        boneSprite->setColor(cocos2d::Color3B(172,172,172));
        boneCount++;
        printf("bone count = %d\n",boneCount);
    }
    if(_boneData->length)
    {
        boneSprite->setScaleX(_boneData->length / 15.0f);
        
    }
    boneSprite->setRotationSkewX(global.skewX * RADIAN_TO_ANGLE);
    boneSprite->setRotationSkewY(global.skewY * RADIAN_TO_ANGLE);
#endif
    
    
    blendingTimeline();
    
    // 计算global
    Transform parentGlobalTransform;
    Matrix parentGlobalTransformMatrix;
    updateGlobal(parentGlobalTransform, parentGlobalTransformMatrix);
    
    // 计算globalForChild
    bool ifExistOffsetTranslation = offset.x != 0 || offset.y != 0;
    bool ifExistOffsetScale = offset.scaleX != 1 || offset.scaleY != 1;
    bool ifExistOffsetRotation = offset.skewX != 0 || offset.skewY != 0;
    
    if(	(!ifExistOffsetTranslation || applyOffsetTranslationToChild) &&
       (!ifExistOffsetScale || applyOffsetScaleToChild) &&
       (!ifExistOffsetRotation || applyOffsetRotationToChild))
    {
        _globalTransformForChild = global;
        _globalTransformMatrixForChild = _globalTransformMatrix;
    }
    else
    {
        
        _globalTransformForChild.x = origin.x + _tween.x;
        _globalTransformForChild.y = origin.y + _tween.y;
        _globalTransformForChild.scaleX = origin.scaleX * _tween.scaleX;
        _globalTransformForChild.scaleY = origin.scaleY * _tween.scaleY;
        _globalTransformForChild.skewX = origin.skewX + _tween.skewX;
        _globalTransformForChild.skewY = origin.skewY + _tween.skewY;
        
        if(applyOffsetTranslationToChild)
        {
            _globalTransformForChild.x += offset.x;
            _globalTransformForChild.y += offset.y;
        }
        if(applyOffsetScaleToChild)
        {
            _globalTransformForChild.scaleX *= offset.scaleX;
            _globalTransformForChild.scaleY *= offset.scaleY;
        }
        if(applyOffsetRotationToChild)
        {
            _globalTransformForChild.skewX += offset.skewX;
            _globalTransformForChild.skewY += offset.skewY;
        }
        
        _globalTransformForChild.toMatrix(_globalTransformMatrixForChild, true);
        
        Transform::matrixToTransform(
                                     _globalTransformMatrixForChild, 
                                     _globalTransformForChild,
                                     _globalTransformForChild.scaleX * parentGlobalTransform.scaleX >= 0,
                                     _globalTransformForChild.scaleY * parentGlobalTransform.scaleY >= 0);
    }
    for(const auto &obj: _childBones) {
        DBBone *childBone = static_cast<DBBone*>(obj);
        if (childBone)
        {
            childBone->update(isFading);
        }
    }
}

void DBBone::blendingTimeline()
{
    size_t i = _timelineStateList.size();
    if (i == 1)
    {
        DBTimelineState *timelineState = _timelineStateList.at(0);
        const Transform &transform = timelineState->getTransform();
        const Point &pivot = timelineState->getPivot();
        timelineState->setWeight(timelineState->getAnimationState()->getCurrentWeight());
        const float weight = timelineState->getWeight();
        _tween.x = transform.x * weight;
        _tween.y = transform.y * weight;
        _tween.skewX = transform.skewX * weight;
        _tween.skewY = transform.skewY * weight;
        _tween.scaleX = 1 + (transform.scaleX - 1) * weight;
        _tween.scaleY = 1 + (transform.scaleY - 1) * weight;
        _tweenPivot.x = pivot.x * weight;
        _tweenPivot.y = pivot.y * weight;
    }
    else if (i > 1)
    {
        int prevLayer = _timelineStateList.at(i -1)->getAnimationState()->getLayer();
        int currentLayer = 0;
        float weigthLeft = 1.f;
        float layerTotalWeight = 0.f;
        float x = 0.f;
        float y = 0.f;
        float skewX = 0.f;
        float skewY = 0.f;
        float scaleX = 1.f;
        float scaleY = 1.f;
        float pivotX = 0.f;
        float pivotY = 0.f;
        
        while (i--)
        {
            DBTimelineState *timelineState = _timelineStateList.at(i);
            currentLayer = timelineState->getAnimationState()->getLayer();
            
            if (prevLayer != currentLayer)
            {
                if (layerTotalWeight >= weigthLeft)
                {
                    timelineState->setWeight(0);
                    break;
                }
                else
                {
                    weigthLeft -= layerTotalWeight;
                }
            }
            
            prevLayer = currentLayer;
            timelineState->setWeight(timelineState->getAnimationState()->getCurrentWeight() * weigthLeft);
            const float weight = timelineState->getWeight();
            
            //timelineState
            if (weight && timelineState->getBlendEnabled())
            {
                const Transform &transform = timelineState->getTransform();
                const Point &pivot = timelineState->getPivot();
                x += transform.x * weight;
                y += transform.y * weight;
                skewX += transform.skewX * weight;
                skewY += transform.skewY * weight;
                scaleX += (transform.scaleX - 1) * weight;
                scaleY += (transform.scaleY - 1) * weight;
                pivotX += pivot.x * weight;
                pivotY += pivot.y * weight;
                layerTotalWeight += weight;
            }
        }
        
        _tween.x = x;
        _tween.y = y;
        _tween.skewX = skewX;
        _tween.skewY = skewY;
        _tween.scaleX = scaleX;
        _tween.scaleY = scaleY;
        _tweenPivot.x = pivotX;
        _tweenPivot.y = pivotY;
    }
}

const Point& DBBone::getTweenPivot() const
{
    return this->_tweenPivot;
}

int DBBone::getNeedUpdate()
{
    return this->_needUpdate;
}

void DBBone::calculateRelativeParentTransform()
{
    global.scaleX = origin.scaleX * _tween.scaleX * offset.scaleX;
    global.scaleY = origin.scaleY * _tween.scaleY * offset.scaleY;
    global.skewX = origin.skewX + _tween.skewX + offset.skewX;
    global.skewY = origin.skewY + _tween.skewY + offset.skewY;
    global.x = origin.x + _tween.x + offset.x;
    global.y = origin.y + _tween.y + offset.y;
}

// static method
bool DBBone::sortState(const DBTimelineState *a, const DBTimelineState *b)
{
    return a->getAnimationState()->getLayer() < b->getAnimationState()->getLayer();
}

void DBBone::addState(DBTimelineState *timelineState)
{
    auto iterator = std::find(_timelineStateList.cbegin(), _timelineStateList.cend(), timelineState);
    
    if (iterator == _timelineStateList.cend())
    {
        _timelineStateList.pushBack(timelineState);
        std::sort(_timelineStateList.begin() , _timelineStateList.end() , sortState);
    }
}

void DBBone::removeState(DBTimelineState *timelineState)
{
    auto iterator = std::find(_timelineStateList.begin(), _timelineStateList.end(), timelineState);
    
    if (iterator != _timelineStateList.end())
    {
        _timelineStateList.erase(iterator);
    }
}

void DBBone::invalidUpdate()
{
    _needUpdate = 2;
}


void DBBone::arriveAtFrame(TransformFrame *frame, const DBTimelineState *timelineState, DBAnimationState *animationState, bool isCross)
{

    bool displayControl =
    animationState->displayControl &&
    (displayController.empty() || displayController == animationState->name)/* &&
    animationState->getMixingTransform(this->getName())*/;
    
    // && timelineState->_weight > 0
    // TODO: 需要修正混合动画干扰关键帧数据的问题，如何正确高效的判断混合动画？
    if (displayControl)
    {
        
        if (!frame->event.empty())
        {
            
            DBEventData *eventData = _pArmature->getEventDataManager()->getUnusedEventData();
            eventData->setType(DBEventData::EventType::BONE_FRAME_EVENT);
            eventData->setArmature(_pArmature);
            eventData->setBone(this);
            eventData->setAnimationState(animationState);
            eventData->setFrameLabel(frame->event);
            eventData->setFrame(frame);
            
        }
        
//        if (!frame->sound.empty() && Armature::soundEventDispatcher && Armature::soundEventDispatcher->hasEvent(EventData::EventType::SOUND))
//        {
//            EventData *eventData = EventData::borrowObject(EventData::EventType::SOUND);
//            eventData->armature = _pArmature;
//            eventData->bone = this;
//            eventData->animationState = animationState;
//            eventData->sound = frame->sound;
//            Armature::soundEventDispatcher->dispatchEvent(eventData);
//            EventData::returnObject(eventData);
//        }
        
//        if (!frame->action.empty())
//        {
//            for (size_t i = 0, l = _slotList.size(); i < l; ++i)
//            {
//                if (_slotList[i]->_childArmature)
//                {
//                    _slotList[i]->_childArmature->_animation->gotoAndPlay(frame->action);
//                }
//            }
//        }
    }

}

void DBBone::adjustGlobalTransformMatrixByIK()
{
    DBBone* parentBone = dynamic_cast<DBBone*>(_parentBone);
    if(!parentBone)
    {
        return;
    }
    
    global.setRotation(this->_rotationIK);
    global.toMatrix(_globalTransformMatrix,true);
    _globalTransformForChild.setRotation(_rotationIK);
    _globalTransformForChild.toMatrix( _globalTransformMatrixForChild,true);
}

void DBBone::hideSlots()
{
    for (auto slot : _slotList)
    {
        slot->changeDisplay(-1);
    }
}

void DBBone::setRotationIK(float rotationIK)
{
    this->_rotationIK = rotationIK;
}

void DBBone::setIsIKConstraint(bool isIKConstraint)
{
    this->_isIKConstraint = isIKConstraint;
}

NAME_SPACE_DRAGON_BONES_END