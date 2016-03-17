//
//  DBSlotTimelineState.cpp
//  dragonbones
//
//  Created by Relvin on 16/3/17.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBSlotTimelineState.h"

#include <cstdlib>

#include "renderer/DBSlot.h"
#include "renderer/DBArmature.h"
#include "objects/SlotFrame.h"
#include "objects/SlotTimeline.h"
#include "objects/CurveData.h"
#include "objects/AnimationData.h"
#include "animation/DBAnimation.h"
#include "animation/DBAnimationState.h"

NAME_SPACE_DRAGON_BONES_BEGIN
std::vector<DBSlotTimelineState*> DBSlotTimelineState::_pool;

DBSlotTimelineState* DBSlotTimelineState::borrowObject()
{
    if (_pool.empty())
    {
        return new DBSlotTimelineState();
    }
    
    DBSlotTimelineState *timelinseState = _pool.back();
    _pool.pop_back();
    return timelinseState;
}

void DBSlotTimelineState::returnObject(DBSlotTimelineState *timelineState)
{
    if (std::find(_pool.cbegin(), _pool.cend(), timelineState) == _pool.end())
    {
        _pool.push_back(timelineState);
    }
    
    timelineState->clear();
}

void DBSlotTimelineState::clearObjects()
{
    for (size_t i = 0, l = _pool.size(); i < l; ++i)
    {
        _pool[i]->clear();
        delete _pool[i];
    }
    
    _pool.clear();
}

DBSlotTimelineState::DBSlotTimelineState() : _tweenCurve(nullptr)
{}
DBSlotTimelineState::~DBSlotTimelineState()
{
    clear();
}

