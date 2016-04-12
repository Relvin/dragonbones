//
//  DBAutoPolygon.hpp
//  dragonbones
//
//  Created by Relvin on 16/4/5.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef DBAutoPolygon_hpp
#define DBAutoPolygon_hpp

#include "DragonBones.h"
#include "renderer/CCTrianglesCommand.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class MeshData;

class DBAutoPolygon
{
public:
    DBAutoPolygon();
    
    static DBAutoPolygon* getInstance();
    cocos2d::TrianglesCommand::Triangles triangulate(const MeshData* meshData,const cocos2d::Vec2& offset,cocos2d::Size textureSize);
    
};


NAME_SPACE_DRAGON_BONES_END
#endif /* DBAutoPolygon_hpp */
