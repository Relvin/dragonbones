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
#include "DBSlot.h"

USING_NS_CC;
NAME_SPACE_DRAGON_BONES_BEGIN

class Frame;
class DBEventDataMgr;
class DBIKConstraint;
class DBEventData;

class DBArmature
: public Node
{
public:
    
    
    static DBArmature* createWithFileName(const std::string &dragonBoneFileName,const std::string &textureFilename,const std::string &dragonBoneName);
    static DBArmature* createWithFileName(const std::string &dragonBonesFileName,const std::string &textureFilename,const std::string &dragonBonesName,const std::string &texture);
    
    static DBArmature* create(const std::string &dragonBonesName);
    static DBArmature* create(const std::string &dragonBonesName,const std::string &texture);
    
    static DBArmature* createWithData(const std::string &dragonBonesName, const std::string &dragonbonesDataName);
    
    DBArmature();
    virtual ~DBArmature();
    
    virtual bool initWithName(const std::string &name);
    virtual bool initWithName(const std::string &name,const std::string &textureName);
    virtual bool initWithDataName(const std::string &name, const std::string &dataName);
    
    void setDragonBonesData(dragonBones::DragonBonesData* data);
    
    void setAnimation(dragonBones::DBAnimation* animation);
    dragonBones::DBAnimation* getAnimation();
    
    DBBone* createBone(BoneData* boneData);
    
    void addBone(DBBone *bone, const std::string& parentName = "");
    DBBone* getBone(const std::string& boneName);
    
    DBSlot* getSlot(const std::string& slotName);
    
    void createSkin(const std::string &textureName);
    
    inline void setSpeed(float speed) {_pAnimation->setTimeScale(speed);};
    
    void buildIK();
    
    void update(float delta);
    void updateBoneCache();
    virtual void onEnter();
    
    void arriveAtFrame(Frame *frame, DBAnimationState *animationState, bool isCross);
    cocos2d::Vector<DBIKConstraint*> getIKTargetData(DBBone *bone);
    DBEventDataMgr* getEventDataManager() const;
    inline bool getSkewEnable() {return this->_skewEnable;};
    
    void setEventCallback(const std::function<void(const DBEventData*)>& callback) { _dbEventCallback = callback; }
    const std::function<void(const DBEventData*)>& getEventCallback() const { return _dbEventCallback; }
protected:
    std::function<void(const DBEventData*)> _dbEventCallback;
private:
    
    DragonBonesData* _pDragonBonesData;
    ArmatureData* _pArmatureData;
    SkinData* _pSkinData;
    DBAnimation* _pAnimation;
    
    cocos2d::Map<std::string, DBBone*> _boneDic;                    //! The dictionary of the bones, include all bones in the armature, no matter it is the direct bone or the indirect bone. It is different from m_pChindren.
    cocos2d::Vector<DBBone*> _topBoneList;
    cocos2d::Map<std::string,DBSlot*> _slotDic;
    cocos2d::Vector<DBIKConstraint*> _ikList;
    
    DBEventDataMgr* _eventDataMgr;
    bool _skewEnable;
    
    
};

NAME_SPACE_DRAGON_BONES_END
#endif /* DBArmature_hpp */
