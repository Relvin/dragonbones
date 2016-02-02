#ifndef DRAGONBONES_OBJECTS_FRAME_H
#define DRAGONBONES_OBJECTS_FRAME_H

#include "DragonBones.h"
#include "objects/CurveData.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class Frame
{
public:
    Frame() :
		position(0)
		,duration(0)
		,action("")
		,event("")
		,sound("")
		,frameType(FrameType::FT_FRAME)
		,curve(nullptr)
    {}
    Frame(const Frame &copyData)
    {
        operator=(copyData);
    }
    Frame& operator=(const Frame &copyData)
    {
        dispose();
        position = copyData.position;
        duration = copyData.duration;
        action = copyData.action;
        event = copyData.event;
        sound = copyData.sound;

		if (copyData.curve)
		{
			curve = new CurveData();
			*curve = *(copyData.curve);
		}

        return *this;
    }
    virtual ~Frame()
    {
        dispose();
    }
    virtual void dispose() 
    {
		if (curve)
		{
			delete curve;
			curve = nullptr;
		}
    }

public:
	enum class FrameType {FT_FRAME, FT_TRANSFORM_FRAME};

	int position;
	int duration;

	FrameType frameType;
	std::string action;
	std::string event;
	std::string sound;

	CurveData *curve;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_OBJECTS_FRAME_H
