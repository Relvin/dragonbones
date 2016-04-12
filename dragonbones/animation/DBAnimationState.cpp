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
#include "animation/DBFFDTimelineState.h"
#include "objects/AnimationData.h"
#include "renderer/DBSlot.h"
#include "renderer/DBBone.h"
#include "renderer/DBArmature.h"
#include "events/DBEventData.h"
#include "animation/DBAnimation.h"
#include "objects/MeshData.h"

NAME_SPACE_DRAGON_BONES_BEGIN

DBAnimationState* DBAnimationState::create()
{
    DBAnimationState* animationState = new (std::nothrow) DBAnimationState();
    if (animationState && animationState->init())
    {
        animationState->autorelease();
        return animationState;
    }
    CC_SAFE_DELETE(animationState);
    return nullptr;
}

bool DBAnimationState::init()
{
    this->_timelineStateMgr = DBTimelineStateMgr::create();
    CC_SAFE_RETAIN(_timelineStateMgr);
    this->_slotTimelineStateMgr = DBSlotTimelineStateMgr::create();
    CC_SAFE_RETAIN(_slotTimelineStateMgr);
    this->_ffdTimelineStateMgr = DBFFDTimelineStateMgr::create();
    CC_SAFE_RETAIN(_ffdTimelineStateMgr);
    resetTimelineStateList();
    return true;
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

void DBAnimationState::setLayer(int layer)
{
    this->_layer = layer;
}

void DBAnimationState::setGroup(const std::string &group)
{
    this->_group = group;
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

const DBAnimationState::FadeState DBAnimationState::getFadeState() const
{
    return this->_fadeState;
};

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
,_timelineStateMgr(nullptr)
,_slotTimelineStateMgr (nullptr)
,_ffdTimelineStateMgr(nullptr)
{}
DBAnimationState::~DBAnimationState()
{
    resetTimelineStateList();
    CC_SAFE_RELEASE_NULL(_timelineStateMgr);
    CC_SAFE_RELEASE_NULL(_slotTimelineStateMgr);
    CC_SAFE_RELEASE_NULL(_ffdTimelineStateMgr);
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
    _boneMasks.clear();
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
        cocos2d::Vector<DBTimelineState*> timelineStateList = this->_timelineStateMgr->getAllTimelineState();
        for (size_t i = 0, l = timelineStateList.size(); i < l; ++i)
        {
            timelineStateList.at(i)->fadeOut();
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

DBAnimationState* DBAnimationState::removeAllMixingTransform()
{
    _boneMasks.clear();
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

    for (auto iter = _timelineStateMgr->getUsedVectorBegin();iter != _timelineStateMgr->getUsedVectorEnd(); ++ iter)
    {
        DBTimelineState *timelineState = *iter;
        if (!_armature->getBone(timelineState->getName()))
        {
            iter = _timelineStateMgr->removeTimelineState(iter);
        }
    }
    
    for (auto iter = _slotTimelineStateMgr->getUsedVectorBegin();iter != _slotTimelineStateMgr->getUsedVectorEnd(); ++ iter)
    {
        DBSlotTimelineState *timelineState = *iter;
        if (!_armature->getSlot(timelineState->getName()))
        {
            iter = _slotTimelineStateMgr->removeTimelineState(iter);
        }
    }

    for (auto iter = _ffdTimelineStateMgr->getUsedVectorBegin();iter != _ffdTimelineStateMgr->getUsedVectorEnd(); ++ iter)
    {
        DBFFDTimelineState *timelineState = *iter;
        auto slot = _armature->getSlot(timelineState->getName());
        if (!slot || !slot->getMeshData(timelineState->getName()))
        {
            iter = _ffdTimelineStateMgr->removeTimelineState(iter);
        }
    }
    
    if (_boneMasks.size() > 0)
    {
        
        for (auto iter = _timelineStateMgr->getUsedVectorBegin();iter != _timelineStateMgr->getUsedVectorEnd(); ++ iter)
        {
            DBTimelineState *timelineState = *iter;
            
            auto iterator = std::find(_boneMasks.cbegin(), _boneMasks.cend(), timelineState->getName());
            
            if (iterator == _boneMasks.cend())
            {
                iter = _timelineStateMgr->removeTimelineState(iter);
            }
        }
        
        for (size_t i = 0, l = _boneMasks.size(); i < l; ++i)
        {
            addTimelineState(_boneMasks[i]);
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
    
    for (size_t i = 0, l = _clip->ffdTimelineList.size(); i < l; ++i)
    {
        addFFDTimelineState(_clip->ffdTimelineList[i]->skinName,_clip->ffdTimelineList[i]->slotName,_clip->ffdTimelineList[i]->name);
    }
}

void DBAnimationState::addTimelineState(const std::string &timelineName)
{
    DBBone *bone = _armature->getBone(timelineName);
    if (bone)
    {
        cocos2d::Vector<DBTimelineState*> timelineStateList = this->_timelineStateMgr->getAllTimelineState();
        for (size_t i = 0, l = timelineStateList.size(); i < l; ++i)
        {
            if (timelineStateList.at(i)->getName() == timelineName)
            {
                return;
            }
        }

        DBTimelineState *timelineState = this->_timelineStateMgr->getUnusedTimelineState();
        timelineState->fadeIn(bone, this, _clip->getTimeline(timelineName));
    }
}

void DBAnimationState::removeTimelineState(DBTimelineState *timelineState)
{
    _timelineStateMgr->removeTimelineState(timelineState);
}

void DBAnimationState::addSlotTimelineState(const std::string &timelineName)
{
    DBSlot *slot = _armature->getSlot(timelineName);
    if (slot /*&& slot->getDisplayList().size() > 0*/)
    {
        
        cocos2d::Vector<DBSlotTimelineState*> timelineStateList = this->_slotTimelineStateMgr->getAllTimelineState();
        for (size_t i = 0, l = timelineStateList.size(); i < l; ++i)
        {
            if (timelineStateList.at(i)->getName() == timelineName)
            {
                return;
            }
        }
        DBSlotTimelineState *timelineState = _slotTimelineStateMgr->getUnusedTimelineState();
        timelineState->fadeIn(slot, this, _clip->getSlotTimeline(timelineName));
    }
}

void DBAnimationState::removeSlotTimelineState(DBSlotTimelineState *timelineState)
{
    _slotTimelineStateMgr->removeTimelineState(timelineState);
}

void DBAnimationState::addFFDTimelineState(const std::string &skinName, const std::string & slotName, const std::string &timelineName)
{
    //TODO:换肤
    DBSlot* slot = _armature->getSlot(slotName);
    if(slot /*&& slot.displayList.length > 0*/)
    {
        
        
        cocos2d::Vector<DBFFDTimelineState*> timelineStateList = this->_ffdTimelineStateMgr->getAllTimelineState();
        for (size_t i = 0, l = timelineStateList.size(); i < l; ++i)
        {
            if (timelineStateList.at(i)->getName() == timelineName)
            {
                return;
            }
        }
        MeshData* meshData = slot->getMeshData(timelineName);
        if (meshData)
        {
            DBFFDTimelineState *timelineState = _ffdTimelineStateMgr->getUnusedTimelineState();
            timelineState->fadeIn(meshData, this, _clip->getFFDTimeline(timelineName));
        }
        
    }
}

void DBAnimationState::removeFFDTimelineState(DBFFDTimelineState *timelineState)
{
    _ffdTimelineStateMgr->removeTimelineState(timelineState);
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
        DBEventData::EventType eventDataType;
        
        if (_isFadeOut)
        {
            eventDataType = DBEventData::EventType::FADE_OUT;
        }
        else
        {
            hideBones();
            eventDataType = DBEventData::EventType::FADE_IN;
        }
        

//        if (_armature->_eventDispatcher->hasEvent(eventDataType))
        {
            DBEventData *eventData = _armature->getEventDataManager()->getUnusedEventData();
            eventData->setType(eventDataType);
            eventData->setArmature(_armature);
            eventData->setAnimationState(this);
        }
    }
    
    if (fadeCompleteFlg)
    {
        DBEventData::EventType eventDataType;
        if (_isFadeOut)
        {
            eventDataType = DBEventData::EventType::FADE_OUT_COMPLETE;
        }
        else
        {
            eventDataType = DBEventData::EventType::FADE_IN_COMPLETE;
        }

//        if (_armature->_eventDispatcher->hasEvent(eventDataType))
        {
        
            DBEventData *eventData = _armature->getEventDataManager()->getUnusedEventData();
            eventData->setType(eventDataType);
            eventData->setArmature(_armature);
            eventData->setAnimationState(this);
        
        }
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
    cocos2d::Vector<DBTimelineState*> timelineStateList = this->_timelineStateMgr->getAllTimelineState();
    for (size_t i = 0, l = timelineStateList.size(); i < l; ++i)
    {
        timelineStateList.at(i)->update(progress);
        _isComplete = timelineStateList.at(i)->getIsComplete() && _isComplete;
    }
    
    cocos2d::Vector<DBSlotTimelineState*> slotTimelineStateList = this->_slotTimelineStateMgr->getAllTimelineState();
    for (size_t i = 0, l = slotTimelineStateList.size(); i < l; ++i)
    {
        slotTimelineStateList.at(i)->update(progress);
        _isComplete = slotTimelineStateList.at(i)->getIsComplete() && _isComplete;
    }
    
    cocos2d::Vector<DBFFDTimelineState*> ffdTimelineStateList = this->_ffdTimelineStateMgr->getAllTimelineState();
    for (size_t i = 0, l = ffdTimelineStateList.size(); i < l; ++i)
    {
        ffdTimelineStateList.at(i)->update(progress);
        _isComplete = ffdTimelineStateList.at(i)->getIsComplete() && _isComplete;
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

//        if (_armature->_eventDispatcher->hasEvent(EventData::EventType::START))
        {
            
            DBEventData *eventData = _armature->getEventDataManager()->getUnusedEventData();
            eventData->setType(DBEventData::EventType::START);
            eventData->setArmature(_armature);
            eventData->setAnimationState(this);
        }
    }
    
    if (completeFlg)
    {
//        if (_armature->_eventDispatcher->hasEvent(EventData::EventType::COMPLETE))
        {
            DBEventData *eventData = _armature->getEventDataManager()->getUnusedEventData();
            eventData->setType(DBEventData::EventType::COMPLETE);
            eventData->setArmature(_armature);
            eventData->setAnimationState(this);
            
        }
        
        if (autoFadeOut)
        {
            fadeOut(fadeOutTime, true);
        }
    }
    else if (loopCompleteFlg)
    {
//        if (_armature->_eventDispatcher->hasEvent(EventData::EventType::LOOP_COMPLETE))
        {
            DBEventData *eventData = _armature->getEventDataManager()->getUnusedEventData();
            eventData->setType(DBEventData::EventType::LOOP_COMPLETE);
            eventData->setArmature(_armature);
            eventData->setAnimationState(this);
        }
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
            bone->hideSlots();
        }
    }
}

void DBAnimationState::resetTimelineStateList()
{
    this->_timelineStateMgr->removeAllTimelineState();
    
    this->_slotTimelineStateMgr->removeAllTimelineState();
    this->_ffdTimelineStateMgr->removeAllTimelineState();
    _boneMasks.clear();
    _armature = nullptr;
    _clip = nullptr;
}

NAME_SPACE_DRAGON_BONES_END
