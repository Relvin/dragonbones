#ifndef DRAGONBONES_PARSERS_JSON_DATA_PARSER_H
#define DRAGONBONES_PARSERS_JSON_DATA_PARSER_H

#include "3rd/rapidjson/document.h"

#include "DragonBones.h"
#include "BaseDataParser.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class TextureAtlasData;
class DragonBonesData;
class ArmatureData;
class AnimationData;
class BoneData;
class TransformFrame;
class TransformTimeline;
class Transform;
class TextureData;
class SkinData;
class SlotData;
class DisplayData;
class SlotTimeline;
class SlotFrame;
class Frame;
class Timeline;
class Point;
class ColorTransform;

class JSONDataParser : public BaseDataParser
{
public:
	static TextureAtlasData* parseTextureAtlasData(const char *rawTextureAtlasData);
    static DragonBonesData* parseDragonBonesData(const char *rawDragonBonesData);
    
private:
    static TextureData* parseTextureData(const rapidjson::Value &inTextures);
    static ArmatureData* parseArmatureData(const rapidjson::Value &inArmatures, int frameRate = 30);
    static BoneData* parseBoneData(const rapidjson::Value &inBones);
    static SkinData* parseSkinData(const rapidjson::Value &inSkins);
	static SlotData* parseSlotDisplayData(const rapidjson::Value &inSlots);
    static SlotData* parseSlotData(const rapidjson::Value &inSlots);
    static DisplayData* parseDisplayData(const rapidjson::Value &inDisplay);
    static AnimationData* parseAnimationData(const rapidjson::Value &inAnimations, const ArmatureData *armatureData, int frameRate = 30);
    static TransformTimeline* parseTransformTimeline(const rapidjson::Value &inTimeline, int duration);
	static SlotTimeline* parseSlotTimeline(const rapidjson::Value &inFrame, int duration);
	static SlotFrame* parseSlotFrame(const rapidjson::Value &inTimeline);
    static Frame* parseMainFrame(const rapidjson::Value &inFrame, int frameRate = 30);
    static TransformFrame* parseTransformFrame(const rapidjson::Value &inFrame);
    
    static void parseTimeline(Timeline &timeline);
    static void parseFrame(const rapidjson::Value &inFrame, Frame &frame, int frameRate = 30);
    static void parseTransform(const rapidjson::Value &inTransform, Transform &transform);
    static void parsePivot(const rapidjson::Value &inTransform, Point &pivot);
    static void parseColorTransform(const rapidjson::Value &inColorTransform, ColorTransform &colorTransform); 
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_PARSERS_JSON_DATA_PARSER_H
