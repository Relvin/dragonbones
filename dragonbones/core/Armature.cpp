#include "Armature.h"
#include "Slot.h"
#include "Bone.h"
#include "events/IEventDispatcher.h"
#include "objects/SkinData.h"
#include "objects/ArmatureData.h"
#include "animation/AnimationState.h"
#include "animation/Animation.h"

NAME_SPACE_DRAGON_BONES_BEGIN

IEventDispatcher *Armature::soundEventDispatcher = nullptr;
//IEventDispatcher* Armature::getEventDispatcher() const
//{
//    return _eventDispatcher;
//}

Armature::Armature(ArmatureData *armatureData, Animation *animation, IEventDispatcher *eventDispatcher, cocos2d::Node *display)
    :_isInheritAnimation(true)
	,_dragonBonesData(nullptr)
	,_armatureData(armatureData)
	,_animation(animation)
	,_eventDispatcher(eventDispatcher)
	,_display(display)
	,_needUpdate(false)
	,_slotsZOrderChanged(false)
	,_delayDispose(false)
	,_lockDispose(false)
	,userData(nullptr)
{
	_animation->_armature = this;
}

Armature::~Armature()
{
    dispose();
}

void Armature::dispose()
{
    _delayDispose = true;
    if(!_animation || _lockDispose)
    {
        return;
    }

    if (_animation)
    {
        _animation->dispose();
        delete _animation;
        _animation = nullptr;
    }
    
    //
    for (size_t i = 0, l = _boneList.size(); i < l; ++i)
    {
        if (_boneList[i])
        {
            _boneList[i]->dispose();
            delete _boneList[i];
            _boneList[i] = nullptr;
        }
    }
    
    //
    for (size_t i = 0, l = _slotList.size(); i < l; ++i)
    {
        if (_slotList[i])
        {
            _slotList[i]->dispose();
            delete _slotList[i];
            _slotList[i] = nullptr;
        }
    }
    
    //
    for (size_t i = 0, l = _eventDataList.size(); i < l; ++i)
    {
        if (_eventDataList[i])
        {
            EventData::returnObject(_eventDataList[i]);
        }
    }
    
    //
    _boneList.clear();
    _slotList.clear();
    _eventDataList.clear();
    
    if (_eventDispatcher)
    {
        _eventDispatcher->dispose();
        delete _eventDispatcher;
        _eventDispatcher = nullptr;
    }
    
    if (_display)
    {
        _display = nullptr;
    }
    
    if (userData)
    {
        delete userData;
        userData = nullptr;
    }
}

bool Armature::sortBone(const std::pair<int, Bone*> &a, const std::pair<int, Bone*> &b)
{
	return a.first > b.first;
}

bool Armature::sortSlot(const Slot *a, const Slot *b)
{
	return a->getZOrder() < b->getZOrder();
}

const std::vector<Bone*>& Armature::getBones() const
{
	return _boneList;
}

const std::vector<Slot*>& Armature::getSlots() const
{
	return _slotList;
}

const ArmatureData* Armature::getArmatureData() const
{
	return _armatureData;
}

Animation* Armature::getAnimation() const
{
	return _animation;
}

cocos2d::Node* Armature::getDisplay() const
{
	return _display;
}

Bone* Armature::getBone(const std::string &boneName) const
{
    if (boneName.empty())
    {
        return nullptr;
    }
    
    for (size_t i = 0, l = _boneList.size(); i < l; ++i)
    {
        if (_boneList[i]->name == boneName)
        {
            return _boneList[i];
        }
    }
    return nullptr;
}

Bone* Armature::getBoneByDisplay(const cocos2d::Node *display) const
{
    if (!display)
    {
        return nullptr;
    }
    
    Slot *slot = getSlotByDisplay(display);
    return slot ? slot->_parentBone : nullptr;
}

void Armature::addBone(Bone *bone)
{
    if (!bone)
    {
        return;
    }
    
    if (bone->_parentBone)
    {
        bone->_parentBone->removeChildBone(bone);
    }
    
    bone->setArmature(this);
}