void DBSlotTimelineState::fadeIn(DBSlot *slot, DBAnimationState *animationState, SlotTimeline *timelineData)
{
    _slot = slot;
    _armature = dynamic_cast<DBArmature*>( _slot->getArmature());
    if (!_armature)
    {
        CCLOG("DBSlot's armature is null!");
        return;
    }
    _animation = _armature->getAnimation();
    _animationState = animationState;
    _timelineData = timelineData;
    name = timelineData->name;
    
    _totalTime = _timelineData->duration;
    _rawAnimationScale = _animationState->getClip()->scale;
    
    _isComplete = false;
    _blendEnabled = false;
    _tweenColor = false;
    _currentTime = -1;
    _currentFrameIndex = -1;
    _tweenEasing = NO_TWEEN_EASING;
    _weight = 1.f;
    
    switch (_timelineData->frameList.size())
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

void DBSlotTimelineState::update(float progress)
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

void DBSlotTimelineState::updateMultipleFrame(float progress)
{
    int currentPlayTimes = 0;
    progress /= _timelineData->scale;
    progress += _timelineData->offset;
    
    int currentTime = (int)(_totalTime * progress);
    int playTimes = _animationState->getPlayTimes();
    
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
        _currentTime = currentTime;
        SlotFrame *prevFrame = nullptr;
        SlotFrame *currentFrame = nullptr;
        
        for (size_t i = 0, l = _timelineData->frameList.size(); i < l; ++i)
        {
            if (_currentFrameIndex < 0)
            {
                _currentFrameIndex = 0;
            }
            else if (_currentTime < _currentFramePosition || _currentTime >= _currentFramePosition + _currentFrameDuration)
            {
                ++_currentFrameIndex;
                
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
            
            currentFrame = static_cast<SlotFrame*>(_timelineData->frameList[_currentFrameIndex]);
            
            if (prevFrame)
            {
                _slot->arriveAtFrame(prevFrame, this, _animationState, true);
            }
            
            _currentFrameDuration = currentFrame->duration;
            _currentFramePosition = currentFrame->position;
            prevFrame = currentFrame;
        }
        
        if (currentFrame)
        {
            _slot->arriveAtFrame(currentFrame, this, _animationState, false);
            _blendEnabled = currentFrame->displayIndex >= 0;
            
            if (_blendEnabled)
            {
                updateToNextFrame(currentPlayTimes);
            }
            else
            {
                _tweenEasing = NO_TWEEN_EASING;
                _tweenColor = false;
            }
        }
        
        if (_blendEnabled)
        {
            updateTween();
        }
    }
}

void DBSlotTimelineState::updateToNextFrame(int currentPlayTimes)
{
    bool tweenEnabled = false;
    int nextFrameIndex = _currentFrameIndex + 1;
    
    if (nextFrameIndex >= (int)(_timelineData->frameList.size()))
    {
        nextFrameIndex = 0;
    }
    
    const SlotFrame *currentFrame = static_cast<SlotFrame*>(_timelineData->frameList[_currentFrameIndex]);
    const SlotFrame *nextFrame = static_cast<SlotFrame*>(_timelineData->frameList[nextFrameIndex]);
    
    if (
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
    else if (currentFrame->displayIndex < 0 || nextFrame->displayIndex < 0)
    {
        _tweenEasing = NO_TWEEN_EASING;
        tweenEnabled = false;
    }
    else if (_animationState->autoTween)
    {
        _tweenEasing = _animationState->getClip()->tweenEasing;
        
        if (_tweenEasing == USE_FRAME_TWEEN_EASING)
        {
            _tweenEasing = currentFrame->tweenEasing;
            
            if (_tweenEasing == NO_TWEEN_EASING)    // frame no tween
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
        
        if (_tweenEasing == NO_TWEEN_EASING || _tweenEasing == AUTO_TWEEN_EASING)    // frame no tween
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
        // color
        if (currentFrame->color && nextFrame->color)
        {
            _durationColor.alphaOffset = nextFrame->color->alphaOffset - currentFrame->color->alphaOffset;
            _durationColor.redOffset = nextFrame->color->redOffset - currentFrame->color->redOffset;
            _durationColor.greenOffset = nextFrame->color->greenOffset - currentFrame->color->greenOffset;
            _durationColor.blueOffset = nextFrame->color->blueOffset - currentFrame->color->blueOffset;
            _durationColor.alphaMultiplier = nextFrame->color->alphaMultiplier - currentFrame->color->alphaMultiplier;
            _durationColor.redMultiplier = nextFrame->color->redMultiplier - currentFrame->color->redMultiplier;
            _durationColor.greenMultiplier = nextFrame->color->greenMultiplier - currentFrame->color->greenMultiplier;
            _durationColor.blueMultiplier = nextFrame->color->blueMultiplier - currentFrame->color->blueMultiplier;
            
            if (
                _durationColor.alphaOffset ||
                _durationColor.redOffset ||
                _durationColor.greenOffset ||
                _durationColor.blueOffset ||
                _durationColor.alphaMultiplier ||
                _durationColor.redMultiplier ||
                _durationColor.greenMultiplier ||
                _durationColor.blueMultiplier
                )
            {
                _tweenColor = true;
            }
            else
            {
                _tweenColor = false;
            }
        }
        else if (currentFrame->color)
        {
            _tweenColor = true;
            _durationColor.alphaOffset = -currentFrame->color->alphaOffset;
            _durationColor.redOffset = -currentFrame->color->redOffset;
            _durationColor.greenOffset = -currentFrame->color->greenOffset;
            _durationColor.blueOffset = -currentFrame->color->blueOffset;
            _durationColor.alphaMultiplier = 1 - currentFrame->color->alphaMultiplier;
            _durationColor.redMultiplier = 1 - currentFrame->color->redMultiplier;
            _durationColor.greenMultiplier = 1 - currentFrame->color->greenMultiplier;
            _durationColor.blueMultiplier = 1 - currentFrame->color->blueMultiplier;
        }
        else if (nextFrame->color)
        {
            _tweenColor = true;
            _durationColor.alphaOffset = nextFrame->color->alphaOffset;
            _durationColor.redOffset = nextFrame->color->redOffset;
            _durationColor.greenOffset = nextFrame->color->greenOffset;
            _durationColor.blueOffset = nextFrame->color->blueOffset;
            _durationColor.alphaMultiplier = nextFrame->color->alphaMultiplier - 1;
            _durationColor.redMultiplier = nextFrame->color->redMultiplier - 1;
            _durationColor.greenMultiplier = nextFrame->color->greenMultiplier - 1;
            _durationColor.blueMultiplier = nextFrame->color->blueMultiplier - 1;
        }
        else
        {
            _tweenColor = false;
        }
    }
    else
    {
        _tweenColor = false;
    }
    
    if (!_tweenColor && _animationState->displayControl)
    {
        if (currentFrame->color)
        {
            _slot->updateDisplayColor(
                                      currentFrame->color->alphaOffset,
                                      currentFrame->color->redOffset,
                                      currentFrame->color->greenOffset,
                                      currentFrame->color->blueOffset,
                                      currentFrame->color->alphaMultiplier,
                                      currentFrame->color->redMultiplier,
                                      currentFrame->color->greenMultiplier,
                                      currentFrame->color->blueMultiplier,
                                      true
                                      );
        }
        else if (_slot->isColorChanged())
        {
            _slot->updateDisplayColor(0, 0, 0, 0, 1.f, 1.f, 1.f, 1.f, false);
        }
    }
}

void DBSlotTimelineState::updateTween()
{
    const SlotFrame *currentFrame = static_cast<SlotFrame*>(_timelineData->frameList[_currentFrameIndex]);
    if (_tweenColor && _animationState->displayControl)
    {
        float progress = (_currentTime - _currentFramePosition) / (float)(_currentFrameDuration);
        if (!_tweenCurve)
        {
            progress = _tweenCurve->getValueByProgress(progress);
        }
        if (_tweenEasing && _tweenEasing != NO_TWEEN_EASING)
        {
            progress = getEaseValue(progress, _tweenEasing);
        }
        
        if (currentFrame->color)
        {
            _slot->updateDisplayColor(
                                      (int)(currentFrame->color->alphaOffset + _durationColor.alphaOffset * progress),
                                      (int)(currentFrame->color->redOffset + _durationColor.redOffset * progress),
                                      (int)(currentFrame->color->greenOffset + _durationColor.greenOffset * progress),
                                      (int)(currentFrame->color->blueOffset + _durationColor.blueOffset * progress),
                                      currentFrame->color->alphaMultiplier + _durationColor.alphaMultiplier * progress,
                                      currentFrame->color->redMultiplier + _durationColor.redMultiplier * progress,
                                      currentFrame->color->greenMultiplier + _durationColor.greenMultiplier * progress,
                                      currentFrame->color->blueMultiplier + _durationColor.blueMultiplier * progress,
                                      true
                                      );
        }
        else
        {
            _slot->updateDisplayColor(
                                      (int)(_durationColor.alphaOffset * progress),
                                      (int)(_durationColor.redOffset * progress),
                                      (int)(_durationColor.greenOffset * progress),
                                      (int)(_durationColor.blueOffset * progress),
                                      1.f + _durationColor.alphaMultiplier * progress,
                                      1.f + _durationColor.redMultiplier * progress,
                                      1.f + _durationColor.greenMultiplier * progress,
                                      1.f + _durationColor.blueMultiplier * progress,
                                      true
                                      );
        }
    }
}

void DBSlotTimelineState::updateSingleFrame()
{
    SlotFrame *currentFrame = static_cast<SlotFrame*>(_timelineData->frameList.front());
    _slot->arriveAtFrame(currentFrame, this, _animationState, false);
    _isComplete = true;
    _tweenColor = false;
    _tweenEasing = NO_TWEEN_EASING;
    _blendEnabled = currentFrame->displayIndex >= 0;
    
    if (_blendEnabled)
    {
        if (_animationState->displayControl)
        {
            if (currentFrame->color)
            {
                _slot->updateDisplayColor(
                                          currentFrame->color->alphaOffset,
                                          currentFrame->color->redOffset,
                                          currentFrame->color->greenOffset,
                                          currentFrame->color->blueOffset,
                                          currentFrame->color->alphaMultiplier,
                                          currentFrame->color->redMultiplier,
                                          currentFrame->color->greenMultiplier,
                                          currentFrame->color->blueMultiplier,
                                          true
                                          );
            }
            else if (_slot->isColorChanged())
            {
                _slot->updateDisplayColor(0, 0, 0, 0, 1.f, 1.f, 1.f, 1.f, false);
            }
        }
    }
}

void DBSlotTimelineState::clear()
{
    _slot = nullptr;
    _armature = nullptr;
    _animation = nullptr;
    _animationState = nullptr;
    _timelineData = nullptr;
}
NAME_SPACE_DRAGON_BONES_END
