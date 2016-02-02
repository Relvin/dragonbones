#ifndef DRAGONBONES_GEOMS_COLOR_TRANSFORM_H
#define DRAGONBONES_GEOMS_COLOR_TRANSFORM_H

#include "DragonBones.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class ColorTransform
{
public:
    ColorTransform()
    {
        alphaMultiplier = 1.f;
        redMultiplier = 1.f;
        greenMultiplier = 1.f;
        blueMultiplier = 1.f;
        alphaOffset = 0;
        redOffset = 0;
        greenOffset = 0;
        blueOffset = 0;
    }
    ColorTransform(const ColorTransform &copyData)
    {
        operator=(copyData);
    }
    void operator=(const ColorTransform &copyData)
    {
        alphaMultiplier = copyData.alphaMultiplier;
        redMultiplier = copyData.redMultiplier;
        greenMultiplier = copyData.greenMultiplier;
        blueMultiplier = copyData.blueMultiplier;
        alphaOffset = copyData.alphaOffset;
        redOffset = copyData.redOffset;
        greenOffset = copyData.greenOffset;
        blueOffset = copyData.blueOffset;
    }
    virtual ~ColorTransform() {}

public:
	float alphaMultiplier;
	float redMultiplier;
	float greenMultiplier;
	float blueMultiplier;

	int alphaOffset;
	int redOffset;
	int greenOffset;
	int blueOffset;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_GEOMS_COLOR_TRANSFORM_H
