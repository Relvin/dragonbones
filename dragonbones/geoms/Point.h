#ifndef DRAGONBONES_GEOMS_POINT_H
#define DRAGONBONES_GEOMS_POINT_H

#include "DragonBones.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class Point
{
public:
    Point() :
		x(0.f)
		,y(0.f)
    {}
	Point(float x, float y)
	{
		x = x;
		y = y;
	}
    Point(const Point &copyData)
    {
        operator=(copyData);
    }
    void operator=(const Point &copyData)
    {
        x = copyData.x;
        y = copyData.y;
    }
    virtual ~Point() {}

public:
	float x;
	float y;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_GEOMS_POINT_H
