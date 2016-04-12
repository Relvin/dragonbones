//
//  FFDTimeline.hpp
//  dragonbones
//
//  Created by Relvin on 16/4/6.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef FFDTimeline_hpp
#define FFDTimeline_hpp

#include "Timeline.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class FFDTimeline
: public Timeline
{
public:
    FFDTimeline();
    virtual ~FFDTimeline();
    
    
    
    std::string name;
    std::string skinName;
    std::string slotName;
    float offset;
    
};


NAME_SPACE_DRAGON_BONES_END


#endif /* FFDTimeline_hpp */
