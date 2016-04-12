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
#include "events/DBEventData.h"
#include "animation/DBAnimationState.h"


NAME_SPACE_DRAGON_BONES_BEGIN

DBArmature::DBArmature()
: _pDragonBonesData (nullptr)
, _pArmatureData (nullptr)
, _pAnimation (nullptr)
, _pSkinData (nullptr)
, _eventDataMgr (nullptr)
{
    _boneDic.clear();
    _topBoneList.clear();
    _slotDic.clear();
}


DBArmature::~DBArmature()
{
    CC_SAFE_RELEASE_NULL(_pDragonBonesData);
    CC_SAFE_RELEASE_NULL(_eventDataMgr);
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
    CC_SAFE_RELEASE_NULL(_eventDataMgr);
    _eventDataMgr = DBEventDataMgr::create();
    CC_SAFE_RETAIN(_eventDataMgr);
    
    DragonBonesData* dragonBonesData = DBCCFactory::getInstance()->getDragonBonesData(name);
    if (!dragonBonesData)
    {
        return false;
    }
    _pArmatureData = dragonBonesData->getArmatureData(name);
    if (!_pArmatureData)
    {
        _pArmatureData = dragonBonesData->getArmatureDataFirst();
        if (!_pArmatureData)
        {
            return false;
        }
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
    
    
    // update armature pose
    getAnimation()->play();
    update(0);
    getAnimation()->stop();
    
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
        if (slot)
        {
            this->addChild(slot,slotData->zOrder);
            slot->setParentBone(bone);
            slot->setArmature(this);
            _slotDic.insert(slotData->name,slot);
            bone->addSlot(slot);
        }
        
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
    
    if (!_eventDataMgr->empty())
    {
        auto dataList = _eventDataMgr->getAllData();
        for (size_t i = 0, l = dataList.size(); i < l; ++i)
        {
//            _eventDispatcher->dispatchEvent(_eventDataList[i]);
        }
        _eventDataMgr->removeAllEventData();
        
    }

}

void DBArmature::onEnter()
{
    Node::onEnter();
    this->scheduleUpdate();
}

void DBArmature::arriveAtFrame(Frame *frame, DBAnimationState *animationState, bool isCross)
{

    if (!frame->event.empty())
    {
        DBEventData *eventData = _eventDataMgr->getUnusedEventData();
        eventData->setType(DBEventData::EventType::ANIMATION_FRAME_EVENT);
        eventData->setArmature(this);
        eventData->setAnimationState(animationState);
        eventData->setFrameLabel(frame->event);
        eventData->setFrame(frame);
    }
    
//    if (!frame->sound.empty() && soundEventDispatcher && soundEventDispatcher->hasEvent(EventData::EventType::SOUND))
//    {
//        EventData *eventData = EventData::borrowObject(EventData::EventType::SOUND);
//        eventData->armature = this;
//        eventData->animationState = animationState;
//        eventData->sound = frame->sound;
//        soundEventDispatcher->dispatchEvent(eventData);
//        EventData::returnObject(eventData);
//    }
    
    if (!frame->action.empty())
    {
        if (animationState->displayControl)
        {
            _pAnimation->gotoAndPlay(frame->action);
        }
    }

}

DBEventDataMgr* DBArmature::getEventDataManager() const
{
    return this->_eventDataMgr;
}



NAME_SPACE_DRAGON_BONES_END