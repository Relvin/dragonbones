//
//  DBBase.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/14.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBBase.h"
#include "renderer/DBArmature.h"
#include "renderer/DBBone.h"

NAME_SPACE_DRAGON_BONES_BEGIN

DBBase::DBBase() :
inheritRotation(true)
,inheritScale(true)
,inheritTranslation(true)
,userData(nullptr)
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

DBArmature* DBBase::getArmature() const
{
    return _armature;
}

DBBone* DBBase::getParentBone() const
{
    return _parentBone;
}

void DBBase::setArmature(DBArmature *armature)
{
    _armature = armature;
}

void DBBase::setParentBone(DBBone *bone)
{
    _parentBone = bone;
}

void DBBase::calculateRelativeParentTransform()
{
}

void DBBase::calculateParentTransform( Transform &transform, Matrix &matrix )
{
    if (_parentBone && (inheritTranslation || inheritRotation || inheritScale))
    {
        transform = _parentBone->getGlobalTransformForChild();
        matrix = _parentBone->getGlobalTransformMatrixForChild();
    }
    else
    {
        global.toMatrix(_globalTransformMatrix,true);
    }
}

void DBBase::updateGlobal( Transform &transform, Matrix &matrix )
{
    calculateRelativeParentTransform();
    
    
    calculateParentTransform(transform, matrix);
    
    
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
    global.toMatrix(_globalTransformMatrix, true);
}

bool DBBase::getInheritRotation()
{
    return this->inheritRotation;
}

void DBBase::setInheritRotation(bool inheritRotation)
{
    this->inheritRotation = inheritRotation;
}

bool DBBase::getInheritScale()
{
    return this->inheritScale;
}
void DBBase::setInheritScale(bool inheritScale)
{
    this->inheritScale = inheritScale;
}

bool DBBase::getInheritTranslation()
{
    return this->inheritTranslation;
}

void DBBase::setInheritTranslation(bool inheritTranslation)
{
    this->inheritTranslation = inheritTranslation;
}

Transform& DBBase::getGlobalTransform() const
{
    return this->global;
}
void DBBase::setGlobalTransform(const Transform& global)
{
    this->global = global;
}

Transform& DBBase::getOriginTransform() const
{
    return this->origin;
}
void DBBase::setOriginTransform(const Transform& origin)
{
    this->origin = origin;
}

const Transform& DBBase::getOffsetTransform() const
{
    return this->offset;
}

void DBBase::setOffsetTransform(const Transform& offset)
{
    this->offset = offset;
}

const Matrix& DBBase::getGlobalTransformMatrix() const
{
    return this->_globalTransformMatrix;
}

void DBBase::setGlobalTransformMatrix(const Matrix& matrix)
{
    this->_globalTransformMatrix = matrix;
}


NAME_SPACE_DRAGON_BONES_END
