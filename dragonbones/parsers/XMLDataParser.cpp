#include "XMLDataParser.h"

#include "ConstValues.h"
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
bool XMLDataParser::getBoolean(const XMLElement &data, const char *key, bool defaultValue)
{
    if (data.FindAttribute(key))
    {
        const char *value = data.Attribute(key);
        
        if (
            strcmp(value, "0") == 0 ||
            strcmp(value, "NaN") == 0 ||
            strcmp(value, "") == 0 ||
            strcmp(value, "false") == 0 ||
            strcmp(value, "null") == 0 ||
            strcmp(value, "undefined") == 0
        )
        {
            return false;
        }
        else
        {
            return data.BoolAttribute(key);
        }
    }
    
    return defaultValue;
}

float XMLDataParser::getNumber(const XMLElement &data, const char *key, float defaultValue, float nanValue)
{
    if (data.FindAttribute(key))
    {
        const char *value = data.Attribute(key);
        
        if (
            strcmp(value, "NaN") == 0 ||
            strcmp(value, "") == 0 ||
            strcmp(value, "false") == 0 ||
            strcmp(value, "null") == 0 ||
            strcmp(value, "undefined") == 0
        )
        {
            return nanValue;
        }
        else
        {
            return data.FloatAttribute(key);
        }
    }
    
    return defaultValue;
}

XMLDataParser::XMLDataParser() :
	_textureScale(1.f)
	,_frameRate(30)
	,_isGlobalData(false)
    , _armatureScale(1.0f)
{}
XMLDataParser::~XMLDataParser()
{
}

TextureAtlasData* XMLDataParser::parseTextureAtlasData(const void *rawTextureAtlasData, float scale) const
{
    _textureScale = scale;
    const XMLElement *textureAtlasXML = static_cast<const XMLElement*>(rawTextureAtlasData);
    TextureAtlasData *textureAtlasData = new TextureAtlasData();
    const char *name = textureAtlasXML->Attribute(ConstValues::A_NAME.c_str());
    textureAtlasData->name = name ? name : "";
    textureAtlasData->imagePath = textureAtlasXML->Attribute(ConstValues::A_IMAGE_PATH.c_str());
    const char *format = textureAtlasXML->Attribute(ConstValues::A_IMAGE_FORMAT.c_str());
    textureAtlasData->format = getPixelFormatByString(format ? format : "");
    
    for (const XMLElement *textureXML = textureAtlasXML->FirstChildElement(ConstValues::SUB_TEXTURE.c_str()); 
		textureXML; textureXML = textureXML->NextSiblingElement(ConstValues::SUB_TEXTURE.c_str()))
    {
        TextureData *textureData = parseTextureData(textureXML);
        textureAtlasData->textureDataList.push_back(textureData);
    }
    
    return textureAtlasData;
}

TextureData* XMLDataParser::parseTextureData(const XMLElement *textureXML) const
{
    TextureData *textureData = new TextureData();
    textureData->name = textureXML->Attribute(ConstValues::A_NAME.c_str());
    textureData->rotated = textureXML->BoolAttribute(ConstValues::A_ROTATED.c_str());
    textureData->region.x = textureXML->FloatAttribute(ConstValues::A_X.c_str()) / _textureScale;
    textureData->region.y = textureXML->FloatAttribute(ConstValues::A_Y.c_str()) / _textureScale;
    textureData->region.width = textureXML->FloatAttribute(ConstValues::A_WIDTH.c_str()) / _textureScale;
    textureData->region.height = textureXML->FloatAttribute(ConstValues::A_HEIGHT.c_str()) / _textureScale;
    const float frameWidth = textureXML->FloatAttribute(ConstValues::A_FRAME_WIDTH.c_str()) / _textureScale;
    const float frameHeight = textureXML->FloatAttribute(ConstValues::A_FRAME_HEIGHT.c_str()) / _textureScale;
    
    if (frameWidth > 0 && frameHeight > 0)
    {
        textureData->frame = new Rectangle();
        textureData->frame->x = textureXML->FloatAttribute(ConstValues::A_FRAME_X.c_str()) / _textureScale;
        textureData->frame->y = textureXML->FloatAttribute(ConstValues::A_FRAME_Y.c_str()) / _textureScale;
        textureData->frame->width = frameWidth;
        textureData->frame->height = frameHeight;
    }
    
    return textureData;
}

