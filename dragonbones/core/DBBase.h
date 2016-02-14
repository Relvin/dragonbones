//
//  DBBase.hpp
//  dragonbones
//
//  Created by Relvin on 16/2/14.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBBase_hpp
#define DBBase_hpp

#include "DragonBones.h"
#include "geoms/Matrix.h"
#include "geoms/Transform.h"
#include "2d/CCNode.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class DBBase
{
public:
    DBBase();
    virtual ~DBBase();
    virtual void dispose();
    
    virtual bool getBoneVisible() const;
    virtual void setBoneVisible(bool vislble);
    
    virtual cocos2d::Node* getArmature() const;
    virtual cocos2d::Node* getParentBone() const;
    
protected:
    virtual void setArmature(cocos2d::Node *armature);
    virtual void setParentBone(cocos2d::Node *bone);
    // change
    virtual void calculateRelativeParentTransform();
    virtual void calculateParentTransform(Transform &transform, Matrix &matrix);
    virtual void updateGlobal(Transform &transform, Matrix &matrix);
    
public:
    bool inheritRotation;
    bool inheritScale;
    bool inheritTranslation;
    
    std::string name;
    Transform global;
    Transform origin;
    Transform offset;
    Matrix globalTransformMatrix;
    
    void *userData;
    
    
    
protected:
    bool _visible;
    
    cocos2d::Node *_armature;
    cocos2d::Node *_parentBone;
    
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBBase);
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBBase_hpp */
