//
//  DBAnimation.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBAnimation.h"
#include "DBArmature.h"

#include "renderer/DBSlot.h"
#include "animation/DBAnimationState.h"
#include "objects/AnimationData.h"

NAME_SPACE_DRAGON_BONES_BEGIN

DBAnimation* DBAnimation::create(DBArmature *armature)
{
    DBAnimation* animation = new (std::nothrow) DBAnimation();
    if (animation && animation->initWithArmature(armature))
    {
        animation->autorelease();
        return animation;
    }
    CC_SAFE_DELETE(animation);
    return nullptr;
}

bool DBAnimation::initWithArmature(dragonBones::DBArmature *armature)
{
    if (!armature)
    {
        return false;
    }
    
    this->_armature = armature;
    return true;
}



bool DBAnimation::getIsPlaying() const
{
    return _isPlaying && !getIsComplete();
}
bool DBAnimation::getIsComplete() const
{
    if (_lastAnimationState)
    {
        if (!_lastAnimationState->_isComplete)
        {
            return false;
        }
        
        for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
        {
            if (!_animationStateList[i]->_isComplete)
            {
                return false;
            }
        }
        
        return true;
    }
    
    return true;
}

DBAnimationState* DBAnimation::getLastAnimationState() const
{
    return _lastAnimationState;
}

const std::vector<std::string>& DBAnimation::getAnimationList() const
{
    return _animationList;
}

float DBAnimation::getTimeScale() const
{
    return _timeScale;
}
void DBAnimation::setTimeScale(float timeScale)
{
    if (timeScale < 0)
    {
        timeScale = 1;
    }
    
    _timeScale = timeScale;
}

const std::vector<AnimationData*>& DBAnimation::getAnimationDataList() const
{
    return _animationDataList;
}
void DBAnimation::setAnimationDataList(const std::vector<AnimationData*> &animationDataList)
{
    _animationDataList = animationDataList;
    _animationList.clear();
    
    for (size_t i = 0, l = _animationDataList.size(); i < l; ++i)
    {
        _animationList.push_back(_animationDataList[i]->name);
    }
}

DBAnimation::DBAnimation() :
_isFading(false)
,_isPlaying(false)
,_timeScale(1.f)
,autoTween(true)
,_armature(nullptr)
,_lastAnimationState(nullptr)
{
    
}

DBAnimation::~DBAnimation()
{
    dispose();
}

void DBAnimation::dispose()
{
    _animationDataList.clear();
    
    for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
    {
        DBAnimationState::returnObject(_animationStateList[i]);
    }
    
    _animationStateList.clear();
    _armature = nullptr;
    _lastAnimationState = nullptr;
}

void DBAnimation::clear()
{
    stop();
    
    for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
    {
        DBAnimationState::returnObject(_animationStateList[i]);
    }
    _animationStateList.clear();
    _lastAnimationState = nullptr;
    
//    for (size_t i = 0, l = _armature->_slotList.size(); i < l; ++i)
//    {
//        Armature *childArmature = _armature->_slotList[i]->getChildArmature();
//        if (childArmature)
//        {
//            childArmature->getAnimation()->clear();
//        }
//    }
}

DBAnimationState* DBAnimation::gotoAndPlay(
                                       const std::string &animationName,
                                       float fadeInTime,
                                       float duration,
                                       int playTimes,
                                       int layer,
                                       const std::string &group,
                                       AnimationFadeOutMode fadeOutMode,
                                       bool pauseFadeOut,
                                       bool pauseFadeIn
                                       )
{
    AnimationData *animationData = nullptr;
    
    for (size_t i = 0, l = _animationDataList.size(); i < l; ++i)
    {
        if (_animationDataList[i]->name == animationName)
        {
            animationData = _animationDataList[i];
            break;
        }
    }
    
    if (!animationData)
    {
        assert (0);
        //throw std::runtime_error("No animation data.");
        return nullptr;
    }
    
    _isPlaying = true;
    _isFading = true;
    fadeInTime = fadeInTime < 0 ? (animationData->fadeTime < 0 ? 0.3f : animationData->fadeTime) : fadeInTime;
    if (fadeInTime <= 0)
    {
        fadeInTime = 0.01f;
    }
    float durationScale;
    
    if (duration < 0)
    {
        durationScale = animationData->scale < 0 ? 1.f : animationData->scale;
    }
    else
    {
        durationScale = duration * 1000.f / animationData->duration;
    }
    
    if (durationScale == 0)
    {
        durationScale = 0.001f;
    }
    
    playTimes = playTimes < 0 ? animationData->playTimes : playTimes;
    
    switch (fadeOutMode)
    {
        case AnimationFadeOutMode::NONE:
            break;
            
        case AnimationFadeOutMode::SAME_LAYER:
            for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
            {
                DBAnimationState *animationState = _animationStateList[i];
                
                if (animationState->_layer == layer)
                {
                    animationState->fadeOut(fadeInTime, pauseFadeOut);
                }
            }
            
            break;
            
        case AnimationFadeOutMode::SAME_GROUP:
            for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
            {
                DBAnimationState *animationState = _animationStateList[i];
                
                if (animationState->_group == group)
                {
                    animationState->fadeOut(fadeInTime, pauseFadeOut);
                }
            }
            
            break;
            
        case AnimationFadeOutMode::ALL:
            for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
            {
                DBAnimationState *animationState = _animationStateList[i];
                animationState->fadeOut(fadeInTime, pauseFadeOut);
            }
            
            break;
            
        case AnimationFadeOutMode::SAME_LAYER_AND_GROUP:
        default:
            for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
            {
                DBAnimationState *animationState = _animationStateList[i];
                
                if (animationState->_layer == layer && animationState->_group == group)
                {
                    animationState->fadeOut(fadeInTime, pauseFadeOut);
                }
            }
            
            break;
    }
    
    _lastAnimationState = DBAnimationState::borrowObject();
    _lastAnimationState->_layer = layer;
    _lastAnimationState->_group = group;
    _lastAnimationState->autoTween = autoTween;
    _lastAnimationState->fadeIn(_armature, animationData, fadeInTime, 1.f / durationScale, playTimes, pauseFadeIn);
    addState(_lastAnimationState);
    
//    for (size_t i = 0, l = _armature->getSlots().size(); i < l; ++i)
    {
//        Slot *slot = _armature->getSlots()[i];
//        if (slot->_childArmature && slot->_childArmature->_isInheritAnimation &&
//            slot->_childArmature->_animation->hasAnimation(animationName))
//        {
//            slot->_childArmature->_animation->gotoAndPlay(animationName, fadeInTime);
//        }
    }
    
    return _lastAnimationState;
}

