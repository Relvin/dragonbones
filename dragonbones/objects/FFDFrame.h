//
//  FFDFrame.hpp
//  dragonbones
//
//  Created by Relvin on 16/4/6.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef FFDFrame_hpp
#define FFDFrame_hpp

#include "Frame.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class FFDFrame
: public Frame
{
public:
    FFDFrame();
    virtual ~FFDFrame();
    inline void setTweenEasing(float tweenEasing) {this->tweenEasing = tweenEasing ;};
    inline float getTweenEasing() const {return this->tweenEasing;};
    inline void setOffset(float offset) {this->offset = offset ;};
    inline float getOffset() const {return this->offset;};
    std::vector<float >& getVertices() const;

    float tweenEasing;
    float offset;
    mutable std::vector<float> vertices;
};

NAME_SPACE_DRAGON_BONES_END
#endif /* FFDFrame_hpp */
