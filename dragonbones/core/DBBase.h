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
class MultData {
public:
    bool init () {return true;};
    MultData ()
    {
        _dataUsed.clear();
        _dataFreed.clear();
    }
    
    virtual ~MultData()
    {
        _dataUsed.clear();
        _dataFreed.clear();
    }
    
    typename cocos2d::Vector<T*>::iterator getUsedVectorBegin()
    {
        return _dataUsed.begin();
    }
    
    typename cocos2d::Vector<T*>::iterator getUsedVectorEnd()
    {
        return _dataUsed.end();
    }
    
    bool empty()
    {
        return _dataUsed.empty();
    }
    
    T* getUnusedData()
    {
        T* data = nullptr;
        if (_dataFreed.size() > 0)
        {
            data = _dataFreed.at(0);
            data->retain();
            _dataFreed.erase(0);
        }
        
        if (!data)
        {
            data = T::create();
            data->retain();
        }
        
        _dataUsed.pushBack(data);
        data->release();
        return data;
    }
    typename cocos2d::Vector<T*>::iterator removeDataFromUsed(typename cocos2d::Vector<T*>::iterator iter)
    {
        T * data = nullptr;
        if (iter != _dataUsed.end())
        {
            data = *iter;
            data->retain();
            iter = _dataUsed.erase(iter);
            _dataFreed.pushBack(data);
            data->release();
        }
        return iter;
    }
    
    void removeDataFromUsed(T* data)
    {
        if (!data)
        {
            return;
        }
        
        data->retain();
        _dataUsed.eraseObject(data);
        _dataFreed.pushBack(data);
        data->release();
    }
    
    void removeAllDataFromUsed()
    {
        _dataFreed.pushBack(_dataUsed);
        _dataUsed.clear();
    }
    
    cocos2d::Vector<T*>& getAllData() { return _dataUsed; }
private:
    /**
     * Used list of DBTimelineStates
     */
    cocos2d::Vector<T*> _dataUsed;
    /**
     * free list of DBTimelineStates
     */
    cocos2d::Vector<T*> _dataFreed;
    
};

template <class T>
class TemplateTimeline
: public MultData<T>
{
public:
    
    bool init () {return true;};
    TemplateTimeline ()
    {

    }
    
    virtual ~TemplateTimeline()
    {
    }
    
    typename cocos2d::Vector<T*>::iterator getUsedVectorBegin()
    {
        return MultData<T>::getUsedVectorBegin();
    }
    
    typename cocos2d::Vector<T*>::iterator getUsedVectorEnd()
    {
        return MultData<T>::getUsedVectorEnd();
    }
    
    T* getUnusedTimelineState()
    {
        return MultData<T>::getUnusedData();
    }
    typename cocos2d::Vector<T*>::iterator removeTimelineState(typename cocos2d::Vector<T*>::iterator iter)
    {
        T * timeline = *iter;
        timeline->resetTimelineState();
        return MultData<T>::removeDataFromUsed(iter);
    }
    
    void removeTimelineState(T* timeline)
    {
        if (!timeline)
        {
            return;
        }
        timeline->resetTimelineState();
        MultData<T>::removeDataFromUsed(timeline);
        
    }
    
    void removeAllTimelineState()
    {
        
        for (auto iter = getUsedVectorBegin(); iter != getUsedVectorEnd();++iter )
        {
            (*iter)->resetTimelineState();
        }
        MultData<T>::removeAllDataFromUsed();
    }
    
    cocos2d::Vector<T*>& getAllTimelineState() { return MultData<T>::getAllData(); }
    
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
    
    Transform& getGlobalTransform() const;
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
    mutable Transform global;
    Transform origin;
    Transform offset;
    Matrix _globalTransformMatrix;
    
    void *userData;
    
    cocos2d::Node *_armature;
    cocos2d::Node *_parentBone;
    
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBBase);
};

NAME_SPACE_DRAGON_BONES_END

#endif /* DBBase_hpp */
