//
//  DBSkin.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/25.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBSkin.h"
#include "DBCCFactory.h"



NAME_SPACE_DRAGON_BONES_BEGIN


DBSkin* DBSkin::create(const std::string &name,const std::string & textureAtlasName)
{
    DBSkin* skin = new (std::nothrow) DBSkin();
    if (skin && skin->initWithTextureName(name,textureAtlasName))
    {
        skin->autorelease();
        return skin;
    }
    CC_SAFE_DELETE(skin);
    return nullptr;
}

bool DBSkin::initWithTextureName(const std::string &name,const std::string & textureAtlasName)
{
    
    ITextureAtlas *textureAtlas = nullptr;
    TextureData *textureData = nullptr;
    
    if (!textureAtlasName.empty())
    {
        textureAtlas = DBCCFactory::getInstance()->getTextureAtlas(textureAtlasName);
        if (textureAtlas)
        {
            textureData = textureAtlas->textureAtlasData->getTextureData(name);
        }
    }
    
    if (!textureData)
    {
        return false;
    }
    
    
    if (!textureAtlas || !textureData || !this->initWithTextureData(textureAtlas, textureData))
    {
        return false;
    }
    
    return true;
}

bool DBSkin::initWithTextureData(const ITextureAtlas *textureAtlas, const TextureData *textureData)
{
    
    DBCCTextureAtlas *dbccTextureAtlas = (DBCCTextureAtlas*)(textureAtlas);
    
    if (!dbccTextureAtlas || !textureData)
    {
        return false;
    }
    
    auto texture = dbccTextureAtlas->getTexture();
    assert(texture);
    
    const float x = textureData->region.x;
    const float y = textureData->region.y;
    const bool rotated = textureData->rotated;
    const float width = rotated ? textureData->region.height : textureData->region.width;
    const float height = rotated ? textureData->region.width : textureData->region.height;
    cocos2d::Rect rect(x, y, width, height);
    cocos2d::Vec2 offset;
    cocos2d::Size originSize(width, height);
    
    if (textureData->frame)
    {
        float scale = cocos2d::Director::getInstance()->getContentScaleFactor();
        
        float frameX = -textureData->frame->x;
        float frameY = -textureData->frame->y;
        originSize.width = textureData->frame->width ;
        originSize.height = textureData->frame->height;
        // offset = trimed center - origin center
        // y use cocos2d coordinates
        offset.x = (width - originSize.width) * 0.5 + frameX;
        offset.y = (originSize.height - height)*0.5 - frameY;
        
        
        originSize = originSize * scale;
        offset = offset * scale;
        rect = cocos2d::Rect(rect.origin * scale,rect.size * scale);
        auto spriteFrame = cocos2d::SpriteFrame::createWithTexture(texture, rect,
                                                                   textureData->rotated, offset, originSize);
        if (!Sprite::initWithSpriteFrame(spriteFrame))
        {
            return false;
        }
    }
    else
    {
        if (!Sprite::initWithTexture(texture, rect, rotated))
        {
            return false;
        }
    }
    // sprite
    
    this->setCascadeColorEnabled(true);
    this->setCascadeOpacityEnabled(true);
    
    this->setContentSize(originSize);
    return true;
}

DBSkin::DBSkin()
{
    
}

DBSkin::~DBSkin()
{
    
}



NAME_SPACE_DRAGON_BONES_END
