//
//  DBAnimationState.cpp
//  dragonbones
//
//  Created by Relvin on 16/3/14.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBAnimationState.h"

#include <cstdlib>

#include "animation/DBTimelineState.h"
#include "animation/DBSlotTimelineState.h"
#include "objects/AnimationData.h"
#include "renderer/DBSlot.h"
#include "renderer/DBBone.h"
#include "renderer/DBArmature.h"
#include "events/EventData.h"
#include "events/IEventDispatcher.h"
#include "animation/DBAnimation.h"

NAME_SPACE_DRAGON_BONES_BEGIN
std::vector<DBAnimationState*> DBAnimationState::_pool;

DBAnimationState* DBAnimationState::borrowObject()
{
    if (_pool.empty())
    {
        return new DBAnimationState();
    }
    
    DBAnimationState *animationState = _pool.back();
    _pool.pop_back();
    return animationState;
}

void DBAnimationState::returnObject(DBAnimationState *animationState)
{
    auto iterator = std::find(_pool.cbegin(), _pool.cend(), animationState);
    
    if (iterator == _pool.end())
    {
        _pool.push_back(animationState);
    }
    
    animationState->clear();
}

void DBAnimationState::clearObjects()
{
    for (size_t i = 0, l = _pool.size(); i < l; ++i)
    {
        _pool[i]->clear();
        delete _pool[i];
    }
    
    _pool.clear();
}

bool DBAnimationState::getIsComplete() const
{
    return _isComplete;
}

bool DBAnimationState::getIsPlaying() const
{
    return (_isPlaying && !_isComplete);
}

int DBAnimationState::getCurrentPlayTimes() const
{
    return _currentPlayTimes < 0 ? 0 : _currentPlayTimes;
}

int DBAnimationState::getLayer() const
{
    return _layer;
}

float DBAnimationState::getTotalTime() const
{
    return _totalTime * 0.001f;
}

float DBAnimationState::getCurrentWeight() const
{
    return _fadeWeight * weight;
}

const std::string& DBAnimationState::getGroup() const
{
    return _group;
}

const AnimationData* DBAnimationState::getClip() const
{
    return _clip;
}

DBAnimationState* DBAnimationState::setAdditiveBlending(bool value)
{
    additiveBlending = value;
    return this;
}

DBAnimationState* DBAnimationState::setAutoFadeOut(bool value, float fadeOutTime_)
{
    autoFadeOut = value;
    
    if (fadeOutTime_ >= 0)
    {
        fadeOutTime = fadeOutTime_;
    }
    
    return this;
}

DBAnimationState* DBAnimationState::setWeight(float value)
{
    weight = value;
    return this;
}

DBAnimationState* DBAnimationState::setFrameTween(bool autoTween_, bool lastFrameAutoTween_)
{
    autoTween = autoTween_;
    lastFrameAutoTween = lastFrameAutoTween_;
    return this;
}

int DBAnimationState::getPlayTimes() const
{
    return _playTimes;
}
DBAnimationState* DBAnimationState::setPlayTimes(int playTimes)
{
    _playTimes = playTimes;
    
    if (round(_totalTime * 0.001f * _clip->frameRate) < 2)
    {
        _playTimes = playTimes < 0 ? -1 : 1;
    }
    else
    {
        _playTimes = playTimes < 0 ? -playTimes : playTimes;
    }
    
    autoFadeOut = playTimes < 0 ? true : false;
    return this;
}

float DBAnimationState::getCurrentTime() const
{
    return _currentTime < 0 ? 0.f : _currentTime * 0.001f;
}
DBAnimationState* DBAnimationState::setCurrentTime(float currentTime)
{
    if (currentTime < 0 || currentTime != currentTime)
    {
        currentTime = 0.f;
    }
    
    _time = currentTime;
    _currentTime = static_cast<int>(_time * 1000.f);
    return this;
}

float DBAnimationState::getTimeScale() const
{
    return _timeScale;
}
DBAnimationState* DBAnimationState::setTimeScale(float timeScale)
{
    if (timeScale != timeScale)
    {
        timeScale = 1.f;
    }
    
    _timeScale = timeScale;
    return this;
}

