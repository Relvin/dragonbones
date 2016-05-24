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
#include "objects/MeshData.h"

USING_NS_CC;

NAME_SPACE_DRAGON_BONES_BEGIN

DBSkin* DBSkin::create(DisplayData *displayData,const std::string & textureAtlasName)
{
    DBSkin* skin = new (std::nothrow) DBSkin();
    if (skin && displayData && skin->initWithDisplayDataTextureName(displayData, textureAtlasName))
    {
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
#if 0
    Sprite::initWithFile("res/Ubbie/leg_r.png");
#else
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
        
        float scale = cocos2d::Director::getInstance()->getContentScaleFactor();
        rect = cocos2d::Rect(rect.origin * scale,rect.size * scale);
        cocos2d::Size originSize(rect.size);
        
        if (textureData->frame)
        {
            float frameX = -textureData->frame->x;
            float frameY = -textureData->frame->y;
            originSize.width = textureData->frame->width ;
            originSize.height = textureData->frame->height;
            // offset = trimed center - origin center
            // y use cocos2d coordinates
            offset.x = (rect.size.width - originSize.width) * 0.5 + frameX;
            offset.y = (originSize.height - rect.size.height)*0.5 - frameY;
        }
        
        cocos2d::SpriteFrame* spriteFrame = cocos2d::SpriteFrame::createWithTexture(texture, rect,
                                                                                    textureData->rotated, offset, originSize);
        if (!Sprite::initWithSpriteFrame(spriteFrame))
        {
            return false;
        }
    }
#endif
    // sprite
    this->updateBaseInfo();
    
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
    this->buildPolygonInfo();
}

void DBSkin::buildPolygonInfo()
{
    _polyVtx.clear();
    if (this->_displayData->type == DisplayType::DT_MESH)
    {
        MeshData *meshData = dynamic_cast<MeshData *>(this->_displayData);
        do
        {
            CC_BREAK_IF(meshData == nullptr);
    
            Point offset;
            SpriteFrame* spriteFrame = this->getSpriteFrame();
            Size textureSize = this->getTexture()->getContentSize();
            if (spriteFrame)
            {
                offset = spriteFrame->getRect().origin;
            }
            
            const std::vector<MeshData::VECTEX_UV>& vectex_uvs = meshData->_vectices;
            const std::vector<int> triangleVerts = meshData->getTriangles();
            int triangleCnt = meshData->getTriangleCount();
            
            cocos2d::V3F_C4B_T2F* verts= new cocos2d::V3F_C4B_T2F[vectex_uvs.size()];
            unsigned short* indices = new unsigned short[triangleCnt * 3];
            unsigned short idx = 0;
            unsigned short vdx = 0;
            
            float width = meshData->getWidth();
            float height = meshData->getHeight();
            
            for(;idx < triangleVerts.size();idx++)
            {
                int vectexIdx = triangleVerts.at(idx);
                if (vectexIdx >= vectex_uvs.size())
                {
                    CCLOG("Error triangles!!!");
                    continue;
                }
                MeshData::VECTEX_UV vectex_uv = vectex_uvs.at(vectexIdx);
                Point vectex = vectex_uv.vectex;
                Point UV = vectex_uv.UV;
                //dragonbones vectex convert cocos2dx vectex
                auto v3 = cocos2d::Vec3(vectex.x + width / 2, height / 2 - vectex.y, 0);
                bool found = false;
                size_t j;
                size_t length = vdx;
                for(j = 0; j < length; j++)
                {
                    if(verts[j].vertices == v3)
                    {
                        found = true;
                        break;
                    }
                }
                if(found)
                {
                    //if we found the same vertex, don't add to verts, but use the same vertex with indices
                    indices[idx] = j;
                }
                else
                {
                    //vert does not exist yet, so we need to create a new one,
                    auto c4b = cocos2d::Color4B::WHITE;
                    auto t2f = cocos2d::Tex2F((offset.x + UV.x * width) / textureSize.width,(offset.y + UV.y * height)/ textureSize.height); // don't worry about tex coords now, we calculate that later
                    cocos2d::V3F_C4B_T2F vert = {v3,c4b,t2f};
                    verts[vdx] = vert;
                    indices[idx] = vdx;
                    vdx++;
                    
                    _polyVtx.push_back(vectexIdx);
                }
                
                
            }
            cocos2d::PolygonInfo polygonInfo;
            polygonInfo.triangles = {verts, indices, vdx, idx};
            
            polygonInfo.rect = cocos2d::Rect(0,0,meshData->getWidth() ,meshData->getHeight());
            this->setPolygonInfo(polygonInfo);
            
        }while(0);
    }
}

void DBSkin::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
    Sprite::draw(renderer, transform, flags);
    this->updatePolygonVectex();
    
}

void DBSkin::updatePolygonVectex()
{
    if (this->_displayData->type == DisplayType::DT_MESH && _polyVtx.size() > 0)
    {
        MeshData *meshData = dynamic_cast<MeshData *>(this->_displayData);
        do
        {
            CC_BREAK_IF(meshData == nullptr);
            if (!meshData->updated)
            {
                break;
            }
            
            float width = meshData->_width;
            float height = meshData->_height;
            V3F_C4B_T2F* verts = _polyInfo.triangles.verts;
            std::vector<MeshData::VECTEX_UV> &vectex_uvs = meshData->_vectices;
            for(ssize_t i=0; i< _polyInfo.getVertCount(); ++i)
            {
                if(i >= _polyVtx.size())
                {
                    break;
                }
                int index = _polyVtx.at(i);
                if (index >= vectex_uvs.size())
                {
                    continue;
                }
                V3F_C4B_T2F& vert = verts[i];
                const cocos2d::Vec2& vectex = meshData->getVectexByIndex(index);
                
                verts[i].vertices.x = vectex.x + width / 2;
                verts[i].vertices.y = height / 2 - vectex.y;
            }
        }while (0);
    }
}

NAME_SPACE_DRAGON_BONES_END
