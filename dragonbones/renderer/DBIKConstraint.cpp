//
//  DBIKConstraint.cpp
//  dragonbones
//
//  Created by Relvin on 16/4/13.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBIKConstraint.h"
#include "objects/IKData.h"
#include "renderer/DBArmature.h"
#include "renderer/DBBone.h"

NAME_SPACE_DRAGON_BONES_BEGIN

DBIKConstraint* DBIKConstraint::create(IKData* ikData,DBArmature* armature)
{
    DBIKConstraint *ikConstraint = new (std::nothrow) DBIKConstraint();
    if (ikConstraint && ikConstraint->initWithIKDataAndArmature(ikData,armature))
    {
        ikConstraint->autorelease();
        return ikConstraint;
    }
    CC_SAFE_DELETE(ikConstraint);
    return nullptr;
}

DBIKConstraint::DBIKConstraint()
: _ikdata(nullptr)
, _armature(nullptr)
, _target(nullptr)
, _bendDirection(0)
, _weight(0.f)
, _animationCacheBend(0)
, _animationCacheWeight(-1.f)
{
    _bones.clear();
}


DBIKConstraint::~DBIKConstraint()
{
    _bones.clear();
}

bool DBIKConstraint::initWithIKDataAndArmature(IKData* ikData,DBArmature* armature)
{
    this->_ikdata = ikData;
    this->_armature = armature;
				
    _weight = ikData->weight;
    _bendDirection = (ikData->bendPositive?1:-1);

    DBBone* bone = nullptr;
    if(ikData->chain){
        bone = dynamic_cast<DBBone*>(armature->getBone(ikData->bone)->getParentBone());
        bone->setIsIKConstraint(true);
        _bones.pushBack(bone);
    }
    bone = armature->getBone(ikData->bone);
    bone->setIsIKConstraint(true);
    _bones.pushBack(bone);
    _target = armature->getBone(ikData->target);
    
    return true;
}

void DBIKConstraint::dispose()
{
    
}

void DBIKConstraint::compute()
{
    switch (_bones.size()) {
        case 1:
        {
            float weig1 = _animationCacheWeight >= 0 ? _animationCacheWeight : _weight;
            compute1(_bones.at(0), _target, weig1);
            break;
        }
        case 2:
        {
            int bend = _animationCacheBend != 0 ? _animationCacheBend : _bendDirection;
            float weig = _animationCacheWeight >= 0 ? _animationCacheWeight : _weight;
            Point tt = compute2(_bones.at(0),_bones.at(1),_target->getGlobalTransform().x,_target->getGlobalTransform().y, bend, weig);
            float b0Rotation = _bones.at(0)->getOriginTransform().getRotation();
            DBBone* b0Parent = dynamic_cast<DBBone *>(_bones.at(0)->getParentBone());
            float rotationIK0 = b0Rotation+(tt.x-b0Rotation)*weig+b0Parent->getRotationIK();
            
            _bones.at(0)->setRotationIK(rotationIK0);
            
            float b1Rotation = _bones.at(1)->getOriginTransform().getRotation();
            float rotationIK1 = b1Rotation+(tt.y-b1Rotation)*weig+rotationIK0;
            _bones.at(1)->setRotationIK(rotationIK1);
            break;
        }
            
    }
}

void DBIKConstraint::compute1(DBBone* bone,DBBone* target,float weightA)
{
    const DBBone* parentBone = dynamic_cast<DBBone* >(bone->getParentBone());
    
    float parentRotation = (!bone->getInheritRotation() || parentBone == nullptr) ? 0 : parentBone->getGlobalTransform().getRotation();
    float rotation = parentBone->getGlobalTransform().getRotation();
    float rotationIK = atan2(target->getGlobalTransform().y - bone->getGlobalTransform().y, target->getGlobalTransform().x - bone->getGlobalTransform().x);
    bone->setRotationIK(rotation + (rotationIK - rotation) * weightA);
}