bool DBAnimationState::containsBoneMask(const std::string &boneName)
{
    return std::find(_boneMasks.cbegin(), _boneMasks.cend(), boneName) != _boneMasks.cend();
}

DBAnimationState* DBAnimationState::addBoneMask(const std::string &boneName, bool ifInvolveChildBones)
{
    addBoneToBoneMask(boneName);
    if (ifInvolveChildBones)
    {
        auto currentBone = _armature->getBone(boneName);
        if (currentBone)
        {
            auto childBones = currentBone->getBones();
            for (size_t i = 0, l = childBones.size(); i < l; ++i)
            {
                auto tempBone = childBones.at(i);
                addBoneToBoneMask(tempBone->getName());
            }
        }
    }
    updateTimelineStates();
    return this;
}

void DBAnimationState::addBoneToBoneMask( const std::string &boneName )
{
    if (_clip->getTimeline(boneName) && !containsBoneMask(boneName))
    {
        _boneMasks.push_back(boneName);
    }
}

void DBAnimationState::removeBoneFromBoneMask(const std::string &boneName)
{
    auto iterator = std::find(_boneMasks.cbegin(), _boneMasks.cend(), boneName);
    if (iterator != _boneMasks.cend())
    {
        _boneMasks.erase(iterator);
    }
}

DBAnimationState* DBAnimationState::removeBoneMask(const std::string &boneName, bool ifInvolveChildBones)
{
    removeBoneFromBoneMask(boneName);
    if (ifInvolveChildBones)
    {
        auto currentBone = _armature->getBone(boneName);
        if (currentBone)
        {
            auto childBones = currentBone->getBones();
            for (size_t i = 0, l = childBones.size(); i < l; ++i)
            {
                auto tempBone = childBones.at(i);
                removeBoneFromBoneMask(tempBone->getName());
            }
            
        }
    }
    updateTimelineStates();
    return this;
}

DBAnimationState::DBAnimationState() :
_clip(nullptr)
,_armature(nullptr)
{}
DBAnimationState::~DBAnimationState()
{
    clear();
}

void DBAnimationState::fadeIn(DBArmature *armature, AnimationData *clip, float fadeTotalTime, float timeScale, int playTimes, bool pausePlayhead)
{
    _armature = armature;
    _clip = clip;
    _pausePlayheadInFade = pausePlayhead;
    _totalTime = _clip->duration;
    autoTween = _clip->autoTween;
    name = _clip->name;
    setTimeScale(timeScale);
    setPlayTimes(playTimes);
    // reset
    _isComplete = false;
    _currentFrameIndex = -1;
    _currentPlayTimes = -1;
    
    if (round(_totalTime * 0.001f * _clip->frameRate) < 2)
    {
        _currentTime = _totalTime;
    }
    else
    {
        _currentTime = -1;
    }
    
    _time = 0.f;
    _mixingTransforms.clear();
    // fade start
    _isFadeOut = false;
    _fadeWeight = 0.f;
    _fadeTotalWeight = 1.f;
    _fadeCurrentTime = 0.f;
    _fadeBeginTime = _fadeCurrentTime;
    _fadeTotalTime = fadeTotalTime * _timeScale;
    _fadeState = FadeState::FADE_BEFORE;
    // default
    _isPlaying = true;
    displayControl = true;
    lastFrameAutoTween = true;
    additiveBlending = false;
    weight = 1.f;
    fadeOutTime = fadeTotalTime;
    updateTimelineStates();
}

DBAnimationState* DBAnimationState::fadeOut(float fadeTotalTime, bool pausePlayhead)
{
    if (!(fadeTotalTime >= 0))
    {
        fadeTotalTime = 0.f;
    }
    
    _pausePlayheadInFade = pausePlayhead;
    
    if (_isFadeOut)
    {
        if (fadeTotalTime > _fadeTotalTime / _timeScale - (_fadeCurrentTime - _fadeBeginTime))
        {
            return this;
        }
    }
    else
    {
        for (size_t i = 0, l = _timelineStateList.size(); i < l; ++i)
        {
            _timelineStateList[i]->fadeOut();
        }
    }
    
    // fade start
    _isFadeOut = true;
    _fadeTotalWeight = _fadeWeight;
    _fadeState = FadeState::FADE_BEFORE;
    _fadeBeginTime = _fadeCurrentTime;
    _fadeTotalTime = _fadeTotalWeight >= 0 ? fadeTotalTime * _timeScale : 0.f;
    // default
    displayControl = false;
    return this;
}

