#ifndef DRAGONBONES_ANIMATION_TIMELINE_STATE_H
#define DRAGONBONES_ANIMATION_TIMELINE_STATE_H

#include "DragonBones.h"
#include "geoms/ColorTransform.h"
#include "geoms/Point.h"
#include "geoms/Transform.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class AnimationData;
class CurveData;
class TransformTimeline;
class Bone;
class AnimationState;
class DBBone;


class TimelineState
{
    friend class AnimationState;
    friend class Bone;
    friend class DBBone;
    
	enum class UpdateState {UPDATE, UPDATE_ONCE, UNUPDATE};

public:
    TimelineState();
    virtual ~TimelineState();

	void fadeIn(Bone *bone, AnimationState *animationState, TransformTimeline *timeline);
	void fadeOut();
	void update(float progress);
	void updateMultipleFrame(float progress);
	void updateToNextFrame(int currentPlayTimes);
	void updateTween();
	void updateSingleFrame();
	void clear();

	// property
	bool getIsComplete() const { return _isComplete; }

	static TimelineState* borrowObject();
	static void returnObject(TimelineState *timelineState);
	static void clearObjects();
    
public:
	std::string name;

private:
	static std::vector<TimelineState*> _pool;

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

	Bone *_bone;
	AnimationState *_animationState;
	TransformTimeline *_timelineData;
	CurveData *_tweenCurve;

private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(TimelineState);
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_ANIMATION_TIMELINE_STATE_H
