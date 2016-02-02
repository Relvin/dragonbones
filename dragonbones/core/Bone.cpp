#include "Bone.h"
#include "Armature.h"
#include "Slot.h"
#include "events/IEventDispatcher.h"
#include "events/EventData.h"
#include "animation/Animation.h"
#include "animation/AnimationState.h"
#include "animation/TimelineState.h"
#include "objects/TransformFrame.h"

#include "2d/CCSprite.h"

NAME_SPACE_DRAGON_BONES_BEGIN

// static method
bool Bone::sortState(const TimelineState *a, const TimelineState *b)
{
    return a->_animationState->getLayer() < b->_animationState->getLayer();
}

Bone::Bone() :
	displayController("")
	,applyOffsetTranslationToChild(true)
	,applyOffsetRotationToChild(true)
	,applyOffsetScaleToChild(false)
	,_isColorChanged(false)
	,_needUpdate(2)
    ,_boneNode (nullptr)
    ,_boneData(nullptr)
{
    _tween.scaleX = _tween.scaleY = 1.f;
    inheritRotation = true;
    inheritScale = false;
    
    _boneNode = cocos2d::Node::create();
    _boneNode->retain();
}

Bone::~Bone()
{
    _boneNode->release();
    dispose();
}

void Bone::dispose()
{
    DBObject::dispose();
    _boneList.clear();
    _slotList.clear();
    _timelineStateList.clear();
}

Slot* Bone::getSlot() const
{
	return _slotList.empty() ? nullptr : _slotList.front();
}

const std::vector<Slot*>& Bone::getSlots() const
{
	return _slotList;
}

const std::vector<Bone*>& Bone::getBones() const
{
	return _boneList;
}

void Bone::setVisible(bool visible)
{
	if (_visible != visible)
	{
		_visible = visible;

		for (size_t i = 0, l = _slotList.size(); i < l; ++i)
		{
			_slotList[i]->updateDisplayVisible(_visible);
		}
	}
}

void Bone::setArmature(Armature *armature)
{
	if (_armature == armature) return;
	if (_armature)
	{
		_armature->removeBoneFromBoneList(this);
		_armature->updateAnimationAfterBoneListChanged(false);
	}
	_armature = armature;
	if (_armature)
	{
		_armature->addBoneToBoneList(this);
	}

	for (size_t i = 0, l = _boneList.size(); i < l; ++i)
	{
		_boneList[i]->setArmature(_armature);
	}

	for (size_t i = 0, l = _slotList.size(); i < l; ++i)
	{
		_slotList[i]->setArmature(_armature);
	}
    
}

void Bone::invalidUpdate()
{
    _needUpdate = 2;
}

bool Bone::contains(const DBObject *object) const
{
    if (!object)
    {
        // throw
    }
    
    if (object == this)
    {
        return false;
    }
    
    const DBObject *ancestor = object;    
    while (!(ancestor == this || ancestor == nullptr))
    {
        ancestor = ancestor->getParent();
    }
    return ancestor == this;
}

void Bone::addChildBone(Bone *childBone, bool updateLater)
{
    if (!childBone) return;
    
    if (childBone == this || (childBone && childBone->contains(this)))
    {
        DBASSERT(false, "An Bone cannot be added as a child to itself or one of its children (or children's children, etc.)");
    }

	if (this == childBone->getParent())
	{
		return;
	}

    if (childBone->getParent())
    {
        childBone->getParent()->removeChildBone(childBone);
    }
   
	_boneList.push_back(childBone);
	childBone->setParent(this);
	childBone->setArmature(_armature);

    if (_armature && !updateLater)
    {
		_armature->updateAnimationAfterBoneListChanged();
    }
}

void Bone::removeChildBone(Bone *childBone, bool updateLater)
{
    if (!childBone) return;

    auto iterator = std::find(_boneList.begin(), _boneList.end(), childBone);
    if (iterator != _boneList.end())
    {
        _boneList.erase(iterator);
        childBone->setParent(nullptr);
        childBone->setArmature(nullptr);

		if (_armature && !updateLater)
		{
			_armature->updateAnimationAfterBoneListChanged(false);
		}
    }
}

void Bone::addSlot(Slot *childSlot)
{
	if (!childSlot) return;
	if (childSlot && childSlot->getParent())
	{
		childSlot->getParent()->removeSlot(childSlot);
	}
	
	_slotList.push_back(childSlot);
	childSlot->setParent(this);
	childSlot->setArmature(_armature);
    
}

void Bone::removeSlot(Slot *childSlot)
{
	if (!childSlot) return;
	
	auto iterator = std::find(_slotList.begin(), _slotList.end(), childSlot);
	if (iterator != _slotList.end())
	{
		_slotList.erase(iterator);
		childSlot->setParent(nullptr);
		childSlot->setArmature(nullptr);
	}
}

