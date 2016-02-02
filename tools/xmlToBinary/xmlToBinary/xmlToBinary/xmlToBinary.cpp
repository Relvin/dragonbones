//
//  xmlToBinary.cpp
//  Dragonbones
//
//  Created by Relvin on 15/12/2.
//  Copyright © 2015年 Relvin. All rights reserved.
//

#include "xmlToBinary.hpp"
#include "parsers/XMLDataParser.h"
#include "Platform.h"

static xmlToBinary* s_instance = nullptr;

xmlToBinary* xmlToBinary::getInstance()
{
    if (!s_instance)
    {
        s_instance = new (std::nothrow) xmlToBinary();
        s_instance->init();
    }
    
    return s_instance;
}

xmlToBinary::xmlToBinary()
: m_filePath ("")
, m_outPath ("")
, notCreate(false)
{
    
}

xmlToBinary::~xmlToBinary()
{
    
}

bool xmlToBinary::init()
{
    
    
    return true;
}

bool xmlToBinary::converBoneToBinaryByFile(std::string &path,std::string& outPath)
{
    this->m_filePath = path;
    
    
    this->m_outPath = outPath;
    size_t size = 0;
    size_t readsize = 0;
    std::string filename = path;
    size_t pos = filename.find_last_of('.');
    std::string suffix = filename.substr(pos + 1, path.length());
    
    if (suffix != "xml")
    {
        printf("%s is not xml file !\n",path.c_str());
        return false;
    }
    
    unsigned char* buffer = nullptr;
    do
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        FILE *fp;
        fopen_s(&fp,path.c_str(), "rb");
#else
        FILE *fp = fopen(path.c_str(), "rb");
#endif
        if (!fp)
        {
            printf("%s open error !\n",path.c_str());
            break;
        }
        fseek(fp,0,SEEK_END);
        size = ftell(fp);
        fseek(fp,0,SEEK_SET);
        
        
        buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);
        
        readsize = fread(buffer, sizeof(unsigned char), size, fp);
        fclose(fp);
        
    } while (0);
    
    if (nullptr == buffer || 0 == readsize)
    {
        printf("Get data from file %s failed!\n", path.c_str());
    }
    else
    {
        dragonBones::XMLDataParser parser;
        dragonBones::XMLDocument doc;
        doc.Parse(reinterpret_cast<char*>(buffer), readsize);
        auto dragonBonesData = parser.parseDragonBonesData(doc.RootElement());
        this->convertDragonboneToBinary(dragonBonesData);
        return true;
    }
    
    return false;
    
}


bool xmlToBinary::convertAreaData(flatbuffers::FlatBufferBuilder &builder,IAreaData* armAreaData,std::vector<flatbuffers::Offset<RectangleDataOption>> &areaDataOptArr)
{
    float width = 0.0f;
    float height = 0.0f;
    Transform transform;
    dragonBones::Point pivot;
    
    if (armAreaData->areaType == IAreaData::AreaType::AT_ELLIPSE)
    {
        EllipseData* rectData = dynamic_cast<EllipseData* >(armAreaData);
        if (!rectData)
        {
            return false;
        }
        width = rectData->width;
        height = rectData->height;
        transform = rectData->transform;
        pivot = rectData->pivot;
    }
    else
    {
        RectangleData* rectData = dynamic_cast<RectangleData* >(armAreaData);
        if (!rectData)
        {
            return false;
        }
        width = rectData->width;
        height = rectData->height;
        transform = rectData->transform;
        pivot = rectData->pivot;
    }
    
    TransformOption transformOption(transform.x,transform.y,transform.skewX,transform.skewY,transform.scaleX,transform.scaleY);
    Vec2Option pivotOpt(pivot.x,pivot.y);
    
    int areaType = (int)armAreaData->areaType ;
    auto areaOption = CreateAreaDataOption(builder,
                                           areaType,
                                           armAreaData->name.length() == 0 ? 0 : builder.CreateString(armAreaData->name.c_str()));
    
    auto rectangleDataOption = CreateRectangleDataOption(builder,
                                                         width,
                                                         height,
                                                         &transformOption,
                                                         &pivotOpt,
                                                         areaOption);
    
    areaDataOptArr.push_back(rectangleDataOption);
    return true;
}

