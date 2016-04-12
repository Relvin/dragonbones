//
//  DBFFDTimeline.cpp
//  dragonbones
//
//  Created by Relvin on 16/4/6.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBFFDTimelineState.h"
#include "animation/DBAnimation.h"
#include "objects/CurveData.h"
#include "animation/DBAnimationState.h"
#include "renderer/DBArmature.h"
#include "renderer/DBSlot.h"
#include "objects/MeshData.h"
#include "objects/FFDTimeline.h"
#include "objects/FFDFrame.h"

NAME_SPACE_DRAGON_BONES_BEGIN

DBFFDTimelineStateMgr::DBFFDTimelineStateMgr()
{
    
}

DBFFDTimelineStateMgr::~DBFFDTimelineStateMgr()
{
    
}

DBFFDTimelineStateMgr* DBFFDTimelineStateMgr::create()
{
    DBFFDTimelineStateMgr *timelineStataMgr = new (std::nothrow) DBFFDTimelineStateMgr();
    if (timelineStataMgr && timelineStataMgr->init())
    {
        timelineStataMgr->autorelease();
        return timelineStataMgr;
    }
    CC_SAFE_DELETE(timelineStataMgr);
    return nullptr;
}

bool DBFFDTimelineStateMgr::init()
{
    if (!TemplateTimeline<DBFFDTimelineState>::init())
    {
        return false;
    }
    return true;
}

DBFFDTimelineState* DBFFDTimelineState::create()
{
    DBFFDTimelineState *timelineState = new (std::nothrow) DBFFDTimelineState();
    if (timelineState && timelineState->init())
    {
        timelineState->autorelease();
        return timelineState;
    }
    CC_SAFE_DELETE(timelineState);
    return nullptr;
}

bool DBFFDTimelineState::init()
{
    return true;
}

DBFFDTimelineState::DBFFDTimelineState()
: name ("")
, slotName("")
, skinName("")
, _weight(1.f)
, _blendEnabled(false)
, _isComplete(false)
, _animationState(nullptr)
, _totalTime(0)
, _currentTime(0)
, _currentFrameIndex(0)
, _currentFramePosition(0)
, _currentFrameDuration(0)
, _tweenEasing(0.f)
, _tweenCurve(nullptr)
, _tweenVertices (false)
, _offset(0)
, _rawAnimationScale(0.f)
, _updateMode(0)
, _armature(nullptr)
, _animation(nullptr)
, _slot(nullptr)
, _mesh(nullptr)
, _timelineData(nullptr)
{
    _durationVertices.clear();
    _updateVertices.clear();
}

DBFFDTimelineState::~DBFFDTimelineState()
{
    _durationVertices.clear();
    _updateVertices.clear();
}

void DBFFDTimelineState::resetTimelineState()
{
    _slot = nullptr;
    _mesh = nullptr;
    _armature = nullptr;
    _animation = nullptr;
    _animationState = nullptr;
    _timelineData = nullptr;
    _durationVertices.clear();
    _updateVertices.clear();
}

void DBFFDTimelineState::fadeIn(dragonBones::MeshData *mesh, dragonBones::DBAnimationState *animationState, dragonBones::FFDTimeline *timelineData)
{
    _mesh = mesh;
    //_armature = _slot.armature;
    //_animation = _armature.animation;
    _animationState = animationState;
    _timelineData = timelineData;
    
    name = timelineData->name;
    slotName = timelineData->slotName;
    skinName = timelineData->skinName;
    
    _totalTime = _timelineData->duration;
    _rawAnimationScale = _animationState->getClip()->scale;
    
    _isComplete = false;
    _blendEnabled = false;
    
    _currentFrameIndex = -1;
    _currentTime = -1;
    _tweenEasing = NO_TWEEN_EASING;
    _weight = 1;
    switch(_timelineData->frameList.size())
    {
        case 0:
            _updateMode = 0;
            break;
        case 1:
            _updateMode = 1;
            break;
        default:
            _updateMode = -1;
            break;
    }

}

