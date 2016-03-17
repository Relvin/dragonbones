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

class DBSlotTimelineState
{
    enum class UpdateState {UPDATE, UPDATE_ONCE, UNUPDATE};
    
public:
    static std::vector<DBSlotTimelineState*> _pool;
    static DBSlotTimelineState* borrowObject();
    static void returnObject(DBSlotTimelineState *timelineState);
    static void clearObjects();
    
    DBSlotTimelineState();
    virtual ~DBSlotTimelineState();
    
    void fadeIn(DBSlot *slot, DBAnimationState *animationState, SlotTimeline *timelineData);
    
private:
    void update(float progress);
    void updateMultipleFrame(float progress);
    void updateToNextFrame(int currentPlayTimes);
    void updateTween();
    void updateSingleFrame();
    void clear();
    
public:
    std::string name;
    
private:
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