bool xmlToBinary::convertBoneData(flatbuffers::FlatBufferBuilder &builder,BoneData* boneData,std::vector<flatbuffers::Offset<BoneOption>> &boneOptionVec)
{
    std::vector<flatbuffers::Offset<RectangleDataOption>> bAreaDataOptArr;
    for (int bAreaIdx = 0; bAreaIdx < boneData->areaDataList.size(); bAreaIdx++) {
        convertAreaData(builder,boneData->areaDataList.at(bAreaIdx), bAreaDataOptArr);
    }
    
    bool inheritScale = boneData->inheritScale;
    bool inheritRotation = boneData->inheritRotation;
    float length = boneData->length;
    
    std::string name = boneData->name;
    std::string parent = boneData->parent;
    Transform global = boneData->global;
    TransformOption globalOption(global.x,global.y,global.skewX,global.skewY,global.scaleX,global.scaleY);
    Transform transform = boneData->transform;
    TransformOption transformOption(transform.x,transform.y,transform.skewX,transform.skewY,transform.scaleX,transform.scaleY);
    
    
    auto boneOption = CreateBoneOption(builder,
                                       name.length() == 0 ? 0 : builder.CreateString(name),
                                       parent.length() == 0 ? 0 : builder.CreateString(parent),
                                       length,
                                       inheritScale,
                                       inheritRotation,
                                       &globalOption,
                                       &transformOption,
                                       bAreaDataOptArr.size() == 0 ? 0 : builder.CreateVector(bAreaDataOptArr));
    boneOptionVec.push_back(boneOption);
    return true;
}

bool xmlToBinary::convertDisplayData(flatbuffers::FlatBufferBuilder &builder,DisplayData* displayData,std::vector<flatbuffers::Offset<DisplayOption>> &displayDataArr)
{
    TextData *textData = displayData->textData;
    flatbuffers::Offset<TextDataOption> textDataOpt;
    if (textData != NULL)
    {
        textDataOpt = CreateTextDataOption(builder,
                                           textData->bold,
                                           textData->italic,
                                           textData->htmlText,
                                           textData->size,
                                           textData->alpha,
                                           textData->red,
                                           textData->green,
                                           textData->blue,
                                           textData->width,
                                           textData->height,
                                           textData->letterSpacing,
                                           textData->lineSpacing,
                                           textData->maxCharacters,
                                           textData->face.length() == 0 ? 0 : builder.CreateString(textData->face),
                                           textData->text.length() == 0 ? 0 : builder.CreateString(textData->text),
                                           (int)textData->alignH,
                                           (int)textData->alignV,
                                           (int)textData->textType,
                                           (int)textData->lineType
                                           );
    }
    
    bool scalingGrid = displayData->scalingGrid;
    int left = displayData->scalingGridLeft;
    int right = displayData->scalingGridRight;
    int top = displayData->scalingGridTop;
    int bottom = displayData->scalingGridBottom;
    
    std::string name = displayData->name;
    
    uint8_t type = (uint8_t)displayData->type;
    Transform transform = displayData->transform;
    dragonBones::Point pivot = displayData->pivot;
    TransformOption transformOption(transform.x,transform.y,transform.skewX,transform.skewY,transform.scaleX,transform.scaleY);
    Vec2Option point(pivot.x,pivot.y);
    auto dispLayDataOption = CreateDisplayOption(builder,
                                                 name.length() == 0 ? 0 : builder.CreateString(name),
                                                 type,
                                                 scalingGrid,
                                                 left,
                                                 right,
                                                 top,
                                                 bottom,
                                                 &point,
                                                 &transformOption,
                                                 textData == NULL ?  0 : textDataOpt);
    displayDataArr.push_back(dispLayDataOption);

    return true;
}

bool xmlToBinary::convertSlotData(flatbuffers::FlatBufferBuilder &builder,SlotData* slotData,std::vector<flatbuffers::Offset<SlotOption>> &slotDataArr)
{

        
        std::vector<flatbuffers::Offset<DisplayOption>> displayDataArr;
        for (int disIdx = 0; disIdx < slotData->displayDataList.size(); disIdx++)
        {
            convertDisplayData(builder,slotData->displayDataList.at(disIdx), displayDataArr);
        }
        
        float zOrder = slotData->zOrder;
        std::string name = slotData->name;
        std::string parent = slotData->parent;
        auto slotOption = CreateSlotOption(builder,
                                           zOrder,
                                           name.length() == 0 ? 0 : builder.CreateString(name),
                                           parent.length() == 0 ? 0 : builder.CreateString(parent),
                                           (unsigned char)slotData->blendMode,
                                           displayDataArr.size() == 0 ? 0 : builder.CreateVector(displayDataArr));
        slotDataArr.push_back(slotOption);
    
    return true;
}

