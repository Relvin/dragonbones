#ifndef DRAGONBONES_PARSERS_BASE_DATA_PARSER_H
#define DRAGONBONES_PARSERS_BASE_DATA_PARSER_H

#include "DragonBones.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class TextureAtlasData;
class DragonBonesData;
class ArmatureData;
class AnimationData;
class BoneData;
class TransformFrame;
class TransformTimeline;
class Transform;

class BaseDataParser
{
public:
	BaseDataParser();
	virtual ~BaseDataParser();
	virtual TextureAtlasData *parseTextureAtlasData(const void *rawTextureAtlasData, float scale = 1.f) const = 0;
	virtual DragonBonesData *parseDragonBonesData(const void *rawDragonBonesData, float scale = 1.f) const = 0;

protected:
    static void transformArmatureData(ArmatureData *armatureData);
    static void transformArmatureDataAnimations(ArmatureData *armatureData);
    static void transformAnimationData(AnimationData *animationData, const ArmatureData *armatureData, bool isGlobalData = false);
    static void addHideTimeline(AnimationData *animationData, const ArmatureData *armatureData);
    
private:
    static void setFrameTransform(AnimationData *animationData, const ArmatureData *armatureData, const BoneData *boneData, TransformFrame *frame);
    static void getTimelineTransform(const TransformTimeline *timeline, int position, Transform *retult, bool isGlobal);
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(BaseDataParser);
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_PARSERS_BASE_DATA_PARSER_H