DragonBonesData* XMLDataParser::parseDragonBonesData(const void *rawDragonBonesData,float scale) const
{
    _armatureScale = 1;
    const XMLElement *dragonBonesXML = static_cast<const XMLElement*>(rawDragonBonesData);
    std::string version = dragonBonesXML->Attribute(ConstValues::A_VERSION.c_str());
    // TODO
    /*
    switch(version)
    {
    
    }
    */
    _frameRate = dragonBonesXML->IntAttribute(ConstValues::A_FRAME_RATE.c_str());

	DragonBonesData *dragonBonesData = new DragonBonesData();
    dragonBonesData->name = dragonBonesXML->Attribute(ConstValues::A_NAME.c_str());
	dragonBonesData->isGlobalData = _isGlobalData = 
		(strcmp(dragonBonesXML->Attribute(ConstValues::A_IS_GLOBAL.c_str()), "0") == 0) ? false : true;
    
    for (const XMLElement *armatureXML = dragonBonesXML->FirstChildElement(ConstValues::ARMATURE.c_str()); 
			armatureXML; armatureXML = armatureXML->NextSiblingElement(ConstValues::ARMATURE.c_str()))
    {
        ArmatureData *armatureData = parseArmatureData(armatureXML);
        dragonBonesData->armatureDataList.push_back(armatureData);
    }
    
    return dragonBonesData;
}

ArmatureData* XMLDataParser::parseArmatureData(const XMLElement *armatureXML) const
{
    ArmatureData *armatureData = new ArmatureData();
    armatureData->name = armatureXML->Attribute(ConstValues::A_NAME.c_str());
    
    for (const XMLElement *boneXML = armatureXML->FirstChildElement(ConstValues::BONE.c_str()); 
		boneXML; boneXML = boneXML->NextSiblingElement(ConstValues::BONE.c_str()))
    {
        BoneData *boneData = parseBoneData(boneXML);
        armatureData->boneDataList.push_back(boneData);
    }
	
	for (const XMLElement *slotXML = armatureXML->FirstChildElement(ConstValues::SLOT.c_str()); 
		slotXML; slotXML = slotXML->NextSiblingElement(ConstValues::SLOT.c_str()))
	{
		SlotData *slotData = parseSlotData(slotXML);
		armatureData->slotDataList.push_back(slotData);
	}
   
    for (const XMLElement *skinXML = armatureXML->FirstChildElement(ConstValues::SKIN.c_str()); 
		skinXML; skinXML = skinXML->NextSiblingElement(ConstValues::SKIN.c_str()))
    {
        SkinData *skinData = parseSkinData(skinXML);
        armatureData->skinDataList.push_back(skinData);
    }
		
	// 4.0开始不再支持绝对值，这里的isGlobal只是为了匹配格式说明
	if (_isGlobalData)
	{
		transformArmatureData(armatureData);
	}
    armatureData->sortBoneDataList();
    
    for (const XMLElement *animationXML = armatureXML->FirstChildElement(ConstValues::ANIMATION.c_str()); 
		animationXML; animationXML = animationXML->NextSiblingElement(ConstValues::ANIMATION.c_str()))
    {
        AnimationData *animationData = parseAnimationData(animationXML, armatureData);
        armatureData->animationDataList.push_back(animationData);
    }
    
    return armatureData;
}