bool xmlToBinary::convertSkinData(flatbuffers::FlatBufferBuilder &builder,dragonBones::SkinData* skinData,std::vector<flatbuffers::Offset<SkinOption>> &skinOptionVec)
{
    
    std::vector<flatbuffers::Offset<SlotOption>> slotDataArr;
    for (int slotIdx = 0; slotIdx < skinData->slotDataList.size(); slotIdx++)
    {
        convertSlotData(builder,skinData->slotDataList.at(slotIdx), slotDataArr);
    }
    
    auto skinName = builder.CreateString(skinData->name);
    skinOptionVec.push_back(CreateSkinOption(builder,
                                             skinData->name.length() == 0 ? 0 : skinName,
                                             slotDataArr.size() == 0 ? 0 : builder.CreateVector(slotDataArr)));
    return true;
}

bool xmlToBinary::convertTransformFrameData(flatbuffers::FlatBufferBuilder &builder,TransformFrame* tfFrame,std::vector<flatbuffers::Offset<TransformFrameOption>> &frameOptionArr)
{
    if (!tfFrame)
    {
        return false;
    }
    
    std::string action = tfFrame->action;
    std::string event = tfFrame->event;
    std::string sound = tfFrame->sound;
    std::string eventPs = tfFrame->eventParameters;
    
    auto frameOption = CreateFrameOption(builder,
                                         tfFrame->position,
                                         tfFrame->duration,
                                         (uint8_t)tfFrame->frameType,
                                         action.length() == 0 ? 0 : builder.CreateString(action),
                                         event.length() == 0 ? 0 : builder.CreateString(event),
                                         sound.length() == 0 ? 0 : builder.CreateString(sound),
                                         eventPs.length() == 0 ? 0 : builder.CreateString(eventPs));
    
    
    ColorTransform *color = tfFrame->color;
    flatbuffers::Offset<ColorTransformOption> colorTfOpt;
    if (color)
    {
        colorTfOpt = CreateColorTransformOption(builder,
                                                color->alphaMultiplier,
                                                color->redMultiplier,
                                                color->greenMultiplier,
                                                color->blueMultiplier,
                                                color->alphaOffset,
                                                color->redOffset,
                                                color->greenOffset,
                                                color->blueOffset
                                                );
    }
    
    Transform global = tfFrame->global;
    TransformOption frameglobalOption(global.x,global.y,global.skewX,global.skewY,global.scaleX,global.scaleY);
    Transform transform = tfFrame->transform;
    TransformOption frameTransformOption(transform.x,transform.y,transform.skewX,transform.skewY,transform.scaleX,transform.scaleY);
    dragonBones::Point pivot = tfFrame->pivot;
    Vec2Option pivotOpt(pivot.x, pivot.y);
    dragonBones::Point scaleOffset = tfFrame->scaleOffset;
    Vec2Option scaleOffsetOpt(scaleOffset.x, scaleOffset.y);
    auto tfFrameOption = CreateTransformFrameOption(builder,
                                                    tfFrame->visible,
                                                    tfFrame->tweenScale,
                                                    tfFrame->tweenRotate,
                                                    tfFrame->displayIndex,
                                                    tfFrame->zOrder,
                                                    tfFrame->tweenEasing,
                                                    &frameglobalOption,
                                                    &frameTransformOption,
                                                    &pivotOpt,
                                                    &scaleOffsetOpt,
                                                    color == nullptr ? 0 : colorTfOpt,
                                                    frameOption
                                                    );
    frameOptionArr.push_back(tfFrameOption);
    return true;
}

