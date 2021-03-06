//
//  BinaryParser.hpp
//  xmlToBinary
//
//  Created by Relvin on 16/3/29.
//  Copyright © 2016年 Relvin. All rights reserved.
//

#ifndef BinaryParser_hpp
#define BinaryParser_hpp

#include "DragonBones.h"
#include "geoms/Point.h"

namespace flatbuffers {
    struct String;
}

NAME_SPACE_DRAGON_BONES_BEGIN
/*flatfuffer data structure begin*/

struct PointOption;
struct TransformOption;
struct Vec2Option;
struct IKDataOption;
struct CurveDataOption;
struct FrameOption;
struct TransformFrameOption;
struct ColorTransformOption;
struct SlotFrameOption;
struct FFDFrameOption;
struct TimelineOption;
struct TransformTimelineOption;
struct DisplayDataOption;
struct MeshDataOption;
struct SlotTimelineOption;
struct FFDTimelineOption;
struct BoneDataOption;
struct SlotDataOption;
struct SkinDataOption;
struct AnimationDataOption;
struct ArmatureOption;
struct DragonBonesParseBinary;

/*flatfuffer data structure end*/

class DragonBonesData;
class ArmatureData;
class AnimationData;
class BoneData;
class TransformFrame;
class TransformTimeline;
class Transform;
class SkinData;
class SlotData;
class IKData;
class DisplayData;
class SlotTimeline;
class FFDTimeline;
class SlotFrame;
class Frame;
class Timeline;
class ColorTransform;
class FFDFrame;
class MeshData;


class BinaryParser
{
public:
    BinaryParser();
    virtual ~BinaryParser();
    
    virtual DragonBonesData* parseDragonBonesData(const void *rawDragonBonesData,float scale = 1.f) const;
    
protected:
    
    
    ArmatureData* parseArmatureDataFromBinary(const ArmatureOption * armatureOption) const;
    BoneData* parseBoneDataFromBinary(const BoneDataOption* boneDataOption) const;
    SlotData* parseSlotDataFromBinary(const SlotDataOption* slotDataOption) const;
    
    MeshData* parseMeshDataFromBinary(const MeshDataOption* meshDataOption) const;
    DisplayData* parseMainDisplayDataFromBinary(const DisplayDataOption* displayDataOption) const;
    void parseDisplayDataFromBinary(DisplayData *displayData,const DisplayDataOption* displayDataOption) const;
    
    SkinData* parseSkinDataFromBinary(const SkinDataOption* skinDataOption) const;
    IKData* parseIKDataFromBinary(const IKDataOption* ikDataOption) const;
    AnimationData* parseAnimationDataFromBinary(const AnimationDataOption *animationDataOption) const;
    
    TransformTimeline* parseTransformTimelineFromBinary(const TransformTimelineOption *timelineOption) const;
    SlotTimeline* parseSlotTimelineFromBinary(const SlotTimelineOption *timelineOption) const;
    
    FFDTimeline* parseFFDTimelineFromBinary(const FFDTimelineOption *timelineOption) const;
    FFDFrame* parseFFDFrameFromBinary(const FFDFrameOption *ffdFrameOption) const;
    
    Frame* parseMainFrameFromBinary(const FrameOption *frameOption) const;

    TransformFrame* parseTransformFrameFromBinary(const TransformFrameOption *transformFrameOption) const;
    
    SlotFrame* parseSlotFrameFromBinary(const SlotFrameOption *slotFrameOption) const;
    void parseFrameFromBinary(const FrameOption *frameOption, Frame *frame) const;
    
    ColorTransform* parseColorTransformFromBinary(const ColorTransformOption *colorTransformOption) const;
    void parseFlatStringToString(const flatbuffers::String *flatString,std::string &stdString) const;
    void parseTransformFromBinary(const TransformOption *transformOption, Transform &transform) const;
    void parsePointFromBinary(const PointOption *pointOption, Point &point) const;
private:
    mutable int _frameRate;
    mutable bool _isGlobalData;
    mutable float _armatureScale;
    
    
};


NAME_SPACE_DRAGON_BONES_END

#endif /* BinaryParser_hpp */
