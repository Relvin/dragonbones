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
        
//        bone->getDisplayManager()->setCurrentDecorativeDisplay(nullptr);
        
        _childBones.eraseObject(bone);
    }
}

void DBBone::setArmature(DBArmature *armature)
{
    if (m_pArmature == armature)
        return;
    if (_armature)
    {
        
    }
    
    m_pArmature = armature;
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


NAME_SPACE_DRAGON_BONES_END