DBAnimationState* DBAnimation::gotoAndStop(
                                       const std::string &animationName,
                                       float time,
                                       float normalizedTime,
                                       float fadeInTime,
                                       float duration,
                                       int layer,
                                       const std::string &group,
                                       AnimationFadeOutMode fadeOutMode
                                       )
{
    DBAnimationState *animationState = getState(animationName, layer);
    
    if (!animationState)
    {
        animationState = gotoAndPlay(animationName, fadeInTime, duration, -1, layer, group, fadeOutMode);
    }
    
    if (normalizedTime >= 0)
    {
        animationState->setCurrentTime(animationState->getTotalTime() * normalizedTime);
    }
    else
    {
        animationState->setCurrentTime(time);
    }
    
    animationState->stop();
    return animationState;
}

void DBAnimation::play()
{
    if (_animationDataList.empty())
    {
        return;
    }
    
    if (!_lastAnimationState)
    {
        gotoAndPlay(_animationDataList[0]->name);
    }
    else if (!_isPlaying)
    {
        _isPlaying = true;
    }
}

void DBAnimation::stop()
{
    _isPlaying = false;
}

bool DBAnimation::hasAnimation(const std::string &animationName) const
{
    for (size_t i = 0, l = _animationDataList.size(); i < l; ++i)
    {
        if (_animationDataList[i]->name == animationName)
        {
            return true;
        }
    }
    
    return false;
}

DBAnimationState* DBAnimation::getState(const std::string &name, int layer) const
{
    for (size_t i = _animationStateList.size(); i--; )
    {
        DBAnimationState *animationState = _animationStateList[i];
        
        if (animationState->name == name && animationState->_layer == layer)
        {
            return animationState;
        }
    }
    
    return nullptr;
}

void DBAnimation::advanceTime(float passedTime)
{
    if (!_isPlaying)
    {
        return;
    }
    
    bool isFading = false;
    passedTime *= _timeScale;
    
    for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
    {
        DBAnimationState *animationState = _animationStateList[i];
        if (animationState->advanceTime(passedTime))
        {
            removeState(animationState);
            --i;
            --l;
        }
        else if (animationState->_fadeState != DBAnimationState::FadeState::FADE_COMPLETE)
        {
            isFading = true;
        }
    }
    
    _isFading = isFading;
}

void DBAnimation::updateAnimationStates()
{
    for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
    {
        _animationStateList[i]->updateTimelineStates();
    }
}

void DBAnimation::addState(DBAnimationState *animationState)
{
    auto iterator = std::find(_animationStateList.cbegin(), _animationStateList.cend(), animationState);
    
    if (iterator == _animationStateList.cend())
    {
        _animationStateList.push_back(animationState);
    }
}

void DBAnimation::removeState(DBAnimationState *animationState)
{
    auto iterator = std::find(_animationStateList.begin(), _animationStateList.end(), animationState);
    
    if (iterator != _animationStateList.end())
    {
        _animationStateList.erase(iterator);
        DBAnimationState::returnObject(animationState);
        
        if (_lastAnimationState == animationState)
        {
            if (_animationStateList.empty())
            {
                _lastAnimationState = nullptr;
            }
            else
            {
                _lastAnimationState = _animationStateList.back();
            }
        }
    }
}

void DBAnimation::resetAnimationStateList()
{
    for (size_t i = 0, l = _animationStateList.size(); i < l; ++i)
    {
        auto animationState = _animationStateList[i];
        animationState->resetTimelineStateList();
        DBAnimationState::returnObject(animationState);
    }
    _animationStateList.clear();
}

NAME_SPACE_DRAGON_BONES_END