DBAnimationState* DBAnimationState::play()
{
    _isPlaying = true;
    return this;
}

DBAnimationState* DBAnimationState::stop()
{
    _isPlaying = false;
    return this;
}

bool DBAnimationState::getMixingTransform(const std::string &timelineName) const
{
    return std::find(_mixingTransforms.cbegin(), _mixingTransforms.cend(), timelineName) != _mixingTransforms.cend();
}

DBAnimationState* DBAnimationState::addMixingTransform(const std::string &timelineName, bool recursive)
{
    if (recursive)
    {
        DBBone *currentBone = _armature->getBone(timelineName);
        if (currentBone)
        {

            auto childBones = currentBone->getBones();
            for (size_t i = 0, l = childBones.size(); i < l; ++i)
            {
                auto tempBone = childBones.at(i);
                const std::string &boneName = tempBone->getName();
                if (_clip->getTimeline(boneName) &&
                std::find(_mixingTransforms.cbegin(), _mixingTransforms.cend(), boneName) == _mixingTransforms.cend())
                {
                    _mixingTransforms.push_back(boneName);
                }
            }
        }
        
    }
    
    if (
             _clip->getTimeline(timelineName) &&
             std::find(_mixingTransforms.cbegin(), _mixingTransforms.cend(), timelineName) == _mixingTransforms.cend()
             )
    {
        _mixingTransforms.push_back(timelineName);
    }
    
    updateTimelineStates();
    return this;
}

DBAnimationState* DBAnimationState::removeMixingTransform(const std::string &timelineName, bool recursive)
{
    if (recursive)
    {
        DBBone *currentBone = _armature->getBone(timelineName);
        if (currentBone)
        {
            _mixingTransforms.push_back(timelineName);
            auto childBones = currentBone->getBones();
            for (size_t i = 0, l = childBones.size(); i < l; ++i)
            {
                auto tempBone = childBones.at(i);
                const std::string &boneName = tempBone->getName();
                if (_clip->getTimeline(boneName))
                {
                    auto iterator = std::find(_mixingTransforms.begin(), _mixingTransforms.end(), boneName);
                    
                    if (iterator != _mixingTransforms.end())
                    {
                        _mixingTransforms.erase(iterator);
                    }
                }
            }
        }
        // From root to leaf
    }
    
    auto iterator = std::find(_mixingTransforms.begin(), _mixingTransforms.end(), timelineName);
    
    if (iterator != _mixingTransforms.end())
    {
        _mixingTransforms.erase(iterator);
    }
    
    updateTimelineStates();
    return this;
}

DBAnimationState* DBAnimationState::removeAllMixingTransform()
{
    _mixingTransforms.clear();
    updateTimelineStates();
    return this;
}

bool DBAnimationState::advanceTime(float passedTime)
{
    passedTime *= _timeScale;
    advanceFadeTime(passedTime);
    
    if (_fadeWeight)
    {
        advanceTimelinesTime(passedTime);
    }
    
    return _isFadeOut && _fadeState == FadeState::FADE_COMPLETE;
}