Point DBIKConstraint::compute2(DBBone* parent,DBBone* child,float targetX,float targetY,int benDirection,float weightA)
{
    Transform parentGlobal = parent->getGlobalTransform();
    Transform childGlobal = child->getGlobalTransform();

   
    if (weightA == 0) {
        return Point(parentGlobal.getRotation(),childGlobal.getRotation());
    }
    
    
    Point tt;
    
    Point p1(parentGlobal.x,parentGlobal.y);
    
   
    Point p2(childGlobal.x,childGlobal.y);
    
    
    Matrix matrix ;
    DBBone *ppBone = nullptr ;
    if (parent->getParentBone())
    {
        ppBone = dynamic_cast<DBBone *>(parent->getParentBone());
    }
    
    if (ppBone)
    {
        ppBone->getGlobalTransform().toMatrix(matrix);
    }
    
    Matrix tempMatrix = matrix;
    tempMatrix.invert();
    Point targetPoint ;
    tempMatrix.transformPoint(targetPoint,targetX,targetY,true);
    targetX = targetPoint.x;
    targetY = targetPoint.y;
    tempMatrix.transformPoint(p1,p1.x,p1.y,true);
    tempMatrix.transformPoint(p2,p2.x,p2.y,true);
    
    
    Transform parentOrigin = parent->getOriginTransform();
    Transform childOrigin = child->getOriginTransform();
    float psx = parentOrigin.scaleX;
    float psy = parentOrigin.scaleY;
    float csx = childOrigin.scaleX;
    
    float childX = p2.x-p1.x;
    float childY = p2.y-p1.y;
    
    float len1 = sqrt(childX * childX + childY* childY);
    float parentAngle;
    float childAngle;
    int sign = 1;
    float offset1 = 0;
    float offset2 = 0;
    if (psx < 0) {
        psx = -psx;
        offset1 = PI;
        sign = -1;
    } else {
        offset1 = 0;
        sign = 1;
    }
    if (psy < 0) {
        psy = -psy;
        sign = -sign;
    }
    if (csx < 0) {
        csx = -csx;
        offset2 = PI;
    } else{
        offset2 = 0;
    }
    _bendDirection = sign* _bendDirection;
    
    
    if (abs(psx - psy) <= 0.001)
    {
        
        float childlength = child->getLength();
        float len2 = childlength * csx;
        targetX = targetX - p1.x;
        targetY = targetY - p1.y;
        float cosDenom = 2 * len1 * len2;
        float cos = (targetX * targetX + targetY * targetY - len1 * len1 - len2 * len2) / cosDenom;
        
        if (cos < -1)
            cos = -1;
        else if (cos > 1)
            cos = 1;
        
        childAngle = acos(cos) * _bendDirection;//o2
        float adjacent = len1 + len2 * cos;  //ae
        float opposite = len2 * sin(childAngle);//be
        parentAngle = atan2(targetY * adjacent - targetX * opposite, targetX * adjacent + targetY * opposite);//o1
    }
    else
    {
        do {
            float l1 = len1;
            float tx = targetX-p1.x;
            float ty = targetY-p1.y;
            float l2 = child->getLength() * childOrigin.scaleX;//child.currentLocalTransform.scaleX;
            float a = psx * l2;
            float b = psy * l2;
            float ta = atan2(ty, tx);
            float aa = a * a;
            float bb = b * b;
            float ll = l1 * l1;
            float dd = tx * tx + ty * ty;
            float c0 = bb * ll + aa * dd - aa * bb;
            float c1 = -2 * bb * l1;
            float c2 = bb - aa;
            float d = c1 * c1 - 4 * c2 * c0;
            if (d >= 0) {
                float q = sqrt(d);
                if (c1 < 0) q = -q;
                q = -(c1 + q) / 2;
                float r0 = q / c2;
                float r1 = c0 / q;
                float r = abs(r0) < abs(r1) ? r0 : r1;
                if (r * r <= dd) {
                    float y1 = sqrt(dd - r * r) * _bendDirection;
                    parentAngle = ta - atan2(y1, r);
                    childAngle = atan2(y1 / psy, (r - l1) / psx);
                    break;
                }
            }
            float minAngle = 0;
            float minDist = FLT_MAX;//Number.MAX_VALUE;
            float minX = 0;
            float minY = 0;
            float maxAngle = 0;
            float maxDist = 0;
            float maxX = 0;
            float maxY = 0;
            float x2 = l1 + a;
            float dist = x2 * x2;
            if (dist > maxDist) {
                maxAngle = 0;
                maxDist = dist;
                maxX = x2;
            }
            x2 = l1 - a;
            dist = x2 * x2;
            if (dist < minDist) {
                minAngle = PI;
                minDist = dist;
                minX = x2;
            }
            float angle1 = acos(-a * l1 / (aa - bb));
            x2 = a * cos(angle1) + l1;
            float y2 = b * sin(angle1);
            dist = x2 * x2 + y2 * y2;
            if (dist < minDist) {
                minAngle = angle1;
                minDist = dist;
                minX = x2;
                minY = y2;
            }
            if (dist > maxDist) {
                maxAngle = angle1;
                maxDist = dist;
                maxX = x2;
                maxY = y2;
            }
            if (dd <= (minDist + maxDist) / 2) {
                parentAngle = ta - atan2(minY * _bendDirection, minX);
                childAngle = minAngle * _bendDirection;
            } else {
                parentAngle = ta - atan2(maxY * _bendDirection, maxX);
                childAngle = maxAngle * _bendDirection;
            }
        }
        while (0);
        
    }
    float cx = childOrigin.x;
    float cy = childOrigin.y*psy;
    float initalRotation = atan2(cy, cx)*sign;
    tt.x = parentAngle - initalRotation + offset1;
    tt.y = (childAngle+initalRotation) * sign + offset2;
    normalize(tt.x);
    normalize(tt.y);
    return tt;
}


float DBIKConstraint::normalize(float rotation)
{
    if (rotation > PI)
        rotation -= PI*2;
    else if (rotation < -PI)
        rotation += PI*2;
    return rotation;
}


NAME_SPACE_DRAGON_BONES_END