BoneData* XMLDataParser::parseBoneData(const XMLElement *boneXML) const
{
    BoneData *boneData = new BoneData();
    boneData->name = boneXML->Attribute(ConstValues::A_NAME.c_str());
	if (boneXML->FindAttribute(ConstValues::A_PARENT.c_str()))
	{
		boneData->parent = boneXML->Attribute(ConstValues::A_PARENT.c_str());    
	}
    // length目前没什么用，为以后做运行时反向动力学预留
	boneData->length = getNumber(*boneXML, ConstValues::A_LENGTH.c_str(), 0.f, 0.f);
	// 补充文档说明：骨骼是否受到父骨骼的缩放和旋转的影响
    boneData->inheritRotation = getBoolean(*boneXML, ConstValues::A_INHERIT_ROTATION.c_str(), true);
    boneData->inheritScale = getBoolean(*boneXML, ConstValues::A_INHERIT_SCALE.c_str(), true);

    const XMLElement *transformXML = boneXML->FirstChildElement(ConstValues::TRANSFORM.c_str());
    if (transformXML)
    {
        //parseTransform(*transformXML, boneData->global);
		parseTransform(*transformXML, boneData->transform);
    }

	// 4.0开始不再支持绝对值，这里的isGlobal只是为了匹配格式说明
	if (_isGlobalData)
	{
		boneData->transform = boneData->global;
	}
    
    return boneData;
}

SkinData* XMLDataParser::parseSkinData(const XMLElement *skinXML) const
{
    SkinData *skinData = new SkinData();
    skinData->name = skinXML->Attribute(ConstValues::A_NAME.c_str());
    
    for (const XMLElement *slotXML = skinXML->FirstChildElement(ConstValues::SLOT.c_str()); 
		slotXML; slotXML = slotXML->NextSiblingElement(ConstValues::SLOT.c_str()))
    {
        SlotData *slotData = parseSlotDisplayData(slotXML);
        skinData->slotDataList.push_back(slotData);
    }
    
    return skinData;
}

SlotData* XMLDataParser::parseSlotDisplayData(const XMLElement *slotXML) const
{
	SlotData *slotData = new SlotData();
	slotData->name = slotXML->Attribute(ConstValues::A_NAME.c_str());
	for (const XMLElement *displayXML = slotXML->FirstChildElement(ConstValues::DISPLAY.c_str()); 
		displayXML; displayXML = displayXML->NextSiblingElement(ConstValues::DISPLAY.c_str()))
	{
	    DisplayData *displayData = parseDisplayData(displayXML);
	    slotData->displayDataList.push_back(displayData);
	}
	return slotData;
}

SlotData* XMLDataParser::parseSlotData(const XMLElement *slotXML) const
{
    SlotData *slotData = new SlotData();
    slotData->name = slotXML->Attribute(ConstValues::A_NAME.c_str());
    slotData->parent = slotXML->Attribute(ConstValues::A_PARENT.c_str());
    slotData->zOrder = slotXML->FloatAttribute(ConstValues::A_Z_ORDER.c_str());
	slotData->displayIndex = slotXML->FloatAttribute(ConstValues::A_DISPLAY_INDEX.c_str());

	if (slotXML->FindAttribute(ConstValues::A_BLENDMODE.c_str()))
    {
        slotData->blendMode = getBlendModeByString(slotXML->Attribute(ConstValues::A_BLENDMODE.c_str()));
    }    
    return slotData;
}

DisplayData* XMLDataParser::parseDisplayData(const XMLElement *displayXML) const
{
    DisplayData *displayData = new DisplayData();
    displayData->name = displayXML->Attribute(ConstValues::A_NAME.c_str());
    displayData->type = getDisplayTypeByString(displayXML->Attribute(ConstValues::A_TYPE.c_str()));

    const XMLElement *transformXML = displayXML->FirstChildElement(ConstValues::TRANSFORM.c_str());
    if (transformXML)
    {
		// 4.x取消pivot轴动画的支持
        parseTransform(*transformXML, displayData->transform);
        //parsePivot(*transformXML, displayData->pivot);
    }

    return displayData;
}

