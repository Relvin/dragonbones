//
//  DBBone.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBBone.h"
#include "DBArmature.h"
NAME_SPACE_DRAGON_BONES_BEGIN

DBBone* DBBone::create(BoneData* boneData)
{
    DBBone* bone = new (std::nothrow) DBBone();
    if (bone && bone->init())
    {
        bone->autorelease();
        return bone;
    }
    CC_SAFE_DELETE(bone);
    return nullptr;
}

bool DBBone::initWithBoneData(BoneData* boneData)
{
    name = boneData->name;
    this->inheritRotation = boneData->inheritRotation;
    this->inheritScale = boneData->inheritScale;
    origin = boneData->transform;
    
    return true;
}

DBBone::DBBone()
: m_boneData (nullptr)
, m_pArmature (nullptr)
{
    
}


DBBone::~DBBone()
{
    
}


void DBBone::addChildBone(DBBone *child)
{
    CCASSERT( nullptr != child, "Argument must be non-nil");
    CCASSERT( nullptr == child->_parentBone, "child already added. It can't be added again");
    
    if(_children.empty())
    {
        _children.reserve(4);
    }
    
    if (_children.getIndex(child) == cocos2d::CC_INVALID_INDEX)
    {
        _children.pushBack(child);
//        child->setParentBone(this);
    }
}

void DBBone::removeChildBone(DBBone *bone, bool recursion)
{
    if (!_children.empty() && _children.getIndex(bone) != cocos2d::CC_INVALID_INDEX )
    {
        if(recursion)
        {
            auto ccbones = bone->_children;
            
            for(auto& object : ccbones)
            {
                DBBone *ccBone = static_cast<DBBone*>(object);
                bone->removeChildBone(ccBone, recursion);
            }
        }
        
        bone->setParentBone(nullptr);
        
//        bone->getDisplayManager()->setCurrentDecorativeDisplay(nullptr);
        
        _children.eraseObject(bone);
    }
}

void DBBone::setArmature(DBArmature *armature)
{
    if (m_pArmature == armature) return;
    if (_armature)
    {
        
    }
    
    m_pArmature = armature;
}



NAME_SPACE_DRAGON_BONES_END