#ifndef DRAGONBONES_OBJECTS_DISPLAY_DATA_H
#define DRAGONBONES_OBJECTS_DISPLAY_DATA_H

#include "DragonBones.h"
#include "geoms/Point.h"
#include "geoms/Transform.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class DisplayData
{
public:
    DisplayData():
        name("")
		,slotName("")
        ,type(DisplayType::DT_IMAGE)
    {}
    DisplayData(const DisplayData &copyData)
    {
        operator=(copyData);
    }
    DisplayData& operator=(const DisplayData &copyData)
    {
        dispose();

        name = copyData.name;
		slotName = copyData.slotName;
        type = copyData.type;
        transform = copyData.transform;
		pivot = copyData.pivot;

        return *this;
    }
    virtual ~DisplayData() 
    {
        dispose();
    }

    void dispose()
    {
        name.clear();
        type = DisplayType::DT_IMAGE;
    }

public:
	std::string name;
	std::string slotName;
	DisplayType type;
	Transform transform;
	Point pivot;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_OBJECTS_DISPLAY_DATA_H
