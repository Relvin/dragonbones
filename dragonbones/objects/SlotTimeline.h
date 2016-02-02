#ifndef DRAGONBONES_OBJECTS_SLOTTIMELINE_DATA_H
#define DRAGONBONES_OBJECTS_SLOTTIMELINE_DATA_H

#include "Timeline.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class SlotTimeline : public Timeline
{
public:
	SlotTimeline() :
		name("")
		,transformed(false)
		,offset(0)
	{}

	virtual ~SlotTimeline()
	{
		Timeline::dispose();
	}

public:
	std::string name;
	bool transformed;
	float offset;
};

NAME_SPACE_DRAGON_BONES_END
#endif DRAGONBONES_OBJECTS_SLOTTIMELINE_DATA_H