void DBAnimationState::updateTimelineStates()
{
    for (size_t i = _timelineStateList.size(); i--;)
    {
        DBTimelineState *timelineState = _timelineStateList[i];
        if (!_armature->getBone(timelineState->name))
        {
            removeTimelineState(timelineState);
        }
    }
    
    for (size_t i = _slotTimelineStateList.size(); i--;)
    {
        DBSlotTimelineState *timelineState = _slotTimelineStateList[i];
        if (!_armature->getSlot(timelineState->name))
        {
            removeSlotTimelineState(timelineState);
        }
    }
    
    if (_boneMasks.size() > 0)
    {
        for (size_t i = _timelineStateList.size(); i--;)
        {
            DBTimelineState *timelineState = _timelineStateList[i];
            auto iterator = std::find(_mixingTransforms.cbegin(), _mixingTransforms.cend(), timelineState->name);
            
            if (iterator == _mixingTransforms.cend())
            {
                removeTimelineState(timelineState);
            }
        }
        
        for (size_t i = 0, l = _mixingTransforms.size(); i < l; ++i)
        {
            addTimelineState(_mixingTransforms[i]);
        }
    }
    else
    {
        for (size_t i = 0, l = _clip->timelineList.size(); i < l; ++i)
        {
            addTimelineState(_clip->timelineList[i]->name);
        }
    }
    
    // add slot timeline
    for (size_t i = 0, l = _clip->slotTimelineList.size(); i < l; ++i)
    {
        addSlotTimelineState(_clip->slotTimelineList[i]->name);
    }
}

void DBAnimationState::addTimelineState(const std::string &timelineName)
{
    DBBone *bone = _armature->getBone(timelineName);
    if (bone)
    {
        for (size_t i = 0, l = _timelineStateList.size(); i < l; ++i)
        {
            if (_timelineStateList[i]->name == timelineName)
            {
                return;
            }
        }
        
        DBTimelineState *timelineState = DBTimelineState::borrowObject();
        timelineState->fadeIn(bone, this, _clip->getTimeline(timelineName));
        _timelineStateList.push_back(timelineState);
    }
}

void DBAnimationState::removeTimelineState(DBTimelineState *timelineState)
{
    auto iterator = std::find(_timelineStateList.begin(), _timelineStateList.end(), timelineState);
    if (iterator != _timelineStateList.end())
    {
        DBTimelineState::returnObject(timelineState);
        _timelineStateList.erase(iterator);
    }
}

void DBAnimationState::addSlotTimelineState(const std::string &timelineName)
{
    DBSlot *slot = _armature->getSlot(timelineName);
    if (slot /*&& slot->getDisplayList().size() > 0*/)
    {
        for (size_t i = 0, l = _slotTimelineStateList.size(); i < l; ++i)
        {
            if (_slotTimelineStateList[i]->name == timelineName)
            {
                return;
            }
        }
        DBSlotTimelineState *timelineState = DBSlotTimelineState::borrowObject();
        timelineState->fadeIn(slot, this, _clip->getSlotTimeline(timelineName));
        _slotTimelineStateList.push_back(timelineState);
    }
}

void DBAnimationState::removeSlotTimelineState(DBSlotTimelineState *timelineState)
{
    auto iterator = std::find(_slotTimelineStateList.begin(), _slotTimelineStateList.end(), timelineState);
    if (iterator != _slotTimelineStateList.end())
    {
        DBSlotTimelineState::returnObject(timelineState);
        _slotTimelineStateList.erase(iterator);
    }
}

