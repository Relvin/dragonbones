//
//  IKData.hpp
//  dragonbones
//
//  Created by Relvin on 16/4/13.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef IKData_hpp
#define IKData_hpp
#include "DragonBones.h"
NAME_SPACE_DRAGON_BONES_BEGIN
class IKData
{
public:
    IKData();
    virtual ~IKData();
    void dispose()
    {
    }
    
    std::string name;
    std::string target;
    std::string bone;
    float chain;
    float weight;
    bool bendPositive;
    
private:
    
    
};

NAME_SPACE_DRAGON_BONES_END
#endif /* IKData_hpp */
