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
#include "geoms/ColorTransform.h"
#include "geoms/Point.h"
#include "geoms/Transform.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class AnimationData;
class CurveData;
class TransformTimeline;
class DBBone;
class DBAnimationState;
class DBBone;


class DBTimelineState
{
    friend class DBAnimationState;
    friend class DBBone;
    
    enum class UpdateState {UPDATE, UPDATE_ONCE, UNUPDATE};
    
public:
    DBTimelineState();
    virtual ~DBTimelineState();
    
    void fadeIn(DBBone *bone, DBAnimationState *animationState, TransformTimeline *timeline);
    void fadeOut();
    void update(float progress);
    void updateMultipleFrame(float progress);
    void updateToNextFrame(int currentPlayTimes);
    void updateTween();
    void updateSingleFrame();
    void clear();
    
    // property
    bool getIsComplete() const { return _isComplete; }
    
    static DBTimelineState* borrowObject();
    static void returnObject(DBTimelineState *timelineState);
    static void clearObjects();
    
public:
    std::string name;
    
private:
    static std::vector<DBTimelineState*> _pool;
    
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
