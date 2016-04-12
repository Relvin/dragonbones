//
//  DBFFDTimeline.hpp
//  dragonbones
//
//  Created by Relvin on 16/4/6.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBFFDTimeline_hpp
#define DBFFDTimeline_hpp

#include "DragonBones.h"
#include "base/CCRef.h"
#include "core/DBBase.h"


NAME_SPACE_DRAGON_BONES_BEGIN
class DBAnimationState;
class CurveData;
class DBArmature;
class DBAnimation;
class DBSlot;
class MeshData;
class FFDTimeline;


class DBFFDTimelineState;

class DBFFDTimelineStateMgr
: public TemplateTimeline<DBFFDTimelineState>
, public cocos2d::Ref
{
public:
    
    static DBFFDTimelineStateMgr* create();
    bool init ();
    DBFFDTimelineStateMgr ();
    ~DBFFDTimelineStateMgr();
    
private:
    
};



class DBFFDTimelineState
: public cocos2d::Ref
{
    
public:
    static DBFFDTimelineState* create();
    virtual bool init();
    
    DBFFDTimelineState();
    virtual ~DBFFDTimelineState();
    void fadeIn(MeshData *mesh, DBAnimationState *animationState, FFDTimeline* timelineData);
    inline std::string getName() {return this->name;};
    void resetTimelineState();
    inline bool getIsComplete() const { return _isComplete; }
    void update(float progress);
protected:
    
    void updateMultipleFrame(float progress);
    void updateToNextFrame(int currentPlayTimes);
    void updateTween();
    void updateSingleFrame();
    
private:
    std::string name;
    std::string slotName;
    std::string skinName;
    float _weight;
    bool _blendEnabled;
    bool _isComplete;
    
    DBAnimationState *_animationState;
    
    int _totalTime;
    
    int _currentTime;
    int _currentFrameIndex;
    int _currentFramePosition;
    int _currentFrameDuration;

    float _tweenEasing;
    CurveData *_tweenCurve;
    
    bool _tweenVertices;
    int _offset;
    std::vector<float> _durationVertices;
    std::vector<float> _updateVertices;
    float _rawAnimationScale;
    
    int _updateMode;
    DBArmature *_armature;
    DBAnimation *_animation;
    DBSlot *_slot;
    MeshData* _mesh;
    FFDTimeline *_timelineData;
    
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBFFDTimeline_hpp */
