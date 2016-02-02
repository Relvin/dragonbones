#ifndef PARSERS_XML_DATA_PARSER_H
#define PARSERS_XML_DATA_PARSER_H

#include "../DragonBones.h"
#include "BaseDataParser.h"

#include "dbtinyxml2.h"
#include "../dragonbones_generated.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class XMLDataParser : public BaseDataParser
{
private:
    static bool getBoolean(const XMLElement &data, const char *key, bool defaultValue);
    static float getNumber(const XMLElement &data, const char *key, float defaultValue, float nanValue);
    
private:
    mutable float _textureScale;
    mutable float _armatureScale;
    mutable int _frameRate;
    
public:
    XMLDataParser();
    virtual ~XMLDataParser();
    
    
    virtual DragonBonesData* parseDragonBonesData(const void *rawDragonBonesData, float scale = 1.f) const;
    virtual DragonBonesData* parseDragonBonesDataFromBinary(const void *rawDragonBonesData, float scale = 1.f) const;
private:
    
    ArmatureData* parseArmatureData(const XMLElement *armatureXML) const;
    BoneData* parseBoneData(const XMLElement *boneXML) const;
    SkinData* parseSkinData(const XMLElement *skinXML) const;
    SlotData* parseSlotData(const XMLElement *slotXML) const;
    DisplayData* parseDisplayData(const XMLElement *displayXML) const;
    AnimationData* parseAnimationData(const XMLElement *animationXML, const ArmatureData *armatureData) const;
    TransformTimeline* parseTransformTimeline(const XMLElement *timelineXML, int duration) const;
    Frame* parseMainFrame(const XMLElement *frameXML) const;
    TransformFrame* parseTransformFrame(const XMLElement *frameXML) const;
    RectangleData* parseRectangleData(const XMLElement *rectangleXML) const;
    EllipseData* parseEllipseData(const XMLElement *ellipseXML) const;
    
    void parseTimeline(const XMLElement &timelineXML, Timeline &timeline) const;
    void parseFrame(const XMLElement &frameXML, Frame &frame) const;
    void parseTransform(const XMLElement &transformXML, Transform &transform) const;
    void parsePivot(const XMLElement &transformXML, Point &pivot) const;
    void parseColorTransform(const XMLElement &colorTransformXML, ColorTransform &colorTransform) const;
    void parseTextData(const XMLElement &textXML, TextData &textData) const;
    
    
/**
 *
 * add by Relvin parse dragonBones from binary file
 *
 **/
    
    
    ArmatureData* parseArmatureDataFromBinary(const ArmatureOption * armatureDataOpt) const;
    BoneData* parseBoneDataFromBinary(const BoneOption* boneDataOpt) const;
    SkinData* parseSkinDataFromBinary(const SkinOption* skinDataOpt) const;
    SlotData* parseSlotDataFromBinary(const SlotOption *slotOption) const;
    DisplayData* parseDisplayDataFromBinary(const DisplayOption *displayOption) const;
    AnimationData* parseAnimationDataFromBinary(const AnimationOption *animationOption) const;
    TransformTimeline* parseTransformTimelineFromBinary(const TransformTimelineOption *timelineOption) const;
    Frame* parseMainFrameFromBinary(const FrameOption *frameOption) const;
    TransformFrame* parseTransformFrameFromBinary(const TransformFrameOption *transformFrameOption) const;
    RectangleData* parseRectangleDataFromBinary(const RectangleDataOption *rectangleOpt) const;
    EllipseData* parseEllipseDataFromBinary(const RectangleDataOption *ellipseOpt) const;

    void parseFrameFromBinary(const FrameOption *frameOption, Frame &frame) const;
    void parseTransformFromBinary(const TransformOption *transformOption, Transform &transform) const;
    void parsePivotFromBinary(const Vec2Option *point, Point &pivot) const;
    void parseColorTransformFromBinary(const ColorTransformOption *colorTransformOption, ColorTransform &colorTransform) const;
    void parseTextDataFromBinary(const TextDataOption *textDataOption, TextData &textData) const;
    
    void parseFlatStringToString(const flatbuffers::String *flatString,std::string &stdString) const;

    /** end **/
    
    
private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(XMLDataParser);
};
NAME_SPACE_DRAGON_BONES_END
#endif  // PARSERS_XML_DATA_PARSER_H