void Armature::addBone(Bone *bone, const std::string &parentBoneName, bool updateLater)
{
    if (parentBoneName.empty())
    {
        return;
    }
    
    Bone *boneParent = getBone(parentBoneName);
    if (boneParent)
    {
		boneParent->addChildBone(bone, updateLater);
    }
	else
	{
		if (bone->getParentBone())
		{
			bone->getParentBone()->removeChildBone(bone, updateLater);
		}
		bone->setArmature(this);
		if (!updateLater)
		{
			updateAnimationAfterBoneListChanged();
		}
	}
}

void Armature::removeBone(Bone *bone, bool updateLater)
{
    if (!bone || bone->_armature != this)
    {
        return;
    }
    
    if (bone->_parentBone)
    {
        bone->_parentBone->removeChildBone(bone, updateLater);
    }
    else
    {
        bone->setArmature(nullptr);
		if (!updateLater)
		{
			updateAnimationAfterBoneListChanged(false);
		}
    }
}

Bone *Armature::removeBone(const std::string &boneName)
{
    if (boneName.empty())
    {
        return nullptr;
    }
    
    Bone *bone = getBone(boneName);
    if (bone)
    {
        removeBone(bone);
    }
    
    return bone;
}

Slot* Armature::getSlot(const std::string &slotName) const
{
    if (slotName.empty())
    {
        return nullptr;
    }
    
    for (size_t i = 0, l = _slotList.size(); i < l; ++i)
    {
        if (_slotList[i]->name == slotName)
        {
            return _slotList[i];
        }
    }
    
    return nullptr;
}

Slot* Armature::getSlotByDisplay(const cocos2d::Node *display) const
{
	if (display)
	{
		for (size_t i = 0, l = _slotList.size(); i < l; ++i)
		{
			if (_slotList[i]->_display == display)
			{
				return _slotList[i];
			}
		}
	}    
    return nullptr;
}

void Armature::addSlot(Slot *slot, const std::string &parentBoneName)
{
    Bone *bone = getBone(parentBoneName);
    if (bone)
    {
		bone->addSlot(slot);
    }
}

void Armature::removeSlot(Slot *slot)
{
    if (!slot || slot->_armature != this)
    {
        return;
    }
    slot->getParentBone()->removeSlot(slot);
}

Slot* Armature::removeSlotByName(const std::string &slotName)
{
    Slot *slot = getSlot(slotName);    
    if (slot)
    {
        removeSlot(slot);
    }
    
    return slot;
}

void Armature::invalidUpdate(const std::string &boneName)
{
    if (boneName.empty())
    {
		for (size_t i = 0, l = _boneList.size(); i < l; ++i)
		{
			_boneList[i]->invalidUpdate();
		}
    }
	else
	{
		Bone *bone = getBone(boneName);
		if (bone)
		{
			bone->invalidUpdate();
		}
	}
}

void Armature::advanceTime(float passedTime)
{
    _lockDispose = true;
//    _animation->advanceTime(passedTime);
    passedTime *= _animation->_timeScale;
    const bool isFading = _animation->_isFading;
    
    for (size_t i = _boneList.size(); i--;)
    {
		_boneList[i]->update(isFading);
    }
    
    for (size_t i = _slotList.size(); i--;)
    {
        Slot *slot = _slotList[i];
        slot->update();
        
		if (slot->_isShowDisplay && slot->_childArmature)
		{
			slot->_childArmature->advanceTime(passedTime);
		}
	}

	if (_slotsZOrderChanged)
	{
		sortSlotsByZOrder();

#ifdef NEED_Z_ORDER_UPDATED_EVENT
		if (_eventDispatcher->hasEvent(EventData::EventType::Z_ORDER_UPDATED))
		{
			EventData *eventData = new EventData(EventData::EventType::Z_ORDER_UPDATED, this);
			_eventDataList.push_back(eventData);
		}
#endif
	}

	if (!_eventDataList.empty())
	{
		for (size_t i = 0, l = _eventDataList.size(); i < l; ++i)
		{
			_eventDispatcher->dispatchEvent(_eventDataList[i]);
			EventData::returnObject(_eventDataList[i]);
		}

		_eventDataList.clear();
	}
    
    _lockDispose = false;
    
    if (_delayDispose)
    {
        dispose();
    }
}

