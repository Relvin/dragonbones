//
//  DBArmature.h
//  dragonbones
//
//  Created by Relvin on 16/2/3.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBArmature_h
#define DBArmature_h

#include "DragonBones.h"
#include "2d/CCNode.h"
#include "base/CCVector.h"
#include "objects/DragonBonesData.h"
#include "objects/ArmatureData.h"
#include "objects/SkinData.h"
#include "animation/DBAnimation.h"
#include "DBBone.h"

USING_NS_CC;
NAME_SPACE_DRAGON_BONES_BEGIN
class DBArmature
: public Node
{
public:
    static DBArmature* create(const std::string &dragonBonesName);
    
    
    DBArmature();
    virtual ~DBArmature();
    
    bool initWithName(const std::string &name);
    
    void setDragonBonesData(dragonBones::DragonBonesData* data);
    
    void setAnimation(dragonBones::DBAnimation* animation);
    dragonBones::DBAnimation* getAnimation();
    
    DBBone* createBone(BoneData* boneData);
    
    void addBone(DBBone *bone, const std::string& parentName = "");
    DBBone* getBone(const std::string& boneName);
    
    void createSkin();
private:
    
    DragonBonesData* m_pDragonBonesData;
    ArmatureData* m_pArmatureData;
    SkinData* m_pSkinData;
    DBAnimation* m_pAnimation;
    
    cocos2d::Map<std::string, DBBone*> _boneDic;                    //! The dictionary of the bones, include all bones in the armature, no matter it is the direct bone or the indirect bone. It is different from m_pChindren.
    cocos2d::Vector<DBBone*> _topBoneList;
    
};

NAME_SPACE_DRAGON_BONES_END
#endif /* DBArmature_hpp */
