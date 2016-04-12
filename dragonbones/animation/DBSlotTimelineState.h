//
//  DBSlotTimelineState.hpp
//  dragonbones
//
//  Created by Relvin on 16/3/17.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBSlotTimelineState_hpp
#define DBSlotTimelineState_hpp

#include "DragonBones.h"
#include "geoms/ColorTransform.h"
#include "core/DBBase.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class CurveData;
class DBArmature;
class SlotTimeline;
class DBSlot;
class DBAnimation;
class DBAnimationState;
class DBAnimation;
class Frame;
class SlotFrame;

class DBSlotTimelineState;

class DBSlotTimelineStateMgr
: public TemplateTimeline<DBSlotTimelineState>
, public cocos2d::Ref
{
public:
    
    static DBSlotTimelineStateMgr* create();
    bool init ();
    DBSlotTimelineStateMgr ();
    ~DBSlotTimelineStateMgr();
    
private:
    
};


class DBSlotTimelineState
: public cocos2d::Ref
{
    enum class UpdateState {UPDATE, UPDATE_ONCE, UNUPDATE};
    
public:
    
    static DBSlotTimelineState* create();
    virtual bool init();
    
    DBSlotTimelineState();
    virtual ~DBSlotTimelineState();
    
    void fadeIn(DBSlot *slot, DBAnimationState *animationState, SlotTimeline *timelineData);
    const std::string & getName();
    void resetTimelineState();
    void update(float progress);
    inline bool getIsComplete() const { return _isComplete; }
private:
    
    void updateMultipleFrame(float progress);
    void updateToNextFrame(int currentPlayTimes);
    void updateTween();
    void updateSingleFrame();
    
private:
    std::string name;
    float _weight;
    bool _blendEnabled;
    bool _isComplete;
    int _currentTime;
    int _currentFrameIndex;
    int _currentFramePosition;
    int _currentFrameDuration;
    int _totalTime;
    
    bool _tweenColor;
    float _tweenEasing;
    CurveData *_tweenCurve;
    
    float _rawAnimationScale;
    int _updateMode;
    
    DBAnimationState *_animationState;
    DBArmature *_armature;
    DBAnimation *_animation;
    DBSlot *_slot;
    SlotTimeline *_timelineData;
    ColorTransform _durationColor;
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBSlotTimelineState);
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBSlotTimelineState_hpp */
