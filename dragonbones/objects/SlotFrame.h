#ifndef DRAGONBONES_OBJECTS_SLOT_FRAME_H
#define DRAGONBONES_OBJECTS_SLOT_FRAME_H

#include "Frame.h"
#include "geoms/ColorTransform.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class SlotFrame : public Frame
{
public:
	SlotFrame() :
		tweenEasing(10)
		,displayIndex(0)
		,visible(true)
		,zOrder(0.f)
		,color(nullptr)
	{}

	virtual ~SlotFrame()
	{
		dispose();
	}

	virtual void dispose()
	{
		Frame::dispose();
		_dispose();
	}

private:
	void _dispose()
	{
		if (color)
		{
			delete color;
			color = nullptr;
		}
	}

public:
	float tweenEasing;
	int displayIndex;
	bool visible;
	float zOrder;
	ColorTransform *color;
};
NAME_SPACE_DRAGON_BONES_END
#endif DRAGONBONES_OBJECTS_SLOT_FRAME_H
