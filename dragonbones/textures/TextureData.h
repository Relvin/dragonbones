#ifndef DRAGONBONES_TEXTURES_TEXTURE_DATA_H
#define DRAGONBONES_TEXTURES_TEXTURE_DATA_H

#include "DragonBones.h"
#include "geoms/Rectangle.h"
#include "2d/CCSpriteFrame.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class TextureData
{    
public:
    TextureData() :
		rotated(false)
		,name("")
		,frame(nullptr)
    ,spriteFrame(nullptr)
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
        if (spriteFrame)
        {
            CC_SAFE_RELEASE_NULL(spriteFrame);
        }
    }
    cocos2d::SpriteFrame* getSpriteFrame() const
    {
        return this->spriteFrame;
    }
    void setSpriteFrame(cocos2d::SpriteFrame* srcSpriteFrame)
    {
        if (this->spriteFrame)
        {
            CC_SAFE_RELEASE_NULL(this->spriteFrame);
        }
        this->spriteFrame = srcSpriteFrame;
        CC_SAFE_RETAIN(this->spriteFrame);
    }

public:
	bool rotated;
	std::string name;
	Rectangle region;
	Rectangle *frame;

    
private:
    cocos2d::SpriteFrame *spriteFrame;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_TEXTURES_TEXTURE_DATA_H
