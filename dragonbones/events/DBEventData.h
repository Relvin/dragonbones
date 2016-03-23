//
//  DBEventData.hpp
//  dragonbones
//
//  Created by Relvin on 16/3/23.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBEventData_hpp
#define DBEventData_hpp

#include "DragonBones.h"
#include "core/DBBase.h"
#include "base/CCRef.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class DBArmature;
class DBBone;
class DBAnimationState;
class Frame;

class DBEventData;

class DBEventDataMgr
: public MultData<DBEventData>
, public cocos2d::Ref
{
public:
    
    static DBEventDataMgr* create();
    bool init ();
    DBEventDataMgr ();
    ~DBEventDataMgr();
    DBEventData* getUnusedEventData();
    void removeEventData(DBEventData* eventData);
    void removeAllEventData();
};

class DBEventData
: public cocos2d::Ref
{
public:
    static const std::string Z_ORDER_UPDATED;
    static const std::string ANIMATION_FRAME_EVENT;
    static const std::string BONE_FRAME_EVENT;
    static const std::string SOUND;
    static const std::string FADE_IN;
    static const std::string FADE_OUT;
    static const std::string START;
    static const std::string COMPLETE;
    static const std::string LOOP_COMPLETE;
    static const std::string FADE_IN_COMPLETE;
    static const std::string FADE_OUT_COMPLETE;
    static const std::string _ERROR;
    
    enum class EventType
    {
        Z_ORDER_UPDATED,
        ANIMATION_FRAME_EVENT,
        BONE_FRAME_EVENT,
        SOUND,
        FADE_IN, FADE_OUT, START, COMPLETE, LOOP_COMPLETE, FADE_IN_COMPLETE, FADE_OUT_COMPLETE,
        _ERROR
    };
    
public:
    static const std::string& typeToString(EventType eventType);
    
    static DBEventData* create();
    static DBEventData* create(EventType type, DBArmature *armatureTarget);
    virtual bool init();
    virtual bool initWithTypeAndArmature(dragonBones::DBEventData::EventType type, dragonBones::DBArmature *armatureTarget);
    
    DBEventData();
    virtual ~DBEventData();
    void resetEventData();
    
    EventType getType() const;
    void setType(EventType type);
    const std::string& getStringType() const;

    const DBArmature* getArmature() const;
    void setArmature(DBArmature* armature);
    
    const DBBone* getBone() const;
    void setBone(DBBone* bone);
    
    const Frame* getFrame() const;
    void setFrame(Frame *frame);
    
    const DBAnimationState* getAnimationState() const;
    void setAnimationState(DBAnimationState *animationState);
    
    const std::string& getFrameLabel() const;
    void setFrameLabel(const std::string& frameLabel);
    const std::string& getSound() const;
    void setSound(const std::string& sound);
    
private:
    std::string _frameLabel;
    std::string _sound;
    DBArmature *_armature;
    DBBone *_bone;
    DBAnimationState *_animationState;
    Frame *_frame;
    EventType _type;
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBEventData);
};
NAME_SPACE_DRAGON_BONES_END


#endif /* DBEventData_hpp */
