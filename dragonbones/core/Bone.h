#ifndef DRAGONBONES_CORE_BONE_H
#define DRAGONBONES_CORE_BONE_H

#include "DragonBones.h"
#include "DBObject.h"
#include "2d/CCNode.h"
#include "objects/BoneData.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class TimelineState;
class Slot;
class AnimationState;
class TransformFrame;

class Bone : public DBObject
{
	friend class Armature;
	friend class Slot;
	friend class AnimationState;
	friend class TimelineState;

public:
	Bone();
	virtual ~Bone();
	virtual void dispose() override;

	virtual void invalidUpdate();
	virtual bool contains(const DBObject *object) const;
	virtual void addChildBone(Bone *childBone, bool updateLater = false);
	virtual void removeChildBone(Bone *childBone, bool updateLater = false);

	virtual void addSlot(Slot *childSlot);
	virtual void removeSlot(Slot *childSlot);
	virtual Slot* getSlot() const;
	virtual const std::vector<Slot*>& getSlots() const;
	virtual const std::vector<Bone*>& getBones() const;
	virtual void setBoneVisible(bool vislble) override;

	virtual void removeAllStates();
    cocos2d::Node* getDisplayNode();
    
    void setBoneData (BoneData *boneData);

protected:
	virtual void setArmature(Armature *armature) override;
	virtual void update(bool needUpdate = false);
	virtual void hideSlots();
	virtual void arriveAtFrame(TransformFrame *frame, const TimelineState *timelineState, AnimationState *animationState, bool isCross);
	virtual void addState(TimelineState *timelineState);
	virtual void removeState(TimelineState *timelineState);
	virtual void blendingTimeline();
	virtual void calculateRelativeParentTransform() override;

private:
	static bool sortState(const TimelineState *a, const TimelineState *b);
    
public:
    std::string displayController;
	bool applyOffsetTranslationToChild;
    bool applyOffsetRotationToChild;
	bool applyOffsetScaleToChild;
	
	Transform _globalTransformForChild;
	Matrix _globalTransformMatrixForChild;

protected:
    std::vector<Bone*> _boneList;
    std::vector<Slot*> _slotList;
    std::vector<TimelineState*> _timelineStateList;

	int _needUpdate;
	bool _isColorChanged;

	Point _tweenPivot;
	Transform _tween;

private:
	Transform _tempGlobalTransformForChild;
	Matrix _tempGlobalTransformMatrixForChild;
    cocos2d::Node *_boneNode;
    
    BoneData * _boneData;

private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(Bone);
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_CORE_BONE_H
