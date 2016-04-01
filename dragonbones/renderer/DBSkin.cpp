//
//  DBSkin.cpp
//  dragonbones
//
//  Created by Relvin on 16/2/25.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBSkin.h"
#include "DBCCFactory.h"
#include "objects/DisplayData.h"


NAME_SPACE_DRAGON_BONES_BEGIN


DBSkin* DBSkin::create(DisplayData *displayData,const std::string & textureAtlasName)
{
    DBSkin* skin = new (std::nothrow) DBSkin();
    if (skin && displayData && skin->initWithDisplayDataTextureName(displayData, textureAtlasName))
    {
        skin->updateBaseInfo();
        skin->autorelease();
        return skin;
    }
    CC_SAFE_DELETE(skin);
    return nullptr;
}

bool DBSkin::initWithDisplayDataTextureName(DisplayData *displayData,const std::string & textureAtlasName)
{
    this->_displayData = displayData;
    if (!this->initWithTextureName(displayData->name,textureAtlasName))
    {
        return false;
    }
    return true;
}

bool DBSkin::initWithTextureName(const std::string &name,const std::string & textureAtlasName)
{
    
    ITextureAtlas *textureAtlas = nullptr;
    TextureData *textureData = nullptr;
    this->_textureAtlasName = textureAtlasName;
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
    if (textureData->getSpriteFrame())
    {
        if (!Sprite::initWithSpriteFrame(textureData->getSpriteFrame()))
        {
            return false;
        }
    }
    else
    {
        const float x = textureData->region.x;
        const float y = textureData->region.y;
        const bool rotated = textureData->rotated;
        const float width = rotated ? textureData->region.height : textureData->region.width;
        const float height = rotated ? textureData->region.width : textureData->region.height;
        cocos2d::Rect rect(x, y, width, height);
        cocos2d::Vec2 offset;
        
        
        if (textureData->frame)
        {
            float scale = cocos2d::Director::getInstance()->getContentScaleFactor();
            rect = cocos2d::Rect(rect.origin * scale,rect.size * scale);
            cocos2d::Size originSize(rect.size);
            
            float frameX = -textureData->frame->x;
            float frameY = -textureData->frame->y;
            originSize.width = textureData->frame->width ;
            originSize.height = textureData->frame->height;
            // offset = trimed center - origin center
            // y use cocos2d coordinates
            offset.x = (rect.size.width - originSize.width) * 0.5 + frameX;
            offset.y = (originSize.height - rect.size.height)*0.5 - frameY;
            
            
            cocos2d::SpriteFrame* spriteFrame = cocos2d::SpriteFrame::createWithTexture(texture, rect,
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
    }
    
    // sprite
    
    this->setCascadeColorEnabled(true);
    this->setCascadeOpacityEnabled(true);
    
    return true;
}

DBSkin::DBSkin()
{
    
}

DBSkin::~DBSkin()
{
    
}

void DBSkin::setDisplayData(dragonBones::DisplayData *displayData)
{
    if (displayData)
    {
        if (!this->_displayData || this->_displayData->name != displayData->name)
        {
            this->_displayData = displayData;
            this->updateBaseInfo();
            this->updateTextureData();
        }
    }
    
}

void DBSkin::updateTextureData()
{
    this->initWithTextureName(this->_displayData->name,this->_textureAtlasName);
}

void DBSkin::updateBaseInfo()
{
    float pivotX = 0.f;
    float pivotY = 0.f;
    DisplayData* displayData = this->_displayData;
    if (displayData)
    {
        pivotX = displayData->pivot.x;
        pivotY = displayData->pivot.y;
    }
    
    if ((pivotX > 0.000001f && pivotX >= -0.000001f) || (pivotY <= 0.000001f && pivotY >= -0.000001f))
    {
        this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    }
    else
    {
        cocos2d::Size size = this->getContentSize();
        this->setAnchorPoint(cocos2d::Vec2(pivotX / size.width, 1.f - pivotY / size.height));
    }
}


NAME_SPACE_DRAGON_BONES_END