void Bone::update(bool needUpdate)
{
    
    static int boneCount = 0;
    _needUpdate --;
    if (needUpdate || _needUpdate > 0 || (_parent && _parent->_needUpdate > 0))
    {
        _needUpdate = 1;
    }
    else
    {
        return;
    }
    
    this->_boneNode->setPosition(global.x,-global.y);
    cocos2d::Sprite* boneSprite = dynamic_cast<cocos2d::Sprite*>(this->_boneNode->getChildByTag(1001));
    if (!boneSprite)
    {
        boneSprite = cocos2d::Sprite::create("res/bone.png");
        this->_boneNode->addChild(boneSprite,100);
        boneSprite->setTag(1001);
        boneSprite->setAnchorPoint(cocos2d::Vec2(0,0));
        boneSprite->setColor(cocos2d::Color3B(172,172,172));
        boneCount++;
        printf("bone count = %d\n",boneCount);
    }
    if(_boneData->length)
    {
        boneSprite->setScaleX(_boneData->length / 15.0f);
        
    }
    boneSprite->setRotationSkewX(global.skewX * RADIAN_TO_ANGLE);
    boneSprite->setRotationSkewY(global.skewY * RADIAN_TO_ANGLE);
    

    
    blendingTimeline();

	// 计算global
	Transform parentGlobalTransform;
	Matrix parentGlobalTransformMatrix;
	updateGlobal(parentGlobalTransform, parentGlobalTransformMatrix);

	// 计算globalForChild
	bool ifExistOffsetTranslation = offset.x != 0 || offset.y != 0;
	bool ifExistOffsetScale = offset.scaleX != 1 || offset.scaleY != 1;
	bool ifExistOffsetRotation = offset.skewX != 0 || offset.skewY != 0;

	if(	(!ifExistOffsetTranslation || applyOffsetTranslationToChild) &&
		(!ifExistOffsetScale || applyOffsetScaleToChild) &&
		(!ifExistOffsetRotation || applyOffsetRotationToChild))
	{
		_globalTransformForChild = global;
		_globalTransformMatrixForChild = globalTransformMatrix;
	}
	else
	{
		//if(!_tempGlobalTransformForChild)
		//{
		//	_tempGlobalTransformForChild = new DBTransform();
		//}
		//_globalTransformForChild = _tempGlobalTransformForChild;

		//if(!_tempGlobalTransformMatrixForChild)
		//{
		//	_tempGlobalTransformMatrixForChild = new Matrix();
		//}
		//_globalTransformMatrixForChild = _tempGlobalTransformMatrixForChild;

		_globalTransformForChild.x = origin.x + _tween.x;
		_globalTransformForChild.y = origin.y + _tween.y;
		_globalTransformForChild.scaleX = origin.scaleX * _tween.scaleX;
		_globalTransformForChild.scaleY = origin.scaleY * _tween.scaleY;
		_globalTransformForChild.skewX = origin.skewX + _tween.skewX;
		_globalTransformForChild.skewY = origin.skewY + _tween.skewY;

		if(applyOffsetTranslationToChild)
		{
			_globalTransformForChild.x += offset.x;
			_globalTransformForChild.y += offset.y;
		}
		if(applyOffsetScaleToChild)
		{
			_globalTransformForChild.scaleX *= offset.scaleX;
			_globalTransformForChild.scaleY *= offset.scaleY;
		}
		if(applyOffsetRotationToChild)
		{
			_globalTransformForChild.skewX += offset.skewX;
			_globalTransformForChild.skewY += offset.skewY;
		}

		_globalTransformForChild.toMatrix(_globalTransformMatrixForChild, true);

		Transform::matrixToTransform(
			_globalTransformMatrixForChild, 
			_globalTransformForChild,
			_globalTransformForChild.scaleX * parentGlobalTransform.scaleX >= 0,
			_globalTransformForChild.scaleY * parentGlobalTransform.scaleY >= 0);
	}

}

void Bone::hideSlots()
{
    for (size_t i = 0, l = _slotList.size(); i < l; ++i)
    {
        _slotList[i]->changeDisplay(-1);
    }
}

void Bone::arriveAtFrame(TransformFrame *frame, const TimelineState *timelineState, AnimationState *animationState, bool isCross)
{
    // TODO:
    bool displayControl =
        animationState->displayControl &&
        (displayController.empty() || displayController == animationState->name) &&
		animationState->getMixingTransform(name);
        
    // && timelineState->_weight > 0
    // TODO: 需要修正混合动画干扰关键帧数据的问题，如何正确高效的判断混合动画？
    if (displayControl)
    {
        
        if (!frame->event.empty() && _armature->_eventDispatcher->hasEvent(EventData::EventType::BONE_FRAME_EVENT))
        {
            EventData *eventData = EventData::borrowObject(EventData::EventType::BONE_FRAME_EVENT);
            eventData->armature = _armature;
            eventData->bone = this;
            eventData->animationState = animationState;
            eventData->frameLabel = frame->event;
            eventData->frame = frame;
            _armature->_eventDataList.push_back(eventData);
        }
        
        if (!frame->sound.empty() && Armature::soundEventDispatcher && Armature::soundEventDispatcher->hasEvent(EventData::EventType::SOUND))
        {
            EventData *eventData = EventData::borrowObject(EventData::EventType::SOUND);
            eventData->armature = _armature;
            eventData->bone = this;
            eventData->animationState = animationState;
            eventData->sound = frame->sound;
            Armature::soundEventDispatcher->dispatchEvent(eventData);
            EventData::returnObject(eventData);
        }
        
        if (!frame->action.empty())
        {
            for (size_t i = 0, l = _slotList.size(); i < l; ++i)
            {
                if (_slotList[i]->_childArmature)
                {
                    _slotList[i]->_childArmature->_animation->gotoAndPlay(frame->action);
                }
            }
        }
    }
}

