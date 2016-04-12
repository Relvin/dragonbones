//
//  FFDFrame.cpp
//  dragonbones
//
//  Created by Relvin on 16/4/6.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#include "FFDFrame.h"
NAME_SPACE_DRAGON_BONES_BEGIN

FFDFrame::FFDFrame()
: tweenEasing(0.f)
, offset(0.f)
{
    vertices.clear();
}

FFDFrame::~FFDFrame()
{
    vertices.clear();
}

std::vector<float>& FFDFrame::getVertices() const
{
    return this->vertices;
}


NAME_SPACE_DRAGON_BONES_END