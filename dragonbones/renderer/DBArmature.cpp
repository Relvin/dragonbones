//
//  DBArmature.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/3.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBArmature.h"
#include "DBCCFactory.h"
#include "objects/SkinData.h"
#include "DBSlot.h"
#include "objects/Frame.h"


NAME_SPACE_DRAGON_BONES_BEGIN

DBArmature::DBArmature()
: _pDragonBonesData (nullptr)
, _pArmatureData (nullptr)
, _pAnimation (nullptr)
, _pSkinData (nullptr)
{
    _boneDic.clear();
    _topBoneList.clear();
    _slotDic.clear();
}


DBArmature::~DBArmature()
{
    CC_SAFE_RELEASE_NULL(_pDragonBonesData);
    _boneDic.clear();
    _topBoneList.clear();
    _slotDic.clear();
}

DBArmature* DBArmature::create(const std::string &dragonBonesName)
{
    DBArmature* armature = new (std::nothrow) DBArmature();
    if (armature && armature->initWithName(dragonBonesName))
    {
        armature->autorelease();
        return armature;
    }
    CC_SAFE_DELETE(armature);
    return nullptr;
}

DBArmature* DBArmature::create(const std::string &dragonBonesName,const std::string &texture)
{
    DBArmature* armature = new (std::nothrow) DBArmature();
    if (armature && armature->initWithName(dragonBonesName,texture))
    {
        armature->autorelease();
        return armature;
    }
    CC_SAFE_DELETE(armature);
    return nullptr;
}


bool DBArmature::initWithName(const std::string &name)
{
    return this->initWithName(name, name);
}

bool DBArmature::initWithName(const std::string &name,const std::string &textureName)
{
    DragonBonesData* dragonBonesData = DBCCFactory::getInstance()->getDragonBonesData(name);
    if (!dragonBonesData)
    {
        return false;
    }
    _pArmatureData = dragonBonesData->getArmatureData(name);
    if (!_pArmatureData)
    {
        return false;
    }
    
    _name = name;
    DBAnimation* animation = DBAnimation::create(this);
    this->setAnimation(animation);
    animation->setAnimationDataList(_pArmatureData->animationDataList);
    
    for (size_t i = 0, l = _pArmatureData->boneDataList.size(); i < l; ++i)
    {
        createBone(_pArmatureData->boneDataList.at(i));
    }

    createSkin(textureName);
    
    
    this->setCascadeColorEnabled(true);
    this->setCascadeOpacityEnabled(true);
    
#if 0
    
    
    // update armature pose
    armature->getAnimation()->play();
    armature->advanceTime(0);
    armature->getAnimation()->stop();
    
#endif
    
    dragonBonesData->retain();
    return true;
}

void DBArmature::setDragonBonesData(dragonBones::DragonBonesData *data)
{
    CC_SAFE_RELEASE_NULL(_pDragonBonesData);
    _pDragonBonesData = data;
    CC_SAFE_RETAIN(_pDragonBonesData);
}

void DBArmature::setAnimation(dragonBones::DBAnimation *animation)
{
    CC_SAFE_RELEASE_NULL(_pAnimation);
    _pAnimation = animation;
    CC_SAFE_RETAIN(_pAnimation);
}

DBAnimation* DBArmature::getAnimation()
{
    return this->_pAnimation;
}

DBBone* DBArmature::createBone(BoneData* boneData)
{
    std::string boneName = boneData->name;
    DBBone *existedBone = getBone(boneName);
    if(existedBone != nullptr)
        return existedBone;
    
    std::string parentName = boneData->parent;
    
    DBBone *bone = nullptr;
    
    if( !parentName.empty() && !this->getBone(parentName))
    {
        BoneData* parentBoneData = this->_pArmatureData->getBoneData(parentName);
        createBone(parentBoneData);
    }
    
    bone = DBBone::create(boneData);
    addBone(bone, parentName.c_str());
    
    return bone;
}

DBBone* DBArmature::getBone(const std::string& boneName)
{
    
    return _boneDic.at(boneName);
}

DBSlot* DBArmature::getSlot(const std::string& slotName)
{
    return _slotDic.at(slotName);
}

void DBArmature::addBone(DBBone *bone, const std::string& parentName)
{
    CCASSERT( bone != nullptr, "Argument must be non-nil");
    CCASSERT(_boneDic.at(bone->getName()) == nullptr, "bone already added. It can't be added again");
    
    if (!parentName.empty())
    {
        DBBone *boneParent = _boneDic.at(parentName);
        if (boneParent)
        {
            boneParent->addChildBone(bone);
        }
        else
        {
            _topBoneList.pushBack(bone);
        }
    }
    else
    {
        _topBoneList.pushBack(bone);
    }
    
    bone->setArmature(this);
    
    _boneDic.insert(bone->getName(), bone);
    this->addChild(bone);
}

void DBArmature::createSkin(const std::string &textureName)
{
    _pSkinData = _pArmatureData->getSkinData("");
    
    
    if (!_pSkinData)
    {
        return;
    }
    _pArmatureData->setSkinData(_pSkinData->name);
    auto slotDataList = _pArmatureData->slotDataList;
    for (size_t i = 0, l = slotDataList.size(); i < l; ++i)
    {
        SlotData *slotData = slotDataList[i];
        DBBone *bone = this->getBone(slotData->parent);
        if (!bone)
        {
            continue;
        }
        
        DBSlot* slot = DBSlot::create(slotData,textureName);
        this->addChild(slot,slotData->zOrder);
        slot->setParentBone(bone);
        _slotDic.insert(slotData->name,slot);
    }
}

void DBArmature::update(float delta)
{
    _pAnimation->advanceTime(delta);
    delta *= _pAnimation->_timeScale;
    const bool isFading = _pAnimation->_isFading;
    
    for(const auto &bone : _topBoneList) {
        bone->update(delta);
    }
    
    for (auto& element : _slotDic)
    {
        element.second->update(delta);
        
        //        if (slot->_isShowDisplay && slot->_childArmature)
        //        {
        //            slot->_childArmature->advanceTime(passedTime);
        //        }
    }
    //
    //    if (_slotsZOrderChanged)
    //    {
    //        sortSlotsByZOrder();
    //        
    //    }

}

void DBArmature::onEnter()
{
    Node::onEnter();
    this->scheduleUpdate();
}

void DBArmature::arriveAtFrame(Frame *frame, DBAnimationState *animationState, bool isCross)
{
#if 0
    if (!frame->event.empty() && _eventDispatcher->hasEvent(EventData::EventType::ANIMATION_FRAME_EVENT))
    {
        EventData *eventData = EventData::borrowObject(EventData::EventType::ANIMATION_FRAME_EVENT);
        eventData->armature = this;
        eventData->animationState = animationState;
        eventData->frameLabel = frame->event;
        eventData->frame = frame;
        _eventDataList.push_back(eventData);
    }
    
    if (!frame->sound.empty() && soundEventDispatcher && soundEventDispatcher->hasEvent(EventData::EventType::SOUND))
    {
        EventData *eventData = EventData::borrowObject(EventData::EventType::SOUND);
        eventData->armature = this;
        eventData->animationState = animationState;
        eventData->sound = frame->sound;
        soundEventDispatcher->dispatchEvent(eventData);
        EventData::returnObject(eventData);
    }
    
    if (!frame->action.empty())
    {
        if (animationState->displayControl)
        {
            _animation->gotoAndPlay(frame->action);
        }
    }
#endif
}



NAME_SPACE_DRAGON_BONES_END