#ifndef DRAGONBONES_OBJECTS_TRANSFORM_FRAME_H
#define DRAGONBONES_OBJECTS_TRANSFORM_FRAME_H

#include "DragonBones.h"
#include "Frame.h"
#include "geoms/Point.h"
#include "geoms/Transform.h"
#include "geoms/ColorTransform.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class TransformFrame : public Frame
{    
public:
    TransformFrame() :
		visible(true)
		,tweenScale(false)
		,tweenRotate(0)
		,tweenEasing(NO_TWEEN_EASING)
    {
		frameType = FrameType::FT_TRANSFORM_FRAME;
	}

    TransformFrame(const TransformFrame &copyData)
    {
        operator=(copyData);
    }
    TransformFrame& operator=(const TransformFrame &copyData)
    {
        Frame::operator=(copyData);
        _dispose();
        visible = copyData.visible;
        tweenScale = copyData.tweenScale;
        tweenRotate = copyData.tweenRotate;
        tweenEasing = copyData.tweenEasing;
        global = copyData.global;
        transform = copyData.transform;
        pivot = copyData.pivot;
        scaleOffset = copyData.scaleOffset;

        return *this;
    }
    virtual ~TransformFrame()
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
    }

public:
	bool visible;
	bool tweenScale;
	int tweenRotate;
	// NaN:no tween, 10:auto tween, [-1, 0):ease in, 0:line easing, (0, 1]:ease out, (1, 2]:ease in out
	float tweenEasing;

	Transform global;
	Transform transform;
	Point pivot;
	Point scaleOffset;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_OBJECTS_TRANSFORM_FRAME_H
