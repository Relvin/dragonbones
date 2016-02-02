#ifndef DRAGONBONES_TEXTURES_TEXTURE_DATA_H
#define DRAGONBONES_TEXTURES_TEXTURE_DATA_H

#include "DragonBones.h"
#include "geoms/Rectangle.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class TextureData
{    
public:
    TextureData() :
		rotated(false)
		,name("")
		,frame(nullptr)
    {}
    TextureData(const TextureData &copyData)
    {
        operator=(copyData);
    }
    TextureData &operator=(const TextureData &copyData)
    {
        dispose();
        rotated = copyData.rotated;
        name = copyData.name;
        region = copyData.region;
        
        if (copyData.frame)
        {
            frame = new Rectangle();
            *frame = *(copyData.frame);
        }
        
        return *this;
    }
    virtual ~TextureData()
    {
        dispose();
    }
    void dispose()
    {
        if (frame)
        {
            delete frame;
            frame = nullptr;
        }
    }

public:
	bool rotated;
	std::string name;
	Rectangle region;
	Rectangle *frame;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_TEXTURES_TEXTURE_DATA_H
