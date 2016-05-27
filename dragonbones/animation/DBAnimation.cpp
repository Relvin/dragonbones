//
//  DBAnimation.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBAnimation.h"
#include "renderer/DBArmature.h"

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
    if (_animationState)
    {
        if (!_animationState->getIsComplete())
        {
            return false;
        }
        
        return true;
    }
    
    return true;
}

DBAnimationState* DBAnimation::getLastAnimationState() const
{
    return _animationState;
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
,_animationState(nullptr)
, _updateAnimationStage(true)
{
    
}

DBAnimation::~DBAnimation()
{
    dispose();
}

void DBAnimation::dispose()
{
    _animationDataList.clear();
    
    CC_SAFE_RELEASE_NULL(_animationState);
    _armature = nullptr;
    _animationState = nullptr;
}

void DBAnimation::clear()
{
    stop();
    
//    for (size_t i = 0, l = _armature->_slotList.size(); i < l; ++i)
//    {
//        Armature *childArmature = _armature->_slotList[i]->getChildArmature();
//        if (childArmature)
//        {
//            childArmature->getAnimation()->clear();
//        }
//    }
}

DBAnimationState* DBAnimation::playOnce(
                           const std::string &animationName
                           )
{
    return this->gotoAndPlay(animationName,-1,-1,1);
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

                if (_animationState && _animationState->getLayer() == layer)
                {
                    _animationState->fadeOut(fadeInTime, pauseFadeOut);
                }
            
            break;
            
        case AnimationFadeOutMode::SAME_GROUP:
                
                if (_animationState && _animationState->getGroup() == group)
                {
                    _animationState->fadeOut(fadeInTime, pauseFadeOut);
                }
            
            break;
            
        case AnimationFadeOutMode::ALL:

                if (_animationState)
                {
                    _animationState->fadeOut(fadeInTime, pauseFadeOut);
                }
                
            
            break;
            
        case AnimationFadeOutMode::SAME_LAYER_AND_GROUP:
        default:

                if (_animationState && _animationState->getLayer() == layer && _animationState->getGroup() == group)
                {
                    _animationState->fadeOut(fadeInTime, pauseFadeOut);
                }
            
            break;
    }
    if (!_animationState)
    {
        _animationState = DBAnimationState::create();
        CC_SAFE_RETAIN(_animationState);
    }
    _updateAnimationStage = true;
    _animationState->resetTimelineStateList();
    _animationState->setLayer(layer);
    _animationState->setGroup(group);
    _animationState->autoTween = autoTween;
    _animationState->fadeIn(_armature, animationData, fadeInTime, 1.f / durationScale, playTimes, pauseFadeIn);
    
    
//    for (size_t i = 0, l = _armature->getSlots().size(); i < l; ++i)
    {
//        Slot *slot = _armature->getSlots()[i];
//        if (slot->_childArmature && slot->_childArmature->_isInheritAnimation &&
//            slot->_childArmature->_animation->hasAnimation(animationName))
//        {
//            slot->_childArmature->_animation->gotoAndPlay(animationName, fadeInTime);
//        }
    }
    
    return _animationState;
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
    
    if (!_animationState)
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
    if (_animationState && _animationState->name == name && _animationState->getLayer() == layer)
    {
        return _animationState;
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
    
    if (_animationState && _updateAnimationStage)
    {
        if (_animationState->advanceTime(passedTime))
        {
            _updateAnimationStage = false;
        }
        else if (_animationState->getFadeState() != DBAnimationState::FadeState::FADE_COMPLETE)
        {
            isFading = true;
        }
    }
    
    
    _isFading = isFading;
}

void DBAnimation::updateAnimationStates()
{
    if (_animationState)
    {
        _animationState->updateTimelineStates();
    }
}

void DBAnimation::addState(DBAnimationState *animationState)
{
}

void DBAnimation::removeState(DBAnimationState *animationState)
{
    
}

void DBAnimation::resetAnimationStateList()
{
    if (_animationState)
    {
        _animationState->resetTimelineStateList();
//        DBAnimationState::returnObject(animationState);
    }
}

NAME_SPACE_DRAGON_BONES_END