AnimationData* XMLDataParser::parseAnimationData(const XMLElement *animationXML, const ArmatureData *armatureData) const
{
    AnimationData *animationData = new AnimationData();
    animationData->name = animationXML->Attribute(ConstValues::A_NAME.c_str());
    animationData->frameRate = _frameRate;
    animationData->duration = (int)(round(animationXML->IntAttribute(ConstValues::A_DURATION.c_str()) * 1000.f / _frameRate));
    animationData->playTimes = animationXML->IntAttribute(ConstValues::A_LOOP.c_str());
    animationData->fadeTime = animationXML->FloatAttribute(ConstValues::A_FADE_IN_TIME.c_str());
    animationData->scale = getNumber(*animationXML, ConstValues::A_SCALE.c_str(), 1.f, 1.f);
    // use frame tweenEase, NaN
    // overwrite frame tweenEase, [-1, 0):ease in, 0:line easing, (0, 1]:ease out, (1, 2]:ease in out
    animationData->tweenEasing = getNumber(*animationXML, ConstValues::A_TWEEN_EASING.c_str(), USE_FRAME_TWEEN_EASING, USE_FRAME_TWEEN_EASING);
    animationData->autoTween = getBoolean(*animationXML, ConstValues::A_AUTO_TWEEN.c_str(), true);
    
    for (const XMLElement *frameXML = animationXML->FirstChildElement(ConstValues::FRAME.c_str()); 
		frameXML; frameXML = frameXML->NextSiblingElement(ConstValues::FRAME.c_str()))
    {
        Frame *frame = parseMainFrame(frameXML);
        animationData->frameList.push_back(frame);
    }
    parseTimeline(*animationXML, *animationData);
    
	int lastFrameDuration = animationData->duration;
	
	// parse bone timeline
	for (const XMLElement *timelineXML = animationXML->FirstChildElement(ConstValues::BONE.c_str()); 
		timelineXML; timelineXML = timelineXML->NextSiblingElement(ConstValues::BONE.c_str()))
	{
		TransformTimeline *timeline = parseTransformTimeline(timelineXML, animationData->duration);
		if (timeline->frameList.size() > 0)
		{
			lastFrameDuration = std::min(lastFrameDuration, timeline->frameList[timeline->frameList.size() - 1]->duration);
			animationData->timelineList.push_back(timeline);
		}
	}

	// pase slot timeline
	for (const XMLElement *timelineXML = animationXML->FirstChildElement(ConstValues::SLOT.c_str()); 
		timelineXML; timelineXML = timelineXML->NextSiblingElement(ConstValues::SLOT.c_str()))
	{
		SlotTimeline *slotTimeline = parseSlotTimeline(timelineXML, animationData->duration);
		if (slotTimeline->frameList.size() > 0)
		{
			lastFrameDuration = std::min(lastFrameDuration, slotTimeline->frameList[slotTimeline->frameList.size() - 1]->duration);
			animationData->slotTimelineList.push_back(slotTimeline);
		}
	}

	if (animationData->frameList.size() > 0)
	{
		lastFrameDuration = std::min(lastFrameDuration, animationData->frameList[animationData->frameList.size() - 1]->duration);
	}
	animationData->lastFrameDuration = lastFrameDuration;

    addHideTimeline(animationData, armatureData);
    transformAnimationData(animationData, armatureData);
    return animationData;
}


TransformTimeline* XMLDataParser::parseTransformTimeline(const XMLElement *timelineXML, int duration) const
{
    TransformTimeline *timeline = new TransformTimeline();
    timeline->name = timelineXML->Attribute(ConstValues::A_NAME.c_str());
    timeline->scale = getNumber(*timelineXML, ConstValues::A_SCALE.c_str(), 1.f, 1.f);
    timeline->offset = timelineXML->FloatAttribute(ConstValues::A_OFFSET.c_str());
    timeline->duration = duration;
    
    for (const XMLElement *frameXML = timelineXML->FirstChildElement(ConstValues::FRAME.c_str()); 
		frameXML; frameXML = frameXML->NextSiblingElement(ConstValues::FRAME.c_str()))
    {
        TransformFrame *frame = parseTransformFrame(frameXML);
        timeline->frameList.push_back(frame);
    }
    parseTimeline(*timelineXML, *timeline);

    return timeline;
}

