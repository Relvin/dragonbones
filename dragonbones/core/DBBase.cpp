//
//  DBBase.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/14.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBBase.h"
//#include "Armature.h"
//#include "core/Bone.h"

NAME_SPACE_DRAGON_BONES_BEGIN

DBBase::DBBase() :
inheritRotation(true)
,inheritScale(true)
,inheritTranslation(true)
,userData(nullptr)
,_visible(true)
,_armature(nullptr)
,_parentBone(nullptr)
{
    offset.scaleX = offset.scaleY = 1.f;
}

DBBase::~DBBase()
{
    dispose();
}

void DBBase::dispose()
{
    _armature = nullptr;
    _parentBone = nullptr;
    
    if (userData)
    {
        delete userData;
        userData = nullptr;
    }
}

bool DBBase::getBoneVisible() const
{
    return _visible;
}

void DBBase::setBoneVisible(bool visible)
{
    _visible = visible;
}

cocos2d::Node* DBBase::getArmature() const
{
    return _armature;
}

cocos2d::Node* DBBase::getParentBone() const
{
    return _parentBone;
}

void DBBase::setArmature(cocos2d::Node *armature)
{
    _armature = armature;
}

void DBBase::setParentBone(cocos2d::Node *bone)
{
    _parentBone = bone;
}

void DBBase::calculateRelativeParentTransform()
{
}

void DBBase::calculateParentTransform( Transform &transform, Matrix &matrix )
{

}

void DBBase::updateGlobal( Transform &transform, Matrix &matrix )
{
    calculateRelativeParentTransform();
    
    // 计算parent bone的绝对坐标
    calculateParentTransform(transform, matrix);
    
    // 计算绝对坐标
    float x = global.x;
    float y = global.y;
    
    global.x = matrix.a * x + matrix.c * y + matrix.tx;
    global.y = matrix.d * y + matrix.b * x + matrix.ty;
    
    if (inheritRotation)
    {
        global.skewX += transform.skewX;
        global.skewY += transform.skewY;
    }
    if (inheritScale)
    {
        global.scaleX *= transform.scaleX;
        global.scaleY *= transform.scaleY;
    }
    global.toMatrix(globalTransformMatrix, true);
}


NAME_SPACE_DRAGON_BONES_END
