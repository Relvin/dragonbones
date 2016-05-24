//
//  MeshData.cpp
//  dragonbones
//
//  Created by Relvin on 16/4/5.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "MeshData.h"

NAME_SPACE_DRAGON_BONES_BEGIN

MeshData::MeshData()
: _width(0.f)
, _height(0.f)
, updated(false)
{
    _vectices.clear();
    _triangles.clear();
    _UVs.clear();
}

MeshData::~MeshData()
{
    _vectices.clear();
    _triangles.clear();
    _UVs.clear();
}

void MeshData::updateVertices(int offset, const std::vector<float> &vectices)
{
    bool skined = false;
    
//    printf("\nvectices size = %d\n",vectices.size());
    for (int i = 0;i < vectices.size();i+=2)
    {
        int idx = (i + offset) >> 1;
        if (idx < _vectices.size())
        {
            if (skined)
            {
                _vectices[idx].vectex.x += vectices[i];
                _vectices[idx].vectex.y += vectices[i + 1];
            }
            else
            {
                _vectices[idx].vectex.x = _orgVectices[idx].x + vectices[i];
                _vectices[idx].vectex.y = _orgVectices[idx].y + vectices[i + 1];
            }
        }
    }
    
    updated = true;
}

void MeshData::resetVertices()
{
    for (int i = 0;i < _vectices.size();i++)
    {
        _vectices[i].vectex.x = _orgVectices[i].x;
        _vectices[i].vectex.y = _orgVectices[i].y;
    }
    
    updated = true;
}

void MeshData::resetVisitData()
{
    this->_vectices.clear();
    for (int idx = 0;idx < this->_orgVectices.size();idx++ )
    {
        VECTEX_UV vec_uv = {this->_orgVectices.at(idx),this->_UVs.at(idx)};
        _vectices.push_back(vec_uv);
    }
}

std::vector<Point> &MeshData::getVectices() const
{
    return this->_orgVectices;
}

std::vector<Point>& MeshData::getUVs() const
{
    return this->_UVs;
}

std::vector<int>& MeshData::getTriangles() const
{
    return this->_triangles;
}

void MeshData::addVectex(dragonBones::Point vectex)
{
    this->_orgVectices.push_back(vectex);
}

void MeshData::addUV(dragonBones::Point UV)
{
    this->_UVs.push_back(UV);
}

void MeshData::addTriangle(int triangle)
{
    this->_triangles.push_back(triangle);
}

void MeshData::setWidth(float width)
{
    this->_width = width;
}

void MeshData::setHeight(float height)
{
    this->_height = height;
}

int MeshData::getIndexInVectices(const Point &point) const
{
    return this->getIndexInVectices(point.x,point.y);
}

int MeshData::getIndexInVectices(float x, float y) const
{
    for (int idx = 0;idx < _orgVectices.size();idx++)
    {
        auto point = _orgVectices.at(idx);
        if (x == point.x && y == point.y)
        {
            return idx;
        }
    }
    return 0;
}

Point MeshData::getVectexByIndex(int index) const
{
    if (index <= _orgVectices.size())
    {
//        if (updated)
        {
            return _vectices.at(index).vectex;
        }
//        else
//        {
//            return _orgVectices.at(index);
//        }
    }
    return Point(0,0);
}

Point MeshData::getUVByIndex(int index) const
{
    if (_UVs.size() > index)
    {
        return _UVs.at(index);
    }
    return Point(0,0);
}


NAME_SPACE_DRAGON_BONES_END