bool xmlToBinary::convertTransformTimelineData(flatbuffers::FlatBufferBuilder &builder,TransformTimeline* timeline,std::vector<flatbuffers::Offset<TransformTimelineOption>> &transformTimelineOptArr)
{
    std::vector<flatbuffers::Offset<TransformFrameOption>> frameOptionArr;
    
    for (int flIdx = 0; flIdx < timeline->frameList.size(); flIdx++)
    {
        convertTransformFrameData(builder,dynamic_cast<TransformFrame*>(timeline->frameList.at(flIdx)), frameOptionArr);
    }
    
    
    bool transformed = timeline->transformed;
    float offset = timeline->offset;
    
    std::string name = timeline->name;
    Transform transform = timeline->originTransform;
    
    TransformOption transformOption(transform.x,transform.y,transform.skewX,transform.skewY,transform.scaleX,transform.scaleY);
    
    dragonBones::Point originPivot = timeline->originPivot;
    
    Vec2Option point(originPivot.x,originPivot.y);
    
    
    auto timelinOption = CreateTimelineOption(builder,
                                              timeline->duration,
                                              timeline->scale,
                                              frameOptionArr.size() == 0 ? 0 : builder.CreateVector(frameOptionArr));
    auto transformTimelineOpt = CreateTransformTimelineOption(builder,
                                                              transformed,
                                                              offset,
                                                              name.length() == 0 ? 0 : builder.CreateString(name),
                                                              &transformOption,
                                                              &point,
                                                              timelinOption);
    transformTimelineOptArr.push_back(transformTimelineOpt);
    
    return true;
}

bool xmlToBinary::convertAnimationData(flatbuffers::FlatBufferBuilder &builder, AnimationData* animationData,std::vector<flatbuffers::Offset<AnimationOption>> &animationOptionOptArr)
{
    std::vector<flatbuffers::Offset<TransformTimelineOption>> transformTimelineOptArr;
    for (int tlIdx = 0; tlIdx < animationData->timelineList.size(); tlIdx++)
    {
        convertTransformTimelineData(builder,animationData->timelineList.at(tlIdx),transformTimelineOptArr);
    }
    
    bool autoTween = animationData->autoTween;
    int frameRate = animationData->frameRate;
    int playTimes = animationData->playTimes;
    float fadeTime = animationData->fadeTime;
    float tweenEasing = animationData->tweenEasing;
    std::string name = animationData->name;
    
    std::vector<flatbuffers::Offset<FrameOption>> frameOptionArr;
    {
        
        for (int flIdx = 0; flIdx < animationData->frameList.size(); flIdx++) {
            Frame* frame = animationData->frameList.at(flIdx);
            
            std::string action = frame->action;
            std::string event = frame->event;
            std::string sound = frame->sound;
            std::string eventPs = frame->eventParameters;
            
            auto frameOption = CreateFrameOption(builder,
                                                 frame->position,
                                                 frame->duration,
                                                 (uint8_t)frame->frameType,
                                                 action.length() == 0 ? 0 : builder.CreateString(action),
                                                 event.length() == 0 ? 0 : builder.CreateString(event),
                                                 sound.length() == 0 ? 0 : builder.CreateString(sound),
                                                 eventPs.length() == 0 ? 0 : builder.CreateString(eventPs));
            
            
            
            frameOptionArr.push_back(frameOption);
        }
        
    }
    
    std::vector<flatbuffers::Offset<flatbuffers::String>> hideTimelineArr;
    for (int hideIdx = 0; hideIdx < animationData->hideTimelineList.size(); hideIdx++) {
        std::string hideTimeline = animationData->hideTimelineList.at(hideIdx);
        if (hideTimeline.length() != 0)
        {
            hideTimelineArr.push_back(builder.CreateString(hideTimeline));
        }
        
    }

    auto timelinOption = CreateNormalTimelineOption(builder,
                                                    animationData->duration,
                                                    animationData->scale,
                                                    frameOptionArr.size() == 0 ? 0 : builder.CreateVector(frameOptionArr));
    auto animationOption = CreateAnimationOption(builder,
                                                 autoTween,
                                                 frameRate,
                                                 playTimes,
                                                 fadeTime,
                                                 tweenEasing,
                                                 name.length() == 0 ? 0 : builder.CreateString(name),
                                                 timelinOption,
                                                 transformTimelineOptArr.size() == 0 ? 0 : builder.CreateVector(transformTimelineOptArr),
                                                 hideTimelineArr.size() == 0 ? 0 : builder.CreateVector(hideTimelineArr));
    animationOptionOptArr.push_back(animationOption);
    
    return true;
}

