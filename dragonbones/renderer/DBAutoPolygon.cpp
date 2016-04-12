//
//  DBAutoPolygon.cpp
//  dragonbones
//
//  Created by Relvin on 16/4/5.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "DBAutoPolygon.h"
#include "objects/MeshData.h"
#include "poly2tri/poly2tri.h"
#include "base/ccTypes.h"
#include "math/Vec3.h"

NAME_SPACE_DRAGON_BONES_BEGIN

static DBAutoPolygon* _sInstance = nullptr;

DBAutoPolygon* DBAutoPolygon::getInstance()
{
    if (!_sInstance)
    {
        _sInstance = new DBAutoPolygon();
    }
    return _sInstance;
}

DBAutoPolygon::DBAutoPolygon()
{
    
}

cocos2d::TrianglesCommand::Triangles DBAutoPolygon::triangulate(const MeshData* meshData,const cocos2d::Vec2& offset,cocos2d::Size textureSize)
{
    // if there are less than 3 points, then we can't triangulate
    const std::vector<Point> points = meshData->getVectices();
    const std::vector<int> triangleVerts = meshData->getTriangles();
    int triangleCnt = meshData->getTriangleCount();
    
    cocos2d::V3F_C4B_T2F* verts= new cocos2d::V3F_C4B_T2F[points.size()];
    unsigned short* indices = new unsigned short[triangleCnt * 3];
    unsigned short idx = 0;
    unsigned short vdx = 0;
    
    float width = meshData->getWidht();
    float height = meshData->getHeight();
    
    for(;idx < triangleVerts.size();idx++)
    {
        int vectexIdx = triangleVerts.at(idx);
        if (vectexIdx >= points.size())
        {
            CCLOG("Error triangles!!!");
            continue;
        }
        Point vectex = points.at(vectexIdx);
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
            int index = meshData->getIndexInVectices(vectex.x,vectex.y);
            const Point& point = meshData->getUVByIndex(index);
            //vert does not exist yet, so we need to create a new one,
            auto c4b = cocos2d::Color4B::WHITE;
            auto t2f = cocos2d::Tex2F((offset.x + point.x * width) / textureSize.width,(offset.y + point.y * height)/ textureSize.height); // don't worry about tex coords now, we calculate that later
            cocos2d::V3F_C4B_T2F vert = {v3,c4b,t2f};
            verts[vdx] = vert;
            indices[idx] = vdx;
            vdx++;
        }

       
    }

    cocos2d::TrianglesCommand::Triangles triangles = {verts, indices, vdx, idx};
    return triangles;
}


NAME_SPACE_DRAGON_BONES_END