SlotTimeline* XMLDataParser::parseSlotTimeline(const XMLElement *timelineXML, int duration) const
{
	SlotTimeline *timeline = new SlotTimeline();
	timeline->name = timelineXML->Attribute(ConstValues::A_NAME.c_str());
	timeline->scale = getNumber(*timelineXML, ConstValues::A_SCALE.c_str(), 1.f, 1.f);
	timeline->offset = timelineXML->FloatAttribute(ConstValues::A_OFFSET.c_str());
	timeline->duration = duration;

	for (const XMLElement *frameXML = timelineXML->FirstChildElement(ConstValues::FRAME.c_str()); 
		frameXML; frameXML = frameXML->NextSiblingElement(ConstValues::FRAME.c_str()))
	{
		SlotFrame *frame = parseSlotFrame(frameXML);
		timeline->frameList.push_back(frame);
	}
	parseTimeline(*timelineXML, *timeline);

	return timeline;
}

SlotFrame* XMLDataParser::parseSlotFrame(const XMLElement *frameXML) const
{
	SlotFrame *frame = new SlotFrame();
	parseFrame(*frameXML, *frame);

	frame->visible = !getBoolean(*frameXML, ConstValues::A_HIDE.c_str(), false);
	frame->tweenEasing = getNumber(*frameXML, ConstValues::A_TWEEN_EASING.c_str(), AUTO_TWEEN_EASING, NO_TWEEN_EASING);
	frame->displayIndex = frameXML->IntAttribute(ConstValues::A_DISPLAY_INDEX.c_str());
	frame->zOrder = getNumber(*frameXML, ConstValues::A_Z_ORDER.c_str(), 0.f, 0.f);

	const XMLElement *colorTransformXML = frameXML->FirstChildElement(ConstValues::COLOR_TRANSFORM.c_str());
	if (colorTransformXML)
	{
		frame->color = new ColorTransform();
		parseColorTransform(*colorTransformXML, *frame->color);
	}

	return frame;
}

Frame* XMLDataParser::parseMainFrame(const XMLElement *frameXML) const
{
    Frame *frame = new Frame();
    parseFrame(*frameXML, *frame);
    return frame;
}

TransformFrame* XMLDataParser::parseTransformFrame(const XMLElement *frameXML) const
{
    TransformFrame *frame = new TransformFrame();
    parseFrame(*frameXML, *frame);

    frame->visible = !getBoolean(*frameXML, ConstValues::A_HIDE.c_str(), false);
    // NaN:no tween, 10:auto tween, [-1, 0):ease in, 0:line easing, (0, 1]:ease out, (1, 2]:ease in out
    frame->tweenEasing = getNumber(*frameXML, ConstValues::A_TWEEN_EASING.c_str(), AUTO_TWEEN_EASING, NO_TWEEN_EASING);
    frame->tweenRotate = frameXML->IntAttribute(ConstValues::A_TWEEN_ROTATE.c_str());
    frame->tweenScale = getBoolean(*frameXML, ConstValues::A_TWEEN_SCALE.c_str(), true);

    const XMLElement *transformXML = frameXML->FirstChildElement(ConstValues::TRANSFORM.c_str());
    if (transformXML)
    {
        parseTransform(*transformXML, frame->global);
        parsePivot(*transformXML, frame->pivot);
    }

    // copy
    frame->transform = frame->global;
    frame->scaleOffset.x = getNumber(*frameXML, ConstValues::A_SCALE_X_OFFSET.c_str(), 0.f, 0.f);
    frame->scaleOffset.y = getNumber(*frameXML, ConstValues::A_SCALE_Y_OFFSET.c_str(), 0.f, 0.f);
    
    return frame;
}

void XMLDataParser::parseTimeline(const XMLElement &timelineXML, Timeline &timeline) const
{
    int position = 0;
    Frame *frame = nullptr;
    
    for (size_t i = 0, l = timeline.frameList.size(); i < l; ++i)
    {
        frame = timeline.frameList[i];
        frame->position = position;
        position += frame->duration;
    }
    
    if (frame)
    {
        frame->duration = timeline.duration - frame->position;
    }
}

