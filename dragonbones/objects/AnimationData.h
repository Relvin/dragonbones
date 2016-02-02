#ifndef DRAGONBONES_OBJECTS_ANIMATION_DATA_H
#define DRAGONBONES_OBJECTS_ANIMATION_DATA_H

#include "DragonBones.h"
#include "TransformTimeline.h"
#include "SlotTimeline.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class AnimationData : public Timeline
{
public:
    AnimationData() :
		name("")
		,frameRate(30)
		,fadeTime(0.f)
		,playTimes(0)
		,tweenEasing(USE_FRAME_TWEEN_EASING)
		,autoTween(true)
		,lastFrameDuration(0)
    {}
    AnimationData(const AnimationData &copyData)
    {
        operator=(copyData);
    }
    AnimationData& operator=(const AnimationData &copyData)
    {
        Timeline::operator=(copyData);
        _dispose();
        autoTween = copyData.autoTween;
        frameRate = copyData.frameRate;
        playTimes = copyData.playTimes;
        fadeTime = copyData.fadeTime;
        tweenEasing = copyData.tweenEasing;
        name = copyData.name;
        timelineList.reserve(copyData.timelineList.size());
        
        for (size_t i = 0, l = timelineList.size(); i < l; ++i)
        {
            timelineList.push_back(new TransformTimeline());
            *(timelineList[i]) = *(copyData.timelineList[i]);
        }

		for (size_t i = 0, l = slotTimelineList.size(); i < l; ++i)
		{
			slotTimelineList.push_back(new SlotTimeline());
			*(slotTimelineList[i]) = *(copyData.slotTimelineList[i]);
		}
        
        // copy
        hideTimelineList = copyData.hideTimelineList;
        return *this;
    }
    virtual ~AnimationData()
    {
        dispose();
    }
    virtual void dispose()
    {
        Timeline::dispose();
        _dispose();
    }
    
    TransformTimeline *getTimeline(const std::string &timelineName) const
    {
        for (size_t i = 0, l = timelineList.size(); i < l; ++i)
        {
            if (timelineList[i]->name == timelineName)
            {
                return timelineList[i];
            }
        }
        return nullptr;
    }

	SlotTimeline *getSlotTimeline(const std::string &timelineName) const
	{
		for (size_t i = 0, l = slotTimelineList.size(); i < l; ++i)
		{
			if (slotTimelineList[i]->name == timelineName)
			{
				return slotTimelineList[i];
			}
		}
		return nullptr;
	}
    
private:
    void _dispose()
    {
        for (size_t i = 0, l = timelineList.size(); i < l; ++i)
        {
            timelineList[i]->dispose();
            delete timelineList[i];
        }
		for (size_t i = 0, l = slotTimelineList.size(); i < l; ++i)
		{
			slotTimelineList[i]->dispose();
			delete slotTimelineList[i];
		}
        
		slotTimelineList.clear();
        timelineList.clear();
        hideTimelineList.clear();
    }

public:
	std::string name;
	int frameRate;
	float fadeTime;
	int playTimes;
	// use frame tweenEase, NaN
	// overwrite frame tweenEase, [-1, 0):ease in, 0:line easing, (0, 1]:ease out, (1, 2]:ease in out
	float tweenEasing;
	bool autoTween;
	int lastFrameDuration;

	std::vector<std::string> hideTimelineList;
	std::vector<TransformTimeline*> timelineList;
	std::vector<SlotTimeline*> slotTimelineList;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_OBJECTS_ANIMATION_DATA_H
