#ifndef DRAGONBONES_RENDERER_DBCC_SLOT_H
#define DRAGONBONES_RENDERER_DBCC_SLOT_H

#include "DragonBonesHeaders.h"
#include "cocos2d.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class DBCCArmature;
class DBCCSlot : public Slot
{
public:
    virtual cocos2d::Node* getCCDisplay() const;
    virtual DBCCArmature* getCCChildArmature() const;
    virtual cocos2d::Vec2 getGlobalPosition() const;
    virtual void setDisplayImage(cocos2d::Node *display, bool disposeExisting = true);
    
public:
    DBCCSlot(SlotData *slotData);
    virtual ~DBCCSlot();
    virtual void dispose() override;
    virtual Rectangle getBoundingBox() override;
    
protected:
    virtual void updateDisplayColor(int aOffset, int rOffset, int gOffset, int bOffset, float aMultiplier, float rMultiplier, float gMultiplier, float bMultiplier, bool colorChanged = false) override;

    virtual int getDisplayZIndex() const override;
    virtual void addDisplayToContainer(cocos2d::Node *container, int zIndex) override;
    virtual void removeDisplayFromContainer() override;
    virtual void disposeDisplay() override;
    virtual void disposeDisplayList() override;
    virtual void updateDisplay(cocos2d::Node *display) override;
    virtual void updateDisplayBlendMode(BlendMode blendMode) override;
    virtual void updateDisplayVisible(bool visible) override;
    virtual void updateDisplayTransform() override;
    
protected:
	cocos2d::Node *_nodeDisplay;

private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBCCSlot);
};
NAME_SPACE_DRAGON_BONES_END

#endif  // DRAGONBONES_RENDERER_DBCC_SLOT_H
