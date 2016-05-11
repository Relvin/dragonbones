//
//  DBIKConstraint.hpp
//  dragonbones
//
//  Created by Relvin on 16/4/13.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBIKConstraint_hpp
#define DBIKConstraint_hpp
#include "DragonBones.h"
#include "base/CCRef.h"
#include "base/CCVector.h"
#include "geoms/Point.h"
NAME_SPACE_DRAGON_BONES_BEGIN

class IKData;
class DBArmature;
class DBBone;

class DBIKConstraint
: public cocos2d::Ref
{
public:
    static DBIKConstraint* create(IKData* ikData,DBArmature* armature);
    DBIKConstraint();
    virtual ~DBIKConstraint();
    virtual bool initWithIKDataAndArmature(IKData* ikData,DBArmature* armature);
    
    void dispose();
    
    void compute();
    void compute1(DBBone* bone,DBBone* target,float weight);
    Point compute2(DBBone* bone,DBBone* child,float targetX,float targetY,int benDirection,float weightA);
    float normalize(float rotation);
    DBBone* getBoneByIndex(int index) {
        if (index >= 0 && index < _bones.size())
        {
            return this->_bones.at(index);
        }
        return nullptr;
    };
    
    inline DBBone* getTargetBone() {return this->_target;};
    inline int getBoneSize() {return this->_bones.size();};
private:
    
    IKData* _ikdata;
    DBArmature* _armature;
    
    cocos2d::Vector<DBBone*> _bones;
    DBBone* _target;
    int _bendDirection;
    float _weight;
    int _animationCacheBend;
    float _animationCacheWeight;
    
    
};



NAME_SPACE_DRAGON_BONES_END

#endif /* DBIKConstraint_hpp */