void Bone::addState(TimelineState *timelineState)
{
    auto iterator = std::find(_timelineStateList.cbegin(), _timelineStateList.cend(), timelineState);
    
    if (iterator == _timelineStateList.cend())
    {
        _timelineStateList.push_back(timelineState);
        std::sort(_timelineStateList.begin() , _timelineStateList.end() , sortState);
    }
}

void Bone::removeState(TimelineState *timelineState)
{
    auto iterator = std::find(_timelineStateList.begin(), _timelineStateList.end(), timelineState);
    
    if (iterator != _timelineStateList.end())
    {
        _timelineStateList.erase(iterator);
    }
}

void Bone::blendingTimeline()
{
    size_t i = _timelineStateList.size();    
    if (i == 1)
    {
        TimelineState *timelineState = _timelineStateList[0];
        const Transform &transform = timelineState->_transform;
        const Point &pivot = timelineState->_pivot;
        timelineState->_weight = timelineState->_animationState->getCurrentWeight();
        const float weight = timelineState->_weight;
        _tween.x = transform.x * weight;
        _tween.y = transform.y * weight;
        _tween.skewX = transform.skewX * weight;
        _tween.skewY = transform.skewY * weight;
        _tween.scaleX = 1 + (transform.scaleX - 1) * weight;
        _tween.scaleY = 1 + (transform.scaleY - 1) * weight;
        _tweenPivot.x = pivot.x * weight;
        _tweenPivot.y = pivot.y * weight;
    }
    else if (i > 1)
    {
        int prevLayer = _timelineStateList[i - 1]->_animationState->getLayer();
        int currentLayer = 0;
        float weigthLeft = 1.f;
        float layerTotalWeight = 0.f;
        float x = 0.f;
        float y = 0.f;
        float skewX = 0.f;
        float skewY = 0.f;
        float scaleX = 1.f;
        float scaleY = 1.f;
        float pivotX = 0.f;
        float pivotY = 0.f;
        
        while (i--)
        {
            TimelineState *timelineState = _timelineStateList[i];
            currentLayer = timelineState->_animationState->getLayer();
            
            if (prevLayer != currentLayer)
            {
                if (layerTotalWeight >= weigthLeft)
                {
                    timelineState->_weight = 0;
                    break;
                }
                else
                {
                    weigthLeft -= layerTotalWeight;
                }
            }
            
            prevLayer = currentLayer;
            timelineState->_weight = timelineState->_animationState->getCurrentWeight() * weigthLeft;
            const float weight = timelineState->_weight;
            
            //timelineState
            if (weight && timelineState->_blendEnabled)
            {
                const Transform &transform = timelineState->_transform;
                const Point &pivot = timelineState->_pivot;
                x += transform.x * weight;
                y += transform.y * weight;
                skewX += transform.skewX * weight;
                skewY += transform.skewY * weight;
                scaleX += (transform.scaleX - 1) * weight;
                scaleY += (transform.scaleY - 1) * weight;
                pivotX += pivot.x * weight;
                pivotY += pivot.y * weight;
                layerTotalWeight += weight;
            }
        }
        
        _tween.x = x;
        _tween.y = y;
        _tween.skewX = skewX;
        _tween.skewY = skewY;
        _tween.scaleX = scaleX;
        _tween.scaleY = scaleY;
        _tweenPivot.x = pivotX;
        _tweenPivot.y = pivotY;
    }
}

void Bone::calculateRelativeParentTransform()
{
	global.scaleX = origin.scaleX * _tween.scaleX * offset.scaleX;
	global.scaleY = origin.scaleY * _tween.scaleY * offset.scaleY;
	global.skewX = origin.skewX + _tween.skewX + offset.skewX;
	global.skewY = origin.skewY + _tween.skewY + offset.skewY;
	global.x = origin.x + _tween.x + offset.x;
	global.y = origin.y + _tween.y + offset.y;
}

void Bone::removeAllStates()
{
	_timelineStateList.clear();
}

cocos2d::Node* Bone::getDisplayNode()
{
    return this->_boneNode;
}

NAME_SPACE_DRAGON_BONES_END