void DBFFDTimelineState::update(float progress)
{
    if (_updateMode == -1)
    {
        updateMultipleFrame(progress);
    }
    else if (_updateMode == 1)
    {
        _updateMode = 0;
        updateSingleFrame();
    }
}

void DBFFDTimelineState::updateMultipleFrame(float progress)
{
    int currentPlayTimes = 0;
    progress /= _timelineData->scale;
    progress += _timelineData->offset;
    
    int currentTime = (int)(_totalTime * progress);
    int playTimes = _animationState->getPlayTimes();
    
    if(playTimes == 0)
    {
        _isComplete = false;
        currentPlayTimes = (int)(ceil(abs(currentTime) / (float)(_totalTime)));
        currentTime -= (int)(floor(currentTime / (float)(_totalTime))) * _totalTime;
        
        if (currentTime < 0)
        {
            currentTime += _totalTime;
        }
    }
    else
    {
        int totalTimes = playTimes * _totalTime;
        if(currentTime >= totalTimes)
        {
            currentTime = totalTimes;
            _isComplete = true;
            _mesh->updated = false;
        }
        else if(currentTime <= -totalTimes)
        {
            currentTime = -totalTimes;
            _isComplete = true;
            _mesh->updated = false;
        }
        else
        {
            _isComplete = false;
        }
        
        if(currentTime < 0)
        {
            currentTime += totalTimes;
        }
        
        currentPlayTimes = (int)(ceil(currentTime / (float)(_totalTime)));
        if(_isComplete)
        {
            currentTime = _totalTime;
        }
        else
        {
            currentTime -= (int)(floor(currentTime / (float)(_totalTime))) * _totalTime;
        }
    }
    
    currentPlayTimes = currentPlayTimes == 0 ? 1 : currentPlayTimes;
    
    if(_currentTime != currentTime)
    {
        _currentTime = currentTime;
        FFDFrame* prevFrame = nullptr;
        FFDFrame* currentFrame = nullptr;
        
        for (size_t i = 0, l = _timelineData->frameList.size(); i < l; ++i)
        {
            if(_currentFrameIndex < 0)
            {
                _currentFrameIndex = 0;
            }
            else if(_currentTime < _currentFramePosition || _currentTime >= _currentFramePosition + _currentFrameDuration)
            {
                ++_currentFrameIndex;
                
                if (_currentFrameIndex >= (int)(l))
                {
                    if(_isComplete)
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
            currentFrame = static_cast<FFDFrame*>(_timelineData->frameList[_currentFrameIndex]);
            
            _currentFrameDuration = currentFrame->duration;
            _currentFramePosition = currentFrame->position;
            prevFrame = currentFrame;
        }
        
        if(currentFrame)
        {
            updateToNextFrame(currentPlayTimes);
        }
        
        updateTween();
    }
    else
    {
        updateTween();
    }
}

void DBFFDTimelineState::updateToNextFrame(int currentPlayTimes)
{
    bool tweenEnabled = false;
    int nextFrameIndex = _currentFrameIndex + 1;
    
    if (nextFrameIndex >= (int)(_timelineData->frameList.size()))
    {
        nextFrameIndex = 0;
    }
    
    const FFDFrame *currentFrame = static_cast<FFDFrame*>(_timelineData->frameList[_currentFrameIndex]);
    const FFDFrame *nextFrame = static_cast<FFDFrame*>(_timelineData->frameList[nextFrameIndex]);

    if(
       nextFrameIndex == 0 &&
       (
        !_animationState->lastFrameAutoTween ||
        (
         _animationState->getPlayTimes() &&
         _animationState->getCurrentPlayTimes() >= _animationState->getPlayTimes() &&
         ((_currentFramePosition + _currentFrameDuration) / _totalTime + currentPlayTimes - _timelineData->offset) * _timelineData->scale > 0.999999f
         )
        )
       )
    {
        _tweenEasing = NO_TWEEN_EASING;
        tweenEnabled = false;
    }
    else if(_animationState->autoTween)
    {
        _tweenEasing = _animationState->getClip()->tweenEasing;
        if(_tweenEasing == NO_TWEEN_EASING)
        {
            _tweenEasing = currentFrame->tweenEasing;
            _tweenCurve = currentFrame->curve;
            if(_tweenEasing == NO_TWEEN_EASING && _tweenCurve == nullptr)    //frame no tween
            {
                tweenEnabled = false;
            }
            else
            {
                if(_tweenEasing == AUTO_TWEEN_EASING)
                {
                    _tweenEasing = 0.f;
                }
                tweenEnabled = true;
            }
        }
        else
        {
            tweenEnabled = true;
        }
    }
    else
    {
        _tweenEasing = currentFrame->tweenEasing;
        _tweenCurve = currentFrame->curve;
        if((_tweenEasing == NO_TWEEN_EASING || _tweenEasing == AUTO_TWEEN_EASING) && _tweenCurve == nullptr)   //frame no tween
        {
            _tweenEasing = NO_TWEEN_EASING;
            tweenEnabled = false;
        }
        else
        {
            tweenEnabled = true;
        }
    }
    
    if(tweenEnabled)
    {
        _offset = currentFrame->offset < nextFrame->offset ? currentFrame->offset : nextFrame->offset;
        int end = currentFrame->offset + currentFrame->vertices.size() > nextFrame->offset + nextFrame->vertices.size() ?
        currentFrame->offset + currentFrame->vertices.size() :
        nextFrame->offset + nextFrame->vertices.size();
        
        float curVertex = 0.f, nextVertex = 0.f;
        _tweenVertices = false;
        _durationVertices.clear();
        for (int i = _offset; i < end; i++)
        {
            curVertex = 0;
            nextVertex = 0;
            if (currentFrame->offset <= i && currentFrame->vertices.size() + currentFrame->offset > i)
            {
                curVertex = currentFrame->vertices[i - currentFrame->offset];
            }
            if (nextFrame->offset <= i && nextFrame->vertices.size() + nextFrame->offset > i)
            {
                nextVertex = nextFrame->vertices[i - nextFrame->offset];
            }
            _durationVertices.push_back(nextVertex - curVertex);
            if ((_durationVertices[i - _offset] < - EPSINON) || (_durationVertices[i - _offset] > EPSINON))
            {
                _tweenVertices = true;
            }
        }
    }
    else
    {
        _tweenVertices = false;
    }
}

void DBFFDTimelineState::updateTween()
{
    FFDFrame* currentFrame = dynamic_cast<FFDFrame *>(_timelineData->frameList[_currentFrameIndex]);
    if( _animationState->displayControl)
    {
        float progress = (_currentTime - _currentFramePosition) / (float)(_currentFrameDuration);
        if (_tweenCurve != nullptr)
        {
            progress = _tweenCurve->getValueByProgress(progress);
        }
        if (_tweenEasing && _tweenEasing != NO_TWEEN_EASING)
        {
            progress = getEaseValue(progress, _tweenEasing);
        }
        
        int end = _offset + _durationVertices.size();
        
        float curVertex = 0.f;
        _updateVertices.clear();
        for (int i = _offset; i < end; i++)
        {
            curVertex = 0;
            if (currentFrame->offset <= i && currentFrame->vertices.size() + currentFrame->offset > i)
            {
                curVertex = currentFrame->vertices[i - currentFrame->offset];
            }
            _updateVertices.push_back(curVertex + _durationVertices[i - _offset] * progress);
            
        }
        _mesh->updateVertices(_offset, _updateVertices);
    }

}

void DBFFDTimelineState::updateSingleFrame()
{
    FFDFrame* currentFrame = dynamic_cast<FFDFrame *>(_timelineData->frameList[0]);
    _isComplete = true;
    _tweenEasing = NO_TWEEN_EASING;
    _tweenVertices = false;
    
    if(_animationState->displayControl)
    {
        _mesh->updateVertices(currentFrame->offset, currentFrame->vertices);
    }
}


NAME_SPACE_DRAGON_BONES_END