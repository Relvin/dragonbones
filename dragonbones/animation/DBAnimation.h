//
//  DBAnimation.hpp
//  dragonbones
//
//  Created by Relvin on 16/2/4.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBAnimation_hpp
#define DBAnimation_hpp

#include "DragonBones.h"
#include "base/CCRef.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class DBAnimationState;
class AnimationData;
class DBArmature;

class DBArmature;

class DBAnimation
: public cocos2d::Ref
{
    friend class DBArmature;
    friend class DBSlot;
    
    enum class AnimationFadeOutMode {NONE, SAME_LAYER, SAME_GROUP, SAME_LAYER_AND_GROUP, ALL};
    
public:
    static DBAnimation* create(DBArmature *armature);
    
    
    bool initWithArmature(DBArmature *armature);
    
    DBAnimation();
    virtual ~DBAnimation();
    
    
    
    void dispose();
    void clear();
    
    DBAnimationState* gotoAndPlay(
                                const std::string &animationName,
                                float fadeInTime = -1.f,
                                float duration = -1.f,
                                int playTimes = -1,
                                int layer = 0,
                                const std::string &group = "",
                                AnimationFadeOutMode fadeOutMode = AnimationFadeOutMode::SAME_LAYER_AND_GROUP,
                                bool pauseFadeOut = true,
                                bool pauseFadeIn = true
                                );
    
    DBAnimationState* gotoAndStop(
                                const std::string &animationName,
                                float time,
                                float normalizedTime = -1.f,
                                float fadeInTime = 0.f,
                                float duration = -1.f,
                                int layer = 0,
                                const std::string &group = "",
                                AnimationFadeOutMode fadeOutMode = AnimationFadeOutMode::ALL
                                );
    
    void play();
    void stop();
    void advanceTime(float passedTime);
    
    bool hasAnimation(const std::string &animationName) const;
    DBAnimationState* getState(const std::string &name, int layer = 0) const;
    
    bool getIsPlaying() const;
    bool getIsComplete() const;
    const std::vector<std::string>& getAnimationList() const;
    DBAnimationState* getLastAnimationState() const;
    float getTimeScale() const;
    void setTimeScale(float timeScale);
    
    const std::vector<AnimationData*>& getAnimationDataList() const;
    void setAnimationDataList(const std::vector<AnimationData*> &animationDataList);
    
protected:
    void addState(DBAnimationState *animationState);
    void removeState(DBAnimationState *animationState);
    void updateAnimationStates();
    void resetAnimationStateList();
    
public:
    bool autoTween;
    
protected:
    bool _isFading;
    bool _isPlaying;
    float _timeScale;
    
    std::vector<std::string> _animationList;
    std::vector<AnimationData*> _animationDataList;
    
    DBArmature *_armature;
    DBAnimationState *_animationState;
    
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBAnimation);
};
NAME_SPACE_DRAGON_BONES_END

#endif /* DBAnimation_hpp */