void DBAnimationState::advanceFadeTime(float passedTime)
{
    bool fadeStartFlg = false;
    bool fadeCompleteFlg = false;
    
    if (_fadeBeginTime >= 0)
    {
        FadeState fadeState = _fadeState;
        _fadeCurrentTime += passedTime < 0 ? -passedTime : passedTime;
        
        if (_fadeCurrentTime >= _fadeBeginTime + _fadeTotalTime)
        {
            // fade complete
            if (_fadeWeight == 1 || _fadeWeight == 0)
            {
                fadeState = FadeState::FADE_COMPLETE;
                
                if (_pausePlayheadInFade)
                {
                    _pausePlayheadInFade = false;
                    _currentTime = -1;
                }
            }
            
            _fadeWeight = _isFadeOut ? 0.f : 1.f;
        }
        else if (_fadeCurrentTime >= _fadeBeginTime)
        {
            // fading
            fadeState = FadeState::FADING;
            _fadeWeight = (_fadeCurrentTime - _fadeBeginTime) / _fadeTotalTime * _fadeTotalWeight;
            
            if (_isFadeOut)
            {
                _fadeWeight = _fadeTotalWeight - _fadeWeight;
            }
        }
        else
        {
            // fade before
            fadeState = FadeState::FADE_BEFORE;
            _fadeWeight = _isFadeOut ? 1.f : 0.f;
        }
        
        if (_fadeState != fadeState)
        {
            // _fadeState == FadeState::FADE_BEFORE && (fadeState == FadeState::FADING || fadeState == FadeState::FADE_COMPLETE)
            if (_fadeState == FadeState::FADE_BEFORE)
            {
                fadeStartFlg = true;
            }
            
            // (_fadeState == FadeState::FADE_BEFORE || _fadeState == FadeState::FADING) && fadeState == FadeState::FADE_COMPLETE
            if (fadeState == FadeState::FADE_COMPLETE)
            {
                fadeCompleteFlg = true;
            }
            
            _fadeState = fadeState;
        }
    }
    
    if (fadeStartFlg)
    {
        EventData::EventType eventDataType;
        
        if (_isFadeOut)
        {
            eventDataType = EventData::EventType::FADE_OUT;
        }
        else
        {
            hideBones();
            eventDataType = EventData::EventType::FADE_IN;
        }
        
        //modify by Relvin need todo
//        if (_armature->_eventDispatcher->hasEvent(eventDataType))
//        {
//            EventData *eventData = EventData::borrowObject(eventDataType);
//            eventData->armature = _armature;
//            eventData->animationState = this;
//            _armature->_eventDataList.push_back(eventData);
//        }
    }
    
    if (fadeCompleteFlg)
    {
        EventData::EventType eventDataType;
        if (_isFadeOut)
        {
            eventDataType = EventData::EventType::FADE_OUT_COMPLETE;
        }
        else
        {
            eventDataType = EventData::EventType::FADE_IN_COMPLETE;
        }
        //modify by Relvin need todo
//        if (_armature->_eventDispatcher->hasEvent(eventDataType))
//        {
//            EventData *eventData = EventData::borrowObject(eventDataType);
//            eventData->armature = _armature;
//            eventData->animationState = this;
//            _armature->_eventDataList.push_back(eventData);
//        }
    }
}

void DBAnimationState::advanceTimelinesTime(float passedTime)
{
    if (_isPlaying && !_pausePlayheadInFade)
    {
        _time += passedTime;
    }
    
    bool startFlg = false;
    bool completeFlg = false;
    bool loopCompleteFlg = false;
    bool isThisComplete = false;
    int currentPlayTimes = 0;
    int currentTime = (int)(_time * 1000.f);
    
    if (_playTimes == 0)
    {
        isThisComplete = false;
        currentPlayTimes = (int)(ceil(abs(currentTime) / (float)(_totalTime)));
        currentTime -= (int)(floor(currentTime / (float)(_totalTime))) * _totalTime;
        
        if (currentTime < 0)
        {
            currentTime += _totalTime;
        }
    }
    else
    {
        int totalTimes = _playTimes * _totalTime;
        
        if (currentTime >= totalTimes)
        {
            currentTime = totalTimes;
            isThisComplete = true;
        }
        else if (currentTime <= -totalTimes)
        {
            currentTime = -totalTimes;
            isThisComplete = true;
        }
        else
        {
            isThisComplete = false;
        }
        
        if (currentTime < 0)
        {
            currentTime += totalTimes;
        }
        
        currentPlayTimes = (int)(ceil(currentTime / (float)(_totalTime)));
        currentTime -= (int)(floor(currentTime / (float)(_totalTime))) * _totalTime;
        
        if (isThisComplete)
        {
            currentTime = _totalTime;
        }
    }
    
    if (currentPlayTimes == 0)
    {
        currentPlayTimes = 1;
    }
    
    // update timeline
    _isComplete = isThisComplete;
    float progress = _time * 1000.f / (float)(_totalTime);
    
    for (size_t i = 0, l = _timelineStateList.size(); i < l; ++i)
    {
        _timelineStateList[i]->update(progress);
        _isComplete = _timelineStateList[i]->getIsComplete() && _isComplete;
    }
    
    // update main timeline
    if (_currentTime != currentTime)
    {
        if (_currentPlayTimes != currentPlayTimes)    // check loop complete
        {
            if (_currentPlayTimes > 0 && currentPlayTimes > 1)
            {
                loopCompleteFlg = true;
            }
            
            _currentPlayTimes = currentPlayTimes;
        }
        
        if (_currentTime < 0 && !_pausePlayheadInFade)    // check start
        {
            startFlg = true;
        }
        
        if (_isComplete)    // check complete
        {
            completeFlg = true;
        }
        
        _lastTime = _currentTime;
        _currentTime = currentTime;
        updateMainTimeline(isThisComplete);
    }
    
    if (startFlg)
    {
        //modify by Relvin need todo
//        if (_armature->_eventDispatcher->hasEvent(EventData::EventType::START))
//        {
//            EventData *eventData = EventData::borrowObject(EventData::EventType::START);
//            eventData->armature = _armature;
//            eventData->animationState = this;
//            _armature->_eventDataList.push_back(eventData);
//        }
    }
    
    if (completeFlg)
    {
        //modify by Relvin need todo
//        if (_armature->_eventDispatcher->hasEvent(EventData::EventType::COMPLETE))
//        {
//            EventData *eventData = EventData::borrowObject(EventData::EventType::COMPLETE);
//            eventData->armature = _armature;
//            eventData->animationState = this;
//            _armature->_eventDataList.push_back(eventData);
//        }
        
        if (autoFadeOut)
        {
            fadeOut(fadeOutTime, true);
        }
    }
    else if (loopCompleteFlg)
    {
        //modify by Relvin need todo
//        if (_armature->_eventDispatcher->hasEvent(EventData::EventType::LOOP_COMPLETE))
//        {
//            EventData *eventData = EventData::borrowObject(EventData::EventType::LOOP_COMPLETE);
//            eventData->armature = _armature;
//            eventData->animationState = this;
//            _armature->_eventDataList.push_back(eventData);
//        }
    }
}

