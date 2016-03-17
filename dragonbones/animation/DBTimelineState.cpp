//
//  DBTimelineState.cpp
//  dragonbones
//
//  Created by Relvin on 16/3/16.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBTimelineState.h"
#include <cstdlib>
#include "objects/TransformTimeline.h"
#include "objects/CurveData.h"
#include "objects/AnimationData.h"
#include "renderer/DBBone.h"
#include "animation/DBAnimationState.h"

NAME_SPACE_DRAGON_BONES_BEGIN
std::vector<DBTimelineState*> DBTimelineState::_pool;

DBTimelineState* DBTimelineState::borrowObject()
{
    if (_pool.empty())
    {
        return new DBTimelineState();
    }
    
    DBTimelineState *timelinseState = _pool.back();
    _pool.pop_back();
    return timelinseState;
}

void DBTimelineState::returnObject(DBTimelineState *timelineState)
{
    if (std::find(_pool.cbegin(), _pool.cend(), timelineState) == _pool.end())
    {
        _pool.push_back(timelineState);
    }
    
    timelineState->clear();
}

void DBTimelineState::clearObjects()
{
    for (size_t i = 0, l = _pool.size(); i < l; ++i)
    {
        _pool[i]->clear();
        delete _pool[i];
    }
    
    _pool.clear();
}

DBTimelineState::DBTimelineState() :
name("")
,_blendEnabled(false)
,_isComplete(false)
,_tweenTransform(false)
,_tweenScale(false)
,_currentTime(-1)
,_currentFrameIndex(-1)
,_lastTime(0)
,_totalTime(0)
,_weight(1.f)
,_tweenEasing(USE_FRAME_TWEEN_EASING)
,_bone(nullptr)
,_animationState(nullptr)
,_timelineData(nullptr)
,_tweenCurve(nullptr)
{}

DBTimelineState::~DBTimelineState()
{
    clear();
}

void DBTimelineState::fadeIn(DBBone *bone, DBAnimationState *animationState, TransformTimeline *timeline)
{
    _bone = bone;
    _animationState = animationState;
    _timelineData = timeline;
    _totalTime = _timelineData->duration;
    name = _timelineData->name;
    _transform.x = 0.f;
    _transform.y = 0.f;
    _transform.scaleX = 1.f;
    _transform.scaleY = 1.f;
    _transform.skewX = 0.f;
    _transform.skewY = 0.f;
    _pivot.x = 0.f;
    _pivot.y = 0.f;
    _durationTransform.x = 0.f;
    _durationTransform.y = 0.f;
    _durationTransform.scaleX = 0.f;
    _durationTransform.scaleY = 0.f;
    _durationTransform.skewX = 0.f;
    _durationTransform.skewY = 0.f;
    _durationPivot.x = 0.f;
    _durationPivot.y = 0.f;
    // copy
    _originTransform = _timelineData->originTransform;
    // copy
    _originPivot = _timelineData->originPivot;
    
    switch (_timelineData->frameList.size())
    {
        case 0:
            _updateState = UpdateState::UNUPDATE;
            break;
            
        case 1:
            _updateState = UpdateState::UPDATE_ONCE;
            break;
            
        default:
            _updateState = UpdateState::UPDATE;
            break;
    }
    
    _bone->addState(this);
}

void DBTimelineState::fadeOut()
{
    _transform.skewX = formatRadian(_transform.skewX);
    _transform.skewY = formatRadian(_transform.skewY);
}

void DBTimelineState::update(float progress)
{
    if (_updateState == UpdateState::UPDATE)
    {
        updateMultipleFrame(progress);
    }
    else if (_updateState == UpdateState::UPDATE_ONCE)
    {
        updateSingleFrame();
        _updateState = UpdateState::UNUPDATE;
    }
}