void Armature::sortBonesList()
{
    if (_boneList.empty())
    {
        return;
    }
    
    std::vector<std::pair<int , Bone*>> sortedList;
    for (size_t i = 0, l = _boneList.size(); i < l; ++i)
    {
        Bone *bone = _boneList[i];
        Bone *parentBone = bone;
        int level = 0;
        
        while (parentBone)
        {
            parentBone = parentBone->_parentBone;
            ++level;
        }
        sortedList.push_back(std::make_pair(level , bone));
    }
    
    std::sort(sortedList.begin() , sortedList.end() , sortBone);    
    for (size_t i = 0, l = sortedList.size(); i < l; ++i)
    {
        _boneList[i] = sortedList[i].second;
    }
}

void Armature::arriveAtFrame(Frame *frame, AnimationState *animationState, bool isCross)
{
    if (!frame->event.empty() && _eventDispatcher->hasEvent(EventData::EventType::ANIMATION_FRAME_EVENT))
    {
        EventData *eventData = EventData::borrowObject(EventData::EventType::ANIMATION_FRAME_EVENT);
        eventData->armature = this;
        eventData->animationState = animationState;
        eventData->frameLabel = frame->event;
        eventData->frame = frame;
        _eventDataList.push_back(eventData);
    }
    
    if (!frame->sound.empty() && soundEventDispatcher && soundEventDispatcher->hasEvent(EventData::EventType::SOUND))
    {
        EventData *eventData = EventData::borrowObject(EventData::EventType::SOUND);
        eventData->armature = this;
        eventData->animationState = animationState;
        eventData->sound = frame->sound;
        soundEventDispatcher->dispatchEvent(eventData);
        EventData::returnObject(eventData);
    }
    
    if (!frame->action.empty())
    {
        if (animationState->displayControl)
        {
            _animation->gotoAndPlay(frame->action);
        }
    }
}

void Armature::resetAnimation()
{
	_animation->stop();
	_animation->resetAnimationStateList();
	for (size_t i = 0, l = _boneList.size(); i < l; ++i)
	{
		_boneList[i]->removeAllStates();
	}
}

void Armature::updateSlotsZOrder()
{
	std::sort(_slotList.begin() , _slotList.end() , sortSlot);
	for (size_t i = 0, l = _slotList.size(); i < l; ++i)
	{
		Slot *slot = _slotList[i];
		if (slot->_isShowDisplay)
		{
			//_display 实际上是container, 这个方法就是把原来的显示对象放到container中的第一个
			slot->addDisplayToContainer(_display, -1);
		}
	}
	_slotsZOrderChanged = false;
}

void Armature::addBoneToBoneList( Bone *bone )
{
	if (bone)
	{
		auto iterator = std::find(_boneList.cbegin(), _boneList.cend(), bone);
		if (iterator == _boneList.cend())
		{
			_boneList.push_back(bone);
            this->getDisplay()->addChild(bone->getDisplayNode(),100);
		}
	}
}

void Armature::removeBoneFromBoneList( Bone *bone )
{
	if (bone)
	{
		auto iterator = std::find(_boneList.cbegin(), _boneList.cend(), bone);
		if (iterator != _boneList.end())
		{
			_boneList.erase(iterator);
		}
	}
}

void Armature::addSlotToSlotList( Slot *slot )
{
	if (slot)
	{
		auto iterator = std::find(_slotList.cbegin(), _slotList.cend(), slot);
		if (iterator == _slotList.end())
		{
			_slotList.push_back(slot);
		}
	}
}

void Armature::removeSlotFromSlotList( Slot *slot )
{
	if (slot)
	{
		auto iterator = std::find(_slotList.cbegin(), _slotList.cend(), slot);
		if (iterator != _slotList.cend())
		{
			_slotList.erase(iterator);
		}
	}
}

void Armature::updateAnimationAfterBoneListChanged( bool ifNeedSortBoneList /*= true*/ )
{
	if (ifNeedSortBoneList)
	{
		sortBonesList();
	}
	_animation->updateAnimationStates();
}

NAME_SPACE_DRAGON_BONES_END
