#ifndef DRAGONBONES_PARSERS_XML_DATA_PARSER_H
#define DRAGONBONES_PARSERS_XML_DATA_PARSER_H

#include "3rd/tinyxml2/dbtinyxml2.h"

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

class XMLDataParser : public BaseDataParser
{
public:
    XMLDataParser();
    virtual ~XMLDataParser();
    virtual TextureAtlasData* parseTextureAtlasData(const void *rawTextureAtlasData, float scale = 1.f) const;
    virtual DragonBonesData* parseDragonBonesData(const void *rawDragonBonesData,float scale = 1.f) const;
    
private:
	static bool getBoolean(const XMLElement &data, const char *key, bool defaultValue);
	static float getNumber(const XMLElement &data, const char *key, float defaultValue, float nanValue);

    TextureData* parseTextureData(const XMLElement *textureXML) const;    
    ArmatureData* parseArmatureData(const XMLElement *armatureXML) const;
    BoneData* parseBoneData(const XMLElement *boneXML) const;
    SkinData* parseSkinData(const XMLElement *skinXML) const;
	SlotData* parseSlotDisplayData(const XMLElement *slotXML) const;
    SlotData* parseSlotData(const XMLElement *slotXML) const;
    DisplayData* parseDisplayData(const XMLElement *displayXML) const;
    AnimationData* parseAnimationData(const XMLElement *animationXML, const ArmatureData *armatureData) const;
    TransformTimeline* parseTransformTimeline(const XMLElement *timelineXML, int duration) const;
	SlotTimeline* parseSlotTimeline(const XMLElement *frameXML, int duration) const;
	SlotFrame* parseSlotFrame(const XMLElement *timelineXML) const;
    Frame* parseMainFrame(const XMLElement *frameXML) const;
    TransformFrame* parseTransformFrame(const XMLElement *frameXML) const;
    
    void parseTimeline(const XMLElement &timelineXML, Timeline &timeline) const;
    void parseFrame(const XMLElement &frameXML, Frame &frame) const;
    void parseTransform(const XMLElement &transformXML, Transform &transform) const;
    void parsePivot(const XMLElement &transformXML, Point &pivot) const;
    void parseColorTransform(const XMLElement &colorTransformXML, ColorTransform &colorTransform) const;
    
private:
	mutable float _textureScale;
	mutable int _frameRate;
	mutable bool _isGlobalData;
    mutable float _armatureScale;

private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(XMLDataParser);
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_PARSERS_XML_DATA_PARSER_H