bool xmlToBinary::convertDragonboneToBinary(dragonBones::DragonBonesData *dragonbonesData)
{
    flatbuffers::FlatBufferBuilder builder;
    std::vector<flatbuffers::Offset<ArmatureOption>> armatureOptVec;
    for (int armIdx = 0;armIdx < dragonbonesData->armatureDataList.size();armIdx++)
    {
        ArmatureData* armData = dragonbonesData->armatureDataList.at(armIdx);
        std::vector<flatbuffers::Offset<RectangleDataOption>> areaDataOptArr;
        for (int areaIdx = 0; areaIdx < armData->areaDataList.size(); areaIdx++)
        {
            convertAreaData(builder,armData->areaDataList.at(areaIdx), areaDataOptArr);
        }
        
        std::vector<flatbuffers::Offset<BoneOption>> boneOptionVec;
        for (int boneIdx = 0;boneIdx < armData->boneDataList.size();boneIdx++)
        {
            convertBoneData(builder,armData->boneDataList.at(boneIdx), boneOptionVec);
        }
        
        std::vector<flatbuffers::Offset<SkinOption>> skinOptionVec;
        for (int skinIdx = 0; skinIdx < armData->skinDataList.size(); skinIdx++)
        {
            convertSkinData(builder,armData->skinDataList.at(skinIdx), skinOptionVec);
        }
        std::vector<flatbuffers::Offset<AnimationOption>> animationOptionOptArr;
        for (int aniIdx = 0; aniIdx < armData->animationDataList.size(); aniIdx++)
        {
            convertAnimationData(builder,armData->animationDataList.at(aniIdx), animationOptionOptArr);
        }
        auto armatureOpt = CreateArmatureOption(builder,
                                                skinOptionVec.size() == 0 ? 0 : builder.CreateVector(skinOptionVec),
                                                animationOptionOptArr.size() == 0 ? 0 : builder.CreateVector(animationOptionOptArr),
                                                armData->name.length() == 0 ? 0 : builder.CreateString(armData->name.c_str()),
                                                boneOptionVec.size() == 0 ? 0 : builder.CreateVector(boneOptionVec),
                                                areaDataOptArr.size() == 0 ? 0 : builder.CreateVector(areaDataOptArr));
        armatureOptVec.push_back(armatureOpt);
    }
    

    auto dragonBoneBinaryData = CreateDragonBones(builder,
                                                  dragonbonesData->name.length() == 0 ? 0 : builder.CreateString(dragonbonesData->name.c_str()),
                                                  dragonbonesData->autoSearch,
                                                  armatureOptVec.size() == 0 ? 0 : builder.CreateVector(armatureOptVec));
    FinishDragonBonesBuffer(builder, dragonBoneBinaryData);
    
    this->writeToPath((char *)builder.GetBufferPointer(), builder.GetSize());
    
    
    return true;
}

bool xmlToBinary::convertBinaryBone(dragonBones::DragonBonesData *dragonbonesData)
{
    //    dragonbonesData->armatureDataList.push_back();
    return true;
}

bool xmlToBinary::writeToPath(const char* data,size_t size)
{
	printf("%s\n", m_filePath.c_str());
    std::string binaryFilePath = m_filePath;
    std::string suffix = ".xmlb";
    std::string outFilePath = "";
    size_t final_pos = 0;
    char mode = '/';
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    
    mode = '\\';
#else
    mode = '/';
#endif
    final_pos = m_outPath.rfind(mode);
    if (0 < final_pos) {
        outFilePath = m_outPath.substr(0, final_pos + 1);
    }
    
    std::string fileName = "";
    
	final_pos = m_filePath.rfind(mode);
    size_t dot_pos = m_filePath.rfind('.');
    if (0 < dot_pos) {
        final_pos = final_pos == 0 ? 0 : final_pos + 1;
		fileName = m_filePath.substr(final_pos, dot_pos - final_pos);
    }
    fileName += suffix;
    binaryFilePath = outFilePath + fileName;
    
	
	if (Platform::DirIsExist(outFilePath.c_str()) == false && notCreate == false)
    {
		printf("%s outFilePath not exit \n",outFilePath.c_str());
		Platform::create_multi_dir(outFilePath.c_str());
		notCreate = true;
    }
    
    FILE *fp;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	
	fopen_s(&fp, binaryFilePath.c_str(), "wb");
#else
	fp = fopen(binaryFilePath.c_str(), "wb");
#endif
    
    if (!fp)
    {
        printf("%s write error !\n",binaryFilePath.c_str());
        return false;
    }
    fwrite(data, sizeof(unsigned char), size, fp);//(buffer,sizeof(unsigned char), *size,fp);
    fclose(fp);
	printf("=====>%s\n", binaryFilePath.c_str());
    return true;
}

