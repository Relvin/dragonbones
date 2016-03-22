//
//  DBBase.hpp
//  dragonbones
//
//  Created by Relvin on 16/2/14.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBBase_hpp
#define DBBase_hpp

#include "DragonBones.h"
#include "geoms/Matrix.h"
#include "geoms/Transform.h"
#include "2d/CCNode.h"

NAME_SPACE_DRAGON_BONES_BEGIN


template <class T>
class TemplateTimeline
{
public:
    
    bool init () {return true;};
    TemplateTimeline ()
    {
        _timelineUsed.clear();
        _timelineFreed.clear();
    }
    
    virtual ~TemplateTimeline()
    {
        _timelineUsed.clear();
        _timelineFreed.clear();
    }
    
    T* getUnusedTimelineState()
    {
        T* timeline = nullptr;
        if (_timelineFreed.size() > 0)
        {
            timeline = _timelineFreed.at(0);
            timeline->retain();
            _timelineFreed.erase(0);
        }
        
        if (!timeline)
        {
            timeline = T::create();
            timeline->retain();
        }
        
        _timelineUsed.pushBack(timeline);
        timeline->release();
        return timeline;
    }
    
    void removeTimelineState(T* timeline)
    {
        if (!timeline)
        {
            return;
        }
        
        timeline->retain();
        _timelineUsed.eraseObject(timeline);
        _timelineFreed.pushBack(timeline);
        timeline->release();
        timeline->resetTimelineState();
    }

    void removeAllTimelineState()
    {
        _timelineFreed.pushBack(_timelineUsed);
        
        for (auto _timeline : _timelineUsed)
        {
            _timeline->resetTimelineState();
        }
        
        _timelineUsed.clear();
    }
    
    cocos2d::Vector<T*>& getAllTimelineState() { return _timelineUsed; }
private:
    /**
     * Used list of DBTimelineStates
     */
    cocos2d::Vector<T*> _timelineUsed;
    /**
     * free list of DBTimelineStates
     */
    cocos2d::Vector<T*> _timelineFreed;
    
};


class DBBase
{
public:
    DBBase();
    virtual ~DBBase();
    virtual void dispose();
    
    virtual cocos2d::Node* getArmature() const;
    virtual cocos2d::Node* getParentBone() const;
    
    virtual void setArmature(cocos2d::Node *armature);
    virtual void setParentBone(cocos2d::Node *bone);
    // change
    virtual void calculateRelativeParentTransform();
    virtual void calculateParentTransform(Transform &transform, Matrix &matrix);
    virtual void updateGlobal(Transform &transform, Matrix &matrix);
    
    bool getInheritRotation();
    void setInheritRotation(bool inheritRotation);
    
    bool getInheritScale();
    void setInheritScale(bool inheritScale);
    
    bool getInheritTranslation();
    void setInheritTranslation(bool inheritTranslation);
    
    const Transform& getGlobalTransform() const;
    void setGlobalTransform(const Transform& global);
    
    const Transform& getOriginTransform() const;
    void setOriginTransform(const Transform& origin);
    
    const Transform& getOffsetTransform() const;
    void setOffsetTransform(const Transform& offset);
    
    const Matrix& getGlobalTransformMatrix() const;
    void setGlobalTransformMatrix(const Matrix& matrix);

    
protected:
    bool inheritRotation;
    bool inheritScale;
    bool inheritTranslation;
    
//    std::string name;
    Transform global;
    Transform origin;
    Transform offset;
    Matrix globalTransformMatrix;
    
    void *userData;
    
    cocos2d::Node *_armature;
    cocos2d::Node *_parentBone;
    
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBBase);
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBBase_hpp */
