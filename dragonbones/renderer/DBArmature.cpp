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


NAME_SPACE_DRAGON_BONES_BEGIN

DBArmature::DBArmature()
: m_pDragonBonesData (nullptr)
, m_pArmatureData (nullptr)
, m_pAnimation (nullptr)
, m_pSkinData (nullptr)
{
    _boneDic.clear();
    _topBoneList.clear();
}


DBArmature::~DBArmature()
{
    CC_SAFE_RELEASE_NULL(m_pDragonBonesData);
    _boneDic.clear();
    _topBoneList.clear();
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

bool DBArmature::initWithName(const std::string &name)
{
    DragonBonesData* dragonBonesData = DBCCFactory::getInstance()->getDragonBonesData(name);
    if (!dragonBonesData)
    {
        return false;
    }
    m_pArmatureData = dragonBonesData->getArmatureData(name);
    if (!m_pArmatureData)
    {
        return false;
    }
    
    _name = name;
    DBAnimation* animation = DBAnimation::create();
    this->setAnimation(animation);
    animation->setAnimationDataList(m_pArmatureData->animationDataList);
    
    for (size_t i = 0, l = m_pArmatureData->boneDataList.size(); i < l; ++i)
    {
        createBone(m_pArmatureData->boneDataList.at(i));
    }

    
//    SkinData *skinData = nullptr;
//    
//    skinData = armatureData->getSkinData("");
//    if (skinData)
//    {
//        buildSlots(armature, armatureData, skinData, skinDataCopy);
//    }
    
    
    this->setCascadeColorEnabled(true);
    this->setCascadeOpacityEnabled(true);
    
#if 0
    
    ArmatureData *animationArmatureData = nullptr;
    
    SkinData *skinDataCopy = nullptr;
    
   
    Armature *armature = generateArmature(armatureData);
    armature->name = armatureName;
    
    if (animationArmatureData)
    {
        armature->getAnimation()->setAnimationDataList(animationArmatureData->animationDataList);
    }
    else
    {
        armature->getAnimation()->setAnimationDataList(armatureData->animationDataList);
    }
    
    //
    buildBones(armature, armatureData);
    
    //
    
    
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
    CC_SAFE_RELEASE_NULL(m_pDragonBonesData);
    m_pDragonBonesData = data;
    CC_SAFE_RETAIN(m_pDragonBonesData);
}

void DBArmature::setAnimation(dragonBones::DBAnimation *animation)
{
    CC_SAFE_RELEASE_NULL(m_pAnimation);
    m_pAnimation = animation;
    CC_SAFE_RETAIN(m_pAnimation);
}

DBAnimation* DBArmature::getAnimation()
{
    return this->m_pAnimation;
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
        BoneData* parentBoneData = this->m_pArmatureData->getBoneData(parentName);
        createBone(parentBoneData);
    }
    
    bone = DBBone::create(boneData);
    addBone(bone, parentName.c_str());
    
    return bone;
}

DBBone* DBArmature::getBone(const std::string& boneName)
{
    
    return nullptr;
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
    addChild(bone);
}

void DBArmature::createSkin()
{
    m_pSkinData = m_pArmatureData->getSkinData("");
    
    
    if (!m_pSkinData)
    {
        return;
    }
    m_pArmatureData->setSkinData(m_pSkinData->name);
    auto slotDataList = m_pArmatureData->slotDataList;
    for (size_t i = 0, l = slotDataList.size(); i < l; ++i)
    {
        SlotData *slotData = slotDataList[i];
        DBBone *bone = this->getBone(slotData->parent);
        if (!bone)
        {
            continue;
        }
        
        DBSlot* slot = DBSlot::create(slotData);
//        DBSlot *slot = generateSlot(slotData);
//        slot->name = slotData->name;
//        slot->_originZOrder = slotData->zOrder;
//        slot->_slotData = slotData;
//        bone->addSlot(slot);
        this->addChild(slot);
        
    }
#if 0
    //auto slotDataList = skinData->slotDataList;
    
    
    
        
            
#endif
}


NAME_SPACE_DRAGON_BONES_END