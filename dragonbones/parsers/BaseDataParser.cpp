#include "BaseDataParser.h"

#include <stdio.h>
#include <cstring>

// geoms
#include "geoms/ColorTransform.h"
#include "geoms/Matrix.h"
#include "geoms/Point.h"
#include "geoms/Rectangle.h"
#include "geoms/Transform.h"
// objects
#include "objects/Frame.h"
#include "objects/TransformFrame.h"
#include "objects/Timeline.h"
#include "objects/TransformTimeline.h"
#include "objects/AnimationData.h"
#include "objects/DisplayData.h"
#include "objects/SlotData.h"
#include "objects/SkinData.h"
#include "objects/BoneData.h"
#include "objects/ArmatureData.h"
#include "objects/DragonBonesData.h"
#include "objects/SlotFrame.h"
// textures
#include "textures/TextureData.h"
#include "textures/TextureAtlasData.h"

NAME_SPACE_DRAGON_BONES_BEGIN

BaseDataParser::BaseDataParser() {}
BaseDataParser::~BaseDataParser() {}

void BaseDataParser::transformArmatureData(ArmatureData *armatureData)
{
    for (size_t i = armatureData->boneDataList.size(); i--;)
    {
        BoneData *boneData = armatureData->boneDataList[i];
        
        if (boneData && !boneData->parent.empty())
        {
            const BoneData *parentBoneData = armatureData->getBoneData(boneData->parent);
            
            if (parentBoneData)
            {
                boneData->transform = boneData->global;
                boneData->transform.transformWith(parentBoneData->global);
            }
        }
    }
}

void BaseDataParser::transformArmatureDataAnimations(ArmatureData *armatureData)
{
    for (size_t i = 0, l = armatureData->animationDataList.size(); i < l; ++i)
    {
        transformAnimationData(armatureData->animationDataList[i], armatureData, false);
    }
}

void BaseDataParser::transformAnimationData(AnimationData *animationData, const ArmatureData *armatureData, bool isGlobalData)
{
	if (!isGlobalData)
	{
		return;
	}

    SkinData *skinData = armatureData->getSkinData("");
	std::vector<SlotData*> slotDataList;
	if (skinData)
	{
		slotDataList = armatureData->slotDataList;
	}

	auto boneDataList = armatureData->boneDataList;
    for (size_t i = 0, l = boneDataList.size(); i < l; ++i)
    {
        const BoneData *boneData = boneDataList[i];
        TransformTimeline *timeline = animationData->getTimeline(boneData->name);
        SlotTimeline *slotTimeline = animationData->getSlotTimeline(boneData->name);
        if (!timeline && !slotTimeline)
        {
            continue;
        }
        
        SlotData *slotData = nullptr;        
        if (skinData)
        {
            for (size_t i = 0, l = skinData->slotDataList.size(); i < l; ++i)
            {
                slotData = skinData->slotDataList[i];
                if (slotData->parent == boneData->name)
                {
                    break;
                }
            }
        }

		std::vector<Frame*> slotFrameList;
		if (slotTimeline)
		{
			slotFrameList = slotTimeline->frameList;
		}

        Transform *originTransform = nullptr;
        Point *originPivot = nullptr;
        TransformFrame *prevFrame = nullptr;

		auto frameList = timeline->frameList;
		size_t frameListLength = frameList.size();
        for (size_t i = 0, l = frameListLength; i < l; ++i)
        {
            TransformFrame *frame = static_cast<TransformFrame*>(frameList[i]);
            // 计算frame的transform信息
			setFrameTransform(animationData, armatureData, boneData, frame);

			// 转换成相对骨架的transform信息
            frame->transform.x -= boneData->transform.x;
            frame->transform.y -= boneData->transform.y;
            frame->transform.skewX -= boneData->transform.skewX;
            frame->transform.skewY -= boneData->transform.skewY;
            frame->transform.scaleX /= boneData->transform.scaleX;
            frame->transform.scaleY /= boneData->transform.scaleY;
            
            
            if (prevFrame)
            {
                const float dLX = frame->transform.skewX - prevFrame->transform.skewX;
                if (prevFrame->tweenRotate)
                {
                    if (prevFrame->tweenRotate > 0)
                    {
                        if (dLX < 0)
                        {
                            frame->transform.skewX += PI * 2;
                            frame->transform.skewY += PI * 2;
                        }
                        
                        if (prevFrame->tweenRotate > 1)
                        {
                            frame->transform.skewX += PI * 2 * (prevFrame->tweenRotate - 1);
                            frame->transform.skewY += PI * 2 * (prevFrame->tweenRotate - 1);
                        }
                    }
                    else
                    {
                        if (dLX > 0)
                        {
                            frame->transform.skewX -= PI * 2;
                            frame->transform.skewY -= PI * 2;
                        }
                        
                        if (prevFrame->tweenRotate < 1)
                        {
                            frame->transform.skewX += PI * 2 * (prevFrame->tweenRotate + 1);
                            frame->transform.skewY += PI * 2 * (prevFrame->tweenRotate + 1);
                        }
                    }
                }
                else
                {
                    frame->transform.skewX = prevFrame->transform.skewX + formatRadian(frame->transform.skewX - prevFrame->transform.skewX);
                    frame->transform.skewY = prevFrame->transform.skewY + formatRadian(frame->transform.skewY - prevFrame->transform.skewY);
                }
            }
            prevFrame = frame;
        }

		if (slotTimeline && slotFrameList.size() > 0)
		{
			frameListLength = slotFrameList.size();
			for (size_t j = 0; j < frameListLength; j++)
			{
				auto slotFrame = static_cast<SlotFrame*>(slotFrameList[j]);
				if (!slotTimeline->transformed)
				{
					if (slotData)
					{
						slotFrame->zOrder -= slotData->zOrder;
					}
				}
			}
			slotTimeline->transformed = true;
		}
        timeline->transformed = true;
    }
}