void XMLDataParser::parseFrame(const XMLElement &frameXML, Frame &frame) const
{
    frame.duration = (int)(round(frameXML.IntAttribute(ConstValues::A_DURATION.c_str()) * 1000.f / _frameRate));
    
    if (frameXML.FindAttribute(ConstValues::A_ACTION.c_str()))
    {
        frame.action = frameXML.Attribute(ConstValues::A_ACTION.c_str());
    }
    
    if (frameXML.FindAttribute(ConstValues::A_EVENT.c_str()))
    {
        frame.event = frameXML.Attribute(ConstValues::A_EVENT.c_str());
    }
    
    if (frameXML.FindAttribute(ConstValues::A_SOUND.c_str()))
    {
        frame.sound = frameXML.Attribute(ConstValues::A_SOUND.c_str());
    }

	// curve data
	if (frameXML.FindAttribute(ConstValues::A_CURVE.c_str()))
	{
		frame.curve = new CurveData();
		for (const XMLElement *curveXML = frameXML.FirstChildElement(ConstValues::A_CURVE.c_str()); 
			curveXML; curveXML = frameXML.NextSiblingElement(ConstValues::A_CURVE.c_str()))
		{
			float x = curveXML->FloatAttribute(ConstValues::A_X.c_str());
			float y = curveXML->FloatAttribute(ConstValues::A_Y.c_str());
			
			frame.curve->_pointList.push_back(new Point(x, y));
		}
	}
}

void XMLDataParser::parseTransform(const XMLElement &transformXML, Transform &transform) const
{
	// 0
    transform.x = transformXML.FloatAttribute(ConstValues::A_X.c_str()) / _armatureScale;
    transform.y = transformXML.FloatAttribute(ConstValues::A_Y.c_str()) / _armatureScale;
    transform.skewX = transformXML.FloatAttribute(ConstValues::A_SKEW_X.c_str()) * ANGLE_TO_RADIAN;
    transform.skewY = transformXML.FloatAttribute(ConstValues::A_SKEW_Y.c_str()) * ANGLE_TO_RADIAN;
	// 1
	transform.scaleX = getNumber(transformXML, ConstValues::A_SCALE_X.c_str(), 1.f, 1.f);
    transform.scaleY = getNumber(transformXML, ConstValues::A_SCALE_Y.c_str(), 1.f, 1.f);
}

void XMLDataParser::parsePivot(const XMLElement &transformXML, Point &pivot) const
{
    pivot.x = transformXML.FloatAttribute(ConstValues::A_PIVOT_X.c_str()) / _armatureScale;
    pivot.y = transformXML.FloatAttribute(ConstValues::A_PIVOT_Y.c_str()) / _armatureScale;
}

void XMLDataParser::parseColorTransform(const XMLElement &colorTransformXML, ColorTransform &colorTransform) const
{
    colorTransform.alphaOffset = colorTransformXML.IntAttribute(ConstValues::A_ALPHA_OFFSET.c_str());
    colorTransform.redOffset = colorTransformXML.IntAttribute(ConstValues::A_RED_OFFSET.c_str());
    colorTransform.greenOffset = colorTransformXML.IntAttribute(ConstValues::A_GREEN_OFFSET.c_str());
    colorTransform.blueOffset = colorTransformXML.IntAttribute(ConstValues::A_BLUE_OFFSET.c_str());
    colorTransform.alphaMultiplier = colorTransformXML.FloatAttribute(ConstValues::A_ALPHA_MULTIPLIER.c_str()) * 0.01f;
    colorTransform.redMultiplier = colorTransformXML.FloatAttribute(ConstValues::A_RED_MULTIPLIER.c_str()) * 0.01f;
    colorTransform.greenMultiplier = colorTransformXML.FloatAttribute(ConstValues::A_GREEN_MULTIPLIER.c_str()) * 0.01f;
    colorTransform.blueMultiplier = colorTransformXML.FloatAttribute(ConstValues::A_BLUE_MULTIPLIER.c_str()) * 0.01f;
}

NAME_SPACE_DRAGON_BONES_END