void DBAnimationState::updateMainTimeline(bool isThisComplete)
{
    if (!_clip->frameList.empty())
    {
        Frame *prevFrame = nullptr;
        Frame *currentFrame = nullptr;
        
        for (size_t i = 0, l = _clip->frameList.size(); i < l; ++i)
        {
            if (_currentFrameIndex < 0)
            {
                _currentFrameIndex = 0;
            }
            else if (_currentTime < _currentFramePosition || _currentTime >= _currentFramePosition + _currentFrameDuration || _currentTime < _lastTime)
            {
                _lastTime = _currentTime;
                ++_currentFrameIndex;
                if (_currentFrameIndex >= (int)(l))
                {
                    if (isThisComplete)
                    {
                        --_currentFrameIndex;
                        break;
                    }
                    else
                    {
                        _currentFrameIndex = 0;
                    }
                }
            }
            else
            {
                break;
            }
            currentFrame = _clip->frameList[_currentFrameIndex];
            
            if (prevFrame)
            {
                _armature->arriveAtFrame(prevFrame, this, true);
            }
            
            _currentFrameDuration = currentFrame->duration;
            _currentFramePosition = currentFrame->position;
            prevFrame = currentFrame;
        }
        
        if (currentFrame)
        {
            _armature->arriveAtFrame(currentFrame, this, false);
        }
    }
}

void DBAnimationState::hideBones()
{
    for (size_t i = 0, l = _clip->hideTimelineList.size(); i < l; ++i)
    {
        DBBone *bone = _armature->getBone(_clip->hideTimelineList[i]);
        
        if (bone)
        {
//            bone->hideSlots();
        }
    }
}

void DBAnimationState::clear()
{
    // reverse delete
    for (size_t i = _timelineStateList.size(); i--;)
    {
        DBTimelineState::returnObject(_timelineStateList[i]);
    }
    
    _timelineStateList.clear();
    _mixingTransforms.clear();
    _armature = nullptr;
    _clip = nullptr;
}

void DBAnimationState::resetTimelineStateList()
{
    for (size_t i = _timelineStateList.size(); i--;)
    {
        DBTimelineState::returnObject(_timelineStateList[i]);
    }
    _timelineStateList.clear();
    
    for (size_t i = _slotTimelineStateList.size(); i--;)
    {
        DBSlotTimelineState::returnObject(_slotTimelineStateList[i]);
    }
    _slotTimelineStateList.clear();
}

NAME_SPACE_DRAGON_BONES_END