void BaseDataParser::addHideTimeline(AnimationData *animationData, const ArmatureData *armatureData)
{
    for (size_t i = 0, l = armatureData->boneDataList.size(); i < l; ++i)
    {
        const BoneData *boneData = armatureData->boneDataList[i];
        
        if (!animationData->getTimeline(boneData->name))
        {
            auto iterator = std::find(animationData->hideTimelineList.cbegin(), animationData->hideTimelineList.cend(), boneData->name);
            
            if (iterator != animationData->hideTimelineList.cend())
            {
                continue;
            }
            
            animationData->hideTimelineList.push_back(boneData->name);
        }
    }
}

void BaseDataParser::setFrameTransform(AnimationData *animationData, const ArmatureData *armatureData, const BoneData *boneData, TransformFrame *frame)
{
    frame->transform = frame->global;
    BoneData *parentData = armatureData->getBoneData(boneData->parent);
    if (parentData)
    {
        TransformTimeline *parentTimeline = animationData->getTimeline(parentData->name);
        if (parentTimeline)
        {
            std::vector<TransformTimeline*> parentTimelineList;
            std::vector<BoneData*> parentDataList;
            while (parentTimeline)
            {
                parentTimelineList.push_back(parentTimeline);
                parentDataList.push_back(parentData);
                parentData = armatureData->getBoneData(parentData->parent);
                
                if (parentData)
                {
                    parentTimeline = animationData->getTimeline(parentData->name);
                }
                else
                {
                    parentTimeline = nullptr;
                }
            }
            
            Transform *globalTransform = nullptr;
			Matrix globalTransformMatrix;
			Transform currentTransform;
			Matrix currentTransformMatrix;
            
            for (size_t i = parentTimelineList.size(); i--;)
            {
                parentTimeline = parentTimelineList[i];
                parentData = parentDataList[i];
				// 一级一级找到当前帧对应的每个父节点的transform(相对transform) 保存到currentTransform，globalTransform保存根节点的transform
                getTimelineTransform(parentTimeline, frame->position, &currentTransform, !globalTransform);
                
                if (globalTransform)
                {
					currentTransform.x += parentTimeline->originTransform.x + parentData->transform.x;
					currentTransform.y += parentTimeline->originTransform.y + parentData->transform.y;

                    currentTransform.skewX += parentTimeline->originTransform.skewX + parentData->transform.skewX;
					currentTransform.skewY += parentTimeline->originTransform.skewY + parentData->transform.skewY;

                    currentTransform.scaleX *= parentTimeline->originTransform.scaleX * parentData->transform.scaleX;
					currentTransform.scaleY *= parentTimeline->originTransform.scaleY * parentData->transform.scaleY;

					currentTransform.toMatrix(currentTransformMatrix, true);
					Transform::matrixToTransform(currentTransformMatrix, *globalTransform, 
						currentTransform.scaleX * globalTransform->scaleX >= 0, currentTransform.scaleY * globalTransform->scaleY >= 0);
                }
                else
                {
                    globalTransform = new Transform();
                    *globalTransform = currentTransform;
                }
                
                globalTransform->toMatrix(globalTransformMatrix, true);
            }
            
            frame->transform.transformWith(*globalTransform);
            
            if (globalTransform)
            {
                delete globalTransform;
                globalTransform = nullptr;
            }
        }
    }
}

void BaseDataParser::getTimelineTransform(const TransformTimeline *timeline, int position, Transform *retult, bool isGlobal)
{
    for (size_t i = 0, l = timeline->frameList.size(); i < l; ++i)
    {
        const TransformFrame *currentFrame = static_cast<const TransformFrame*>(timeline->frameList[i]);
        
        if (currentFrame->position <= position && currentFrame->position + currentFrame->duration > position)
        {
            if (i == timeline->frameList.size() - 1 || position == currentFrame->position)
            {
                //copy
                *retult = isGlobal ? currentFrame->global : currentFrame->transform;
            }
            else
            {
                float progress = (position - currentFrame->position) / (float)(currentFrame->duration);
                const float tweenEasing = currentFrame->tweenEasing;
                
                if (tweenEasing && tweenEasing != NO_TWEEN_EASING && tweenEasing != AUTO_TWEEN_EASING)
                {
                    progress = getEaseValue(progress, tweenEasing);
                }
                
                const TransformFrame *nextFrame = static_cast<const TransformFrame*>(timeline->frameList[i + 1]);
                const Transform *currentTransform = isGlobal ? &currentFrame->global : &currentFrame->transform;
                const Transform *nextTransform = isGlobal ? &nextFrame->global : &nextFrame->transform;
                retult->x = currentTransform->x + (nextTransform->x - currentTransform->x) * progress;
                retult->y = currentTransform->y + (nextTransform->y - currentTransform->y) * progress;
                retult->skewX = formatRadian(currentTransform->skewX + (nextTransform->skewX - currentTransform->skewX) * progress);
                retult->skewY = formatRadian(currentTransform->skewY + (nextTransform->skewY - currentTransform->skewY) * progress);
                retult->scaleX = currentTransform->scaleX + (nextTransform->scaleX - currentTransform->scaleX) * progress;
                retult->scaleY = currentTransform->scaleY + (nextTransform->scaleY - currentTransform->scaleY) * progress;
            }
            
            break;
        }
    }
}

NAME_SPACE_DRAGON_BONES_END
