#ifndef DRAGONBONES_CORE_SLOT_H
#define DRAGONBONES_CORE_SLOT_H

#include "DBObject.h"
#include "DragonBones.h"
#include "geoms/Rectangle.h"
#include "geoms/ColorTransform.h"

#include "2d/CCNode.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class Frame;
class SlotData;
class AnimationState;
class SlotTimelineState;

class Slot : public DBObject
{    
	friend class Armature;
	friend class Bone;
	friend class Animation;
	friend class BaseFactory;
	friend class SlotTimelineState;

public:
	Slot(SlotData *slotData);
	virtual ~Slot();
	virtual void dispose() override;

	virtual Rectangle getBoundingBox() = 0;

	virtual int getDisplayIndex() const;
	bool isShowDisplay() const { return _isShowDisplay; }
	bool isColorChanged() const { return _isColorChanged; }

	virtual SlotData* getSlotData() const;

	virtual float getZOrder() const;
	virtual void setZOrder(float zorder);

	virtual cocos2d::Node* getDisplay() const;
	virtual void setDisplay(cocos2d::Node *display, const DisplayType displayType = DisplayType::DT_IMAGE, bool disposeExisting = true);

	virtual Armature* getChildArmature() const;
	virtual void setChildArmature(Armature *childArmature, bool disposeExisting = true);

	virtual const std::vector<std::pair<cocos2d::Node*, DisplayType>>& getDisplayList() const;
	virtual void setDisplayList(const std::vector<std::pair<cocos2d::Node*, DisplayType>> &displayList, bool disposeExisting = true);

	virtual void setBoneVisible(bool vislble) override;
	virtual void updateDisplayColor(int aOffset, int rOffset, int gOffset, int bOffset, float aMultiplier, float rMultiplier, float gMultiplier, float bMultiplier, bool colorChanged = false);

protected:
	virtual void setArmature(Armature *armature) override;

	virtual void update();
	virtual void changeDisplay(int displayIndex);
	virtual void updateSlotDisplay(bool disposeExisting);
	virtual void updateChildArmatureAnimation();
	virtual void playChildArmatureAnimation();
	virtual void stopChildArmatureAnimation();

	virtual int getDisplayZIndex() const = 0;
	virtual void addDisplayToContainer(cocos2d::Node *container, int zIndex) = 0;
	virtual void removeDisplayFromContainer() = 0;
	virtual void disposeDisplay() = 0;
	virtual void disposeDisplayList() = 0;
	virtual void updateDisplay(cocos2d::Node *display) = 0;
	virtual void updateDisplayBlendMode(BlendMode blendMode) = 0;
	virtual void updateDisplayVisible(bool visible) = 0;
	virtual void updateDisplayTransform() = 0;
	virtual void arriveAtFrame(Frame *frame, const SlotTimelineState *timelineState, AnimationState *animationState, bool isCross);

protected:
    bool _isShowDisplay;
    bool _isColorChanged;
	int _displayIndex;
    float _originZOrder;
    float _tweenZOrder;
    float _offsetZOrder;
    BlendMode _blendMode;
    ColorTransform _colorTransform;
    std::vector<std::pair<cocos2d::Node*, DisplayType>> _displayList;
    SlotData *_slotData;
    cocos2d::Node *_display;
    Armature *_childArmature;
    bool _needUpdate;

private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(Slot);
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_CORE_SLOT_H
