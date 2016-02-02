//
//  xmlToBinary.hpp
//  Dragonbones
//
//  Created by Relvin on 15/12/2.
//  Copyright © 2015年 Relvin. All rights reserved.
//

#ifndef xmlToBinary_hpp
#define xmlToBinary_hpp

#include <stdio.h>
#include "objects/DragonBonesData.h"
#include "dragonbones_generated.h"

using namespace dragonBones;
class xmlToBinary
{
public:
    static xmlToBinary* getInstance();
    
    xmlToBinary();
    virtual ~xmlToBinary();
    
    virtual bool init();
    
    
    bool convertAreaData(flatbuffers::FlatBufferBuilder &builder,IAreaData* armAreaData,std::vector<flatbuffers::Offset<RectangleDataOption>> &areaDataOptArr);
    bool convertBoneData(flatbuffers::FlatBufferBuilder &builder,BoneData* boneData,std::vector<flatbuffers::Offset<BoneOption>> &boneOptionVec);
    bool convertDisplayData(flatbuffers::FlatBufferBuilder &builder,DisplayData* displayData,std::vector<flatbuffers::Offset<DisplayOption>> &displayDataArr);
    bool convertSlotData(flatbuffers::FlatBufferBuilder &builder,SlotData* slotData,std::vector<flatbuffers::Offset<SlotOption>> &slotDataArr);
    bool convertSkinData(flatbuffers::FlatBufferBuilder &builder,dragonBones::SkinData* skinData,std::vector<flatbuffers::Offset<SkinOption>> &skinOptionVec);
    bool convertTransformFrameData(flatbuffers::FlatBufferBuilder &builder,TransformFrame* tfFrame,std::vector<flatbuffers::Offset<TransformFrameOption>> &frameOptionArr);
    bool convertTransformTimelineData(flatbuffers::FlatBufferBuilder &builder,TransformTimeline* timeline,std::vector<flatbuffers::Offset<TransformTimelineOption>> &transformTimelineOptArr);
    bool convertAnimationData(flatbuffers::FlatBufferBuilder &builder, AnimationData* animationData,std::vector<flatbuffers::Offset<AnimationOption>> &animationOptionOptArr);
    
    
    bool converBoneToBinaryByFile(std::string &path,std::string& outPath);
    bool convertDragonboneToBinary (dragonBones::DragonBonesData *bonesData);
    bool convertBinaryBone (dragonBones::DragonBonesData *bonesData);
    
    bool writeToPath(const char* data,size_t size);
private:
    flatbuffers::FlatBufferBuilder builder;
    std::string m_filePath;
    std::string m_outPath;
	bool notCreate;
    
};


#endif /* xmlToBinary_hpp */
