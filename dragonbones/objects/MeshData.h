//
//  MeshData.hpp
//  dragonbones
//
//  Created by Relvin on 16/4/5.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef MeshData_hpp
#define MeshData_hpp

#include "DragonBones.h"
#include "objects/DisplayData.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class MeshData
: public DisplayData
{
public:
    struct VECTEX_UV{
        Point vectex;
        Point UV;
    };
    
    MeshData();
    void updateVertices(int offset,const std::vector<float >& vectices);
    void resetVisitData();
    virtual ~MeshData();
    void addVectex(Point vectex);
    void addUV(Point UV);
    void addTriangle(int triangle);
    
    void setWidth(float width);
    void setHeight(float height);
    
    inline float getWidth() const {return this->_width;};
    inline float getHeight() const {return this->_height;};
    
    int getIndexInVectices(const Point& point) const;
    int getIndexInVectices(float x, float y) const;
    
    Point getUVByIndex(int index) const;
    std::vector<Point>& getVectices() const;
    Point getVectexByIndex(int index) const;
    std::vector<int>& getTriangles() const;
    std::vector<Point>& getUVs() const;
    inline int getTriangleCount() const {return this->_triangles.size() / 3;};
    
    void resetVertices();

    mutable std::vector<VECTEX_UV> _vectices;
    mutable std::vector<int> _triangles;
    mutable std::vector<Point> _UVs;
    mutable std::vector<Point> _orgVectices;
    
    float _width;
    float _height;
    bool updated;
};


NAME_SPACE_DRAGON_BONES_END

#endif /* MeshData_hpp */
