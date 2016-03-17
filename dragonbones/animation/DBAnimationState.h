//
//  DBAnimationState.hpp
//  dragonbones
//
//  Created by Relvin on 16/3/14.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBAnimationState_h
#define DBAnimationState_h

#include "DragonBones.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class AnimationData;
class DBSlotTimelineState;
class DBTimelineState;
class DBArmature;
class DBAnimation;

class DBAnimationState
{
    friend class DBArmature;
    friend class TimelineState;
    friend class SlotTimelineState;
    
    friend class DBAnimation;
    
    enum class FadeState {FADE_BEFORE, FADING, FADE_COMPLETE};
    
public:
    DBAnimationState();
    virtual ~DBAnimationState();
    
    DBAnimationState* fadeOut(float fadeTotalTime, bool pausePlayhead);
    DBAnimationState* play();
    DBAnimationState* stop();
    bool getMixingTransform(const std::string &timelineName) const;
    DBAnimationState* addMixingTransform(const std::string &timelineName, bool recursive = true);
    DBAnimationState* removeMixingTransform(const std::string &timelineName, bool recursive = true);
    DBAnimationState* removeAllMixingTransform();
    
    bool getIsComplete() const;
    bool getIsPlaying() const;
    int getCurrentPlayTimes() const;
    int getLayer() const;
    float getTotalTime() const;
    float getCurrentWeight() const;
    const std::string& getGroup() const;
    const AnimationData* getClip() const;
    
    DBAnimationState* setAdditiveBlending(bool value);
    DBAnimationState* setAutoFadeOut(bool value, float fadeOutTime = -1);
    DBAnimationState* setWeight(float value);
    DBAnimationState* setFrameTween(bool autoTween_, bool lastFrameAutoTween_);
    
    int getPlayTimes() const;
    DBAnimationState* setPlayTimes(int playTimes);
    float getCurrentTime() const;
    DBAnimationState* setCurrentTime(float currentTime);
    float getTimeScale() const;
    DBAnimationState* setTimeScale(float timeScale);
    
    bool containsBoneMask(const std::string &boneName);
    DBAnimationState* addBoneMask(const std::string &boneName, bool ifInvolveChildBones = true);
    void addBoneToBoneMask(const std::string &boneName);
    DBAnimationState* removeBoneMask(const std::string &boneName, bool ifInvolveChildBones = true);
    void removeBoneFromBoneMask(const std::string &boneName);
    
    void resetTimelineStateList();
    
private:
    static std::vector<DBAnimationState*> _pool;
    static DBAnimationState* borrowObject();
    static void returnObject(DBAnimationState *animationState);
    static void clearObjects();
    
    void fadeIn(DBArmature *armature, AnimationData *clip, float fadeTotalTime, float timeScale, int playTimes, bool pausePlayhead);
    
    bool advanceTime(float passedTime);
    void updateTimelineStates();
    void addTimelineState(const std::string &timelineName);
    void addSlotTimelineState(const std::string &timelineName);
    
    void removeTimelineState(DBTimelineState *timelineState);
    void removeSlotTimelineState(DBSlotTimelineState *timelineState);
    void advanceFadeTime(float passedTime);
    void advanceTimelinesTime(float passedTime);
    void updateMainTimeline(bool isThisComplete);
    void hideBones();
    void clear();
    
public:
    bool additiveBlending;
    bool autoTween;
    bool autoFadeOut;
    bool displayControl;
    bool lastFrameAutoTween;
    float fadeOutTime;
    float weight;
    std::string name;
    
private:
    bool _isPlaying;
    bool _isComplete;
    bool _isFadeOut;
    bool _pausePlayheadInFade;
    int _currentPlayTimes;
    int _layer;
    int _playTimes;
    int _currentTime;
    int _currentFrameIndex;
    int _currentFramePosition;
    int _currentFrameDuration;
    int _totalTime;
    int _lastTime;
    float _time;
    float _timeScale;
    float _fadeWeight;
    float _fadeTotalWeight;
    float _fadeCurrentTime;
    float _fadeTotalTime;
    float _fadeBeginTime;
    std::string _group;
    FadeState _fadeState;
    
    std::vector<DBTimelineState*> _timelineStateList;
    std::vector<std::string> _mixingTransforms;
    std::vector<DBSlotTimelineState*> _slotTimelineStateList;
    std::vector<std::string> _boneMasks;
    
    AnimationData *_clip;
    DBArmature *_armature;
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBAnimationState);
};
NAME_SPACE_DRAGON_BONES_END


#endif /* DBAnimationState_h */