void DBTimelineState::updateMultipleFrame(float progress)
{
    progress /= _timelineData->scale;
    progress += _timelineData->offset;
    int currentTime = (int)(_totalTime * progress);
    int currentPlayTimes = 0;
    int playTimes = _animationState->getPlayTimes();
    
    // loop
    if (playTimes == 0)
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
        
        if (currentTime >= totalTimes)
        {
            currentTime = totalTimes;
            _isComplete = true;
        }
        else if (currentTime <= -totalTimes)
        {
            currentTime = -totalTimes;
            _isComplete = true;
        }
        else
        {
            _isComplete = false;
        }
        
        if (currentTime < 0)
        {
            currentTime += totalTimes;
        }
        
        currentPlayTimes = (int)(ceil(currentTime / (float)(_totalTime)));
        
        if (_isComplete)
        {
            currentTime = _totalTime;
        }
        else
        {
            currentTime -= (int)(floor(currentTime / (float)(_totalTime))) * _totalTime;
        }
    }
    
    if (currentPlayTimes == 0)
    {
        currentPlayTimes = 1;
    }
    
    if (_currentTime != currentTime)
    {
        _lastTime = _currentTime;
        _currentTime = currentTime;
        TransformFrame *prevFrame = nullptr;
        TransformFrame *currentFrame = nullptr;
        
        for (size_t i = 0, l = _timelineData->frameList.size(); i < l; ++i)
        {
            if (_currentFrameIndex < 0)
            {
                _currentFrameIndex = 0;
            }
            else if (_currentTime < _currentFramePosition || _currentTime >= _currentFramePosition + _currentFrameDuration || _currentTime < _lastTime)
            {
                ++_currentFrameIndex;
                _lastTime = _currentTime;
                if (_currentFrameIndex >= (int)(l))
                {
                    if (_isComplete)
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
            
            currentFrame = static_cast<TransformFrame*>(_timelineData->frameList[_currentFrameIndex]);
            
            if (prevFrame)
            {
                _bone->arriveAtFrame(prevFrame, this, _animationState, true);
            }
            
            _currentFrameDuration = currentFrame->duration;
            _currentFramePosition = currentFrame->position;
            prevFrame = currentFrame;
        }
        
        if (currentFrame)
        {
            _bone->arriveAtFrame(currentFrame, this, _animationState, false);
            //_blendEnabled = currentFrame->displayIndex >= 0;
            _blendEnabled = currentFrame->tweenEasing != NO_TWEEN_EASING;
            if (_blendEnabled)
            {
                updateToNextFrame(currentPlayTimes);
            }
            else
            {
                _tweenEasing = NO_TWEEN_EASING;
                _tweenTransform = false;
                _tweenScale = false;
            }
        }
        
        if (_blendEnabled)
        {
            updateTween();
        }
    }
}

void DBTimelineState::updateToNextFrame(int currentPlayTimes)
{
    int nextFrameIndex = _currentFrameIndex + 1;
    if (nextFrameIndex >= (int)(_timelineData->frameList.size()))
    {
        nextFrameIndex = 0;
    }
    
    const TransformFrame *currentFrame = static_cast<TransformFrame*>(_timelineData->frameList[_currentFrameIndex]);
    const TransformFrame *nextFrame = static_cast<TransformFrame*>(_timelineData->frameList[nextFrameIndex]);
    bool tweenEnabled = false;
    if (
        nextFrameIndex == 0 &&
        (
         !_animationState->lastFrameAutoTween ||
         (
          _animationState->getPlayTimes() &&
          _animationState->getCurrentPlayTimes() >= _animationState->getPlayTimes() &&
          ((_currentFramePosition + _currentFrameDuration) / _totalTime +
           currentPlayTimes - _timelineData->offset) * _timelineData->scale > 0.999999f
          )
         )
        )
    {
        _tweenEasing = NO_TWEEN_EASING;
        tweenEnabled = false;
    }
    else if (_animationState->autoTween)
    {
        _tweenEasing = _animationState->getClip()->tweenEasing;
        
        //if (_tweenEasing == USE_FRAME_TWEEN_EASING)
        if (_tweenEasing == NO_TWEEN_EASING)
        {
            _tweenEasing = currentFrame->tweenEasing;
            _tweenCurve = currentFrame->curve;
            if ((_tweenEasing == NO_TWEEN_EASING) && (nullptr == _tweenCurve))    // frame no tween
            {
                tweenEnabled = false;
            }
            else
            {
                if (_tweenEasing == AUTO_TWEEN_EASING)
                {
                    _tweenEasing = 0.f;
                }
                
                // _tweenEasing [-1, 0) 0 (0, 1] (1, 2]
                tweenEnabled = true;
            }
        }
        else    // animationData overwrite tween
        {
            // _tweenEasing [-1, 0) 0 (0, 1] (1, 2]
            tweenEnabled = true;
        }
    }
    else
    {
        _tweenEasing = currentFrame->tweenEasing;
        _tweenCurve = currentFrame->curve;
        if ((_tweenEasing == NO_TWEEN_EASING) || ((_tweenEasing == AUTO_TWEEN_EASING) && (_tweenCurve == nullptr)))    // frame no tween
        {
            _tweenEasing = NO_TWEEN_EASING;
            tweenEnabled = false;
        }
        else
        {
            // _tweenEasing [-1, 0) 0 (0, 1] (1, 2]
            tweenEnabled = true;
        }
    }
    
    if (tweenEnabled)
    {
        // transform
        _durationTransform.x = nextFrame->transform.x - currentFrame->transform.x;
        _durationTransform.y = nextFrame->transform.y - currentFrame->transform.y;
        _durationTransform.skewX = nextFrame->transform.skewX - currentFrame->transform.skewX;
        _durationTransform.skewY = nextFrame->transform.skewY - currentFrame->transform.skewY;
        _durationTransform.scaleX = nextFrame->transform.scaleX - currentFrame->transform.scaleX + nextFrame->scaleOffset.x;
        _durationTransform.scaleY = nextFrame->transform.scaleY - currentFrame->transform.scaleY + nextFrame->scaleOffset.y;
        
        // 有问题？
        //_durationTransform.normalizeRotation();
        
        if (nextFrameIndex == 0)
        {
            _durationTransform.skewX = formatRadian(_durationTransform.skewX);
            _durationTransform.skewY = formatRadian(_durationTransform.skewY);
        }
        
        _durationPivot.x = nextFrame->pivot.x - currentFrame->pivot.x;
        _durationPivot.y = nextFrame->pivot.y - currentFrame->pivot.y;
        
        if (
            _durationTransform.x ||
            _durationTransform.y ||
            _durationTransform.skewX ||
            _durationTransform.skewY ||
            _durationTransform.scaleX ||
            _durationTransform.scaleY ||
            _durationPivot.x ||
            _durationPivot.y
            )
        {
            _tweenTransform = true;
            _tweenScale = currentFrame->tweenScale;
        }
        else
        {
            _tweenTransform = false;
            _tweenScale = false;
        }
    }
    else
    {
        _tweenTransform = false;
        _tweenScale = false;
    }
    
    if (!_tweenTransform)
    {
        if (_animationState->additiveBlending)
        {
            _transform.x = currentFrame->transform.x;
            _transform.y = currentFrame->transform.y;
            _transform.skewX = currentFrame->transform.skewX;
            _transform.skewY = currentFrame->transform.skewY;
            _transform.scaleX = currentFrame->transform.scaleX;
            _transform.scaleY = currentFrame->transform.scaleY;
            _pivot.x = currentFrame->pivot.x;
            _pivot.y = currentFrame->pivot.y;
        }
        else
        {
            _transform.x = _originTransform.x + currentFrame->transform.x;
            _transform.y = _originTransform.y + currentFrame->transform.y;
            _transform.skewX = _originTransform.skewX + currentFrame->transform.skewX;
            _transform.skewY = _originTransform.skewY + currentFrame->transform.skewY;
            _transform.scaleX = _originTransform.scaleX * currentFrame->transform.scaleX;
            _transform.scaleY = _originTransform.scaleY * currentFrame->transform.scaleY;
            _pivot.x = _originPivot.x + currentFrame->pivot.x;
            _pivot.y = _originPivot.y + currentFrame->pivot.y;
        }
        
        _bone->invalidUpdate();
    }
    else if (!_tweenScale)
    {
        if (_animationState->additiveBlending)
        {
            _transform.scaleX = currentFrame->transform.scaleX;
            _transform.scaleY = currentFrame->transform.scaleY;
        }
        else
        {
            _transform.scaleX = _originTransform.scaleX * currentFrame->transform.scaleX;
            _transform.scaleY = _originTransform.scaleY * currentFrame->transform.scaleY;
        }
    }
}

void DBTimelineState::updateTween()
{
    const TransformFrame *currentFrame = static_cast<TransformFrame*>(_timelineData->frameList[_currentFrameIndex]);
    if (_tweenTransform)
    {
        float progress = (_currentTime - _currentFramePosition) / (float)(_currentFrameDuration);
        if (_tweenCurve != nullptr)
        {
            progress = _tweenCurve->getValueByProgress(progress);
        }
        
        //if (_tweenEasing && _tweenEasing != NO_TWEEN_EASING)
        //{
        //progress = getEaseValue(progress, _tweenEasing);
        //}
        
        const Transform &currentTransform = currentFrame->transform;
        const Point &currentPivot = currentFrame->pivot;
        if (_animationState->additiveBlending)
        {
            //additive blending
            _transform.x = currentTransform.x + _durationTransform.x * progress;
            _transform.y = currentTransform.y + _durationTransform.y * progress;
            _transform.skewX = currentTransform.skewX + _durationTransform.skewX * progress;
            _transform.skewY = currentTransform.skewY + _durationTransform.skewY * progress;
            
            if (_tweenScale)
            {
                _transform.scaleX = currentTransform.scaleX + _durationTransform.scaleX * progress;
                _transform.scaleY = currentTransform.scaleY + _durationTransform.scaleY * progress;
            }
            
            _pivot.x = currentPivot.x + _durationPivot.x * progress;
            _pivot.y = currentPivot.y + _durationPivot.y * progress;
        }
        else
        {
            // normal blending
            _transform.x = _originTransform.x + currentTransform.x + _durationTransform.x * progress;
            _transform.y = _originTransform.y + currentTransform.y + _durationTransform.y * progress;
            _transform.skewX = _originTransform.skewX + currentTransform.skewX + _durationTransform.skewX * progress;
            _transform.skewY = _originTransform.skewY + currentTransform.skewY + _durationTransform.skewY * progress;
            
            if (_tweenScale)
            {
                _transform.scaleX = _originTransform.scaleX * currentTransform.scaleX + _durationTransform.scaleX * progress;
                _transform.scaleY = _originTransform.scaleY * currentTransform.scaleY + _durationTransform.scaleY * progress;
            }
            
            _pivot.x = _originPivot.x + currentPivot.x + _durationPivot.x * progress;
            _pivot.y = _originPivot.y + currentPivot.y + _durationPivot.y * progress;
        }
        
        _bone->invalidUpdate();
    }
}

void DBTimelineState::updateSingleFrame()
{
    TransformFrame *currentFrame = static_cast<TransformFrame*>(_timelineData->frameList.front());
    _bone->arriveAtFrame(currentFrame, this, _animationState, false);
    _isComplete = true;
    _tweenTransform = false;
    _tweenScale = false;
    //_tweenColor = false;
    _tweenEasing = NO_TWEEN_EASING;
    //_blendEnabled = currentFrame->displayIndex >= 0;
    _blendEnabled = true;
    
    if (_blendEnabled)
    {
        if (_animationState->additiveBlending)
        {
            // additive blending
            // singleFrame.transform (0)
            //_transform.x =
            //    _transform.y =
            //        _transform.skewX =
            //            _transform.skewY =
            //                _transform.scaleX =
            //                    _transform.scaleY = 0.f;
            //_pivot.x =
            //    _pivot.y = 0.f;
            
            _transform.x = currentFrame->transform.x;
            _transform.y = currentFrame->transform.y;
            _transform.skewX = currentFrame->transform.skewX;
            _transform.skewY = currentFrame->transform.skewY;
            _transform.scaleX = currentFrame->transform.scaleX;
            _transform.scaleY = currentFrame->transform.scaleY;
            _pivot.x = currentFrame->pivot.x;
            _pivot.y = currentFrame->pivot.y;
        }
        else
        {
            // normal blending
            // timeline.originTransform + singleFrame.transform (0)
            // copy
            //_transform = _originTransform;
            //// copy
            //_pivot = _originPivot;
            
            _transform.x = _originTransform.x + currentFrame->transform.x;
            _transform.y = _originTransform.y + currentFrame->transform.y;
            _transform.skewX = _originTransform.skewX + currentFrame->transform.skewX;
            _transform.skewY = _originTransform.skewY + currentFrame->transform.skewY;
            _transform.scaleX = _originTransform.scaleX * currentFrame->transform.scaleX;
            _transform.scaleY = _originTransform.scaleY * currentFrame->transform.scaleY;
            _pivot.x = _originPivot.x + currentFrame->pivot.x;
            _pivot.y = _originPivot.y + currentFrame->pivot.y;
        }
        
        _bone->invalidUpdate();
    }
}

void DBTimelineState::clear()
{
    if (_bone)
    {
        _bone->removeState(this);
        _bone = nullptr;
    }
    
    _animationState = nullptr;
    _timelineData = nullptr;
}
NAME_SPACE_DRAGON_BONES_END
