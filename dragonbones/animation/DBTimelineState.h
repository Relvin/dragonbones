//
//  DBTimelineState.hpp
//  dragonbones
//
//  Created by Relvin on 16/3/16.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBTimelineState_hpp
#define DBTimelineState_hpp
#include "DragonBones.h"
#include "core/DBBase.h"
#include "geoms/ColorTransform.h"
#include "geoms/Point.h"
#include "geoms/Transform.h"
#include "base/CCRef.h"
#include "base/CCVector.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class AnimationData;
class CurveData;
class TransformTimeline;
class DBBone;
class DBAnimationState;
class DBBone;
class DBTimelineState;

class DBTimelineStateMgr
: public TemplateTimeline<DBTimelineState>
, public cocos2d::Ref
{
public:
    
    static DBTimelineStateMgr* create();
    bool init ();
    DBTimelineStateMgr ();
    ~DBTimelineStateMgr();
    
private:
    
};


class DBTimelineState
: public cocos2d::Ref
{
    
    enum class UpdateState {UPDATE, UPDATE_ONCE, UNUPDATE};
    
public:
    static DBTimelineState* create();
    bool init();
    
    DBTimelineState();
    virtual ~DBTimelineState();
    
    void fadeIn(DBBone *bone, DBAnimationState *animationState, TransformTimeline *timeline);
    void fadeOut();
    void update(float progress);
    void updateMultipleFrame(float progress);
    void updateToNextFrame(int currentPlayTimes);
    void updateTween();
    void updateSingleFrame();
    void resetTimelineState();
    
    // property
    bool getIsComplete() const { return _isComplete; }
    
public:
    const Transform& getTransform() const;
    const Point& getPivot() const;
    float getWeight();
    void setWeight(float weight);
    const DBAnimationState *getAnimationState() const;
    bool getBlendEnabled();
    
    const std::string& getName() const;
private:
    std::string name;
    
    bool _blendEnabled;
    bool _isComplete;
    bool _tweenTransform;
    bool _tweenScale;
    int _currentTime;
    int _currentFrameIndex;
    int _currentFramePosition;
    int _currentFrameDuration;
    int _lastTime;
    int _totalTime;
    float _weight;
    float _tweenEasing;
    
    UpdateState _updateState;
    Transform _transform;
    Transform _durationTransform;
    Transform _originTransform;
    Point _pivot;
    Point _durationPivot;
    Point _originPivot;
    
    DBBone *_bone;
    DBAnimationState *_animationState;
    TransformTimeline *_timelineData;
    CurveData *_tweenCurve;
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBTimelineState);
};
NAME_SPACE_DRAGON_BONES_END

#endif /* DBTimelineState_hpp */
