#include "JSONDataParser.h"

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

namespace 
{
	std::string GET_STRING(const rapidjson::Value &value, const char* name, const char* defaulltValue = "")
	{
		if (value.HasMember(name))
			return value[name].GetString();
		else return defaulltValue;
	}

	int GET_INT(const rapidjson::Value &value, const char* name, int defaultValue = 0)
	{
		if (value.HasMember(name))
			return value[name].GetInt();
		else return defaultValue;
	}

	float GET_FLOAT(const rapidjson::Value &value, const char* name, float defaultValue = 0.f)
	{
		if (value.HasMember(name))
			return value[name].GetDouble();
		else return defaultValue;
	}

	bool GET_BOOL(const rapidjson::Value &value, const char* name, bool defaultValue = false)
	{
		if (value.HasMember(name))
			return value[name].GetBool();
		else return defaultValue;
	}
}

TextureAtlasData* JSONDataParser::parseTextureAtlasData(const char *rawTextureAtlasData)
{
	rapidjson::Document document;
	assert(!document.Parse(rawTextureAtlasData).HasParseError());
	assert(document.IsObject());

	TextureAtlasData *textureAtlasData = new TextureAtlasData();
	textureAtlasData->name = GET_STRING(document, ConstValues::A_NAME.c_str());
	textureAtlasData->imagePath = GET_STRING(document, ConstValues::A_IMAGE_PATH.c_str());
	textureAtlasData->format = getPixelFormatByString(GET_STRING(document, ConstValues::A_IMAGE_FORMAT.c_str()));

	const rapidjson::Value& subtextures = document[ConstValues::SUB_TEXTURE.c_str()];
	assert(subtextures.IsArray());
	for (rapidjson::SizeType i = 0; i < subtextures.Size(); i++)
	{
		TextureData *textureData = parseTextureData(subtextures[i]);
		textureAtlasData->textureDataList.push_back(textureData);
	}
    return textureAtlasData;
}

TextureData* JSONDataParser::parseTextureData(const rapidjson::Value &inTextures)
{
    TextureData *textureData = new TextureData();
	textureData->name = GET_STRING(inTextures, ConstValues::A_NAME.c_str());
	textureData->rotated = GET_BOOL(inTextures, ConstValues::A_ROTATED.c_str());
	textureData->region.x = GET_FLOAT(inTextures, ConstValues::A_X.c_str());
	textureData->region.y = GET_FLOAT(inTextures, ConstValues::A_Y.c_str());
	textureData->region.width = GET_FLOAT(inTextures, ConstValues::A_WIDTH.c_str());
	textureData->region.height = GET_FLOAT(inTextures, ConstValues::A_HEIGHT.c_str());
	const float frameWidth = GET_FLOAT(inTextures, ConstValues::A_FRAME_WIDTH.c_str());
	const float frameHeight = GET_FLOAT(inTextures, ConstValues::A_FRAME_HEIGHT.c_str());

	if (frameWidth > 0 && frameHeight > 0)
	{
		textureData->frame = new Rectangle();
		textureData->frame->x = GET_FLOAT(inTextures, ConstValues::A_FRAME_X.c_str());
		textureData->frame->y = GET_FLOAT(inTextures, ConstValues::A_FRAME_Y.c_str());
		textureData->frame->width = frameWidth;
		textureData->frame->height = frameHeight;
	}
    return textureData;
}

DragonBonesData* JSONDataParser::parseDragonBonesData(const char *rawDragonBonesData)
{
	rapidjson::Document document;
	assert(!document.Parse(rawDragonBonesData).HasParseError());
	assert(document.IsObject());

	std::string version = GET_STRING(document, ConstValues::A_VERSION.c_str());
    int frameRate = GET_INT(document, ConstValues::A_FRAME_RATE.c_str(), 30);

	DragonBonesData *dragonBonesData = new DragonBonesData();
    dragonBonesData->name = GET_STRING(document, ConstValues::A_NAME.c_str());
	dragonBonesData->isGlobalData = (GET_INT(document, ConstValues::A_IS_GLOBAL.c_str()) == 0) ? false : true;
    
	const rapidjson::Value& armatures = document[ConstValues::ARMATURE.c_str()];
	assert(armatures.IsArray());
	for (rapidjson::SizeType i = 0; i < armatures.Size(); i++)
	{
		ArmatureData *armatureData = parseArmatureData(armatures[i], frameRate);
		dragonBonesData->armatureDataList.push_back(armatureData);
	}
    
    return dragonBonesData;
}

ArmatureData* JSONDataParser::parseArmatureData(const rapidjson::Value &inArmatures, int frameRate)
{
    ArmatureData *armatureData = new ArmatureData();
    armatureData->name = GET_STRING(inArmatures, ConstValues::A_NAME.c_str());
    
	if (inArmatures.HasMember(ConstValues::BONE.c_str()))
	{
		const rapidjson::Value& bones = inArmatures[ConstValues::BONE.c_str()];
		assert(bones.IsArray());
		for (rapidjson::SizeType i = 0; i < bones.Size(); i++)
		{
			BoneData *boneData = parseBoneData(bones[i]);
			armatureData->boneDataList.push_back(boneData);
		}
	}

	if (inArmatures.HasMember(ConstValues::SLOT.c_str()))
	{
		const rapidjson::Value& slots = inArmatures[ConstValues::SLOT.c_str()];
		assert(slots.IsArray());
		for (rapidjson::SizeType i = 0; i < slots.Size(); i++)
		{
			SlotData *slotData = parseSlotData(slots[i]);
			armatureData->slotDataList.push_back(slotData);
		}
	}

	if (inArmatures.HasMember(ConstValues::SKIN.c_str()))
	{
		const rapidjson::Value& skins = inArmatures[ConstValues::SKIN.c_str()];
		assert(skins.IsArray());
		for (rapidjson::SizeType i = 0; i < skins.Size(); i++)
		{
			SkinData *skinData = parseSkinData(skins[i]);
			armatureData->skinDataList.push_back(skinData);
		}
	}

    armatureData->sortBoneDataList();
	if (inArmatures.HasMember(ConstValues::ANIMATION.c_str()))
	{
		const rapidjson::Value& animations = inArmatures[ConstValues::ANIMATION.c_str()];
		assert(animations.IsArray());
		for (rapidjson::SizeType i = 0; i < animations.Size(); i++)
		{
			AnimationData *animationData = parseAnimationData(animations[i], armatureData);
			armatureData->animationDataList.push_back(animationData);
		}
	}
    return armatureData;
}

BoneData* JSONDataParser::parseBoneData(const rapidjson::Value &inBones)
{
    BoneData *boneData = new BoneData();
    boneData->name = GET_STRING(inBones, ConstValues::A_NAME.c_str());
	boneData->parent = GET_STRING(inBones, ConstValues::A_PARENT.c_str());
    // length目前没什么用，为以后做运行时反向动力学预留
	boneData->length = GET_FLOAT(inBones, ConstValues::A_LENGTH.c_str(), 0.f);
	// 补充文档说明：骨骼是否受到父骨骼的缩放和旋转的影响
    boneData->inheritRotation = GET_BOOL(inBones, ConstValues::A_INHERIT_ROTATION.c_str(), true);
    boneData->inheritScale = GET_BOOL(inBones, ConstValues::A_INHERIT_SCALE.c_str(), true);

	if (inBones.HasMember(ConstValues::TRANSFORM.c_str()))
	{
		const rapidjson::Value& transforms = inBones[ConstValues::TRANSFORM.c_str()];
		if (!transforms.IsArray() && !transforms.IsNull())
		{
			parseTransform(transforms, boneData->transform);
		}
	}
    return boneData;
}

SkinData* JSONDataParser::parseSkinData(const rapidjson::Value &inSkins)
{
    SkinData *skinData = new SkinData();
    skinData->name = GET_STRING(inSkins, ConstValues::A_NAME.c_str());
	const rapidjson::Value& slots = inSkins[ConstValues::SLOT.c_str()];
	for (rapidjson::SizeType i = 0; i < slots.Size(); i++)
	{
		SlotData *slotData = parseSlotDisplayData(slots[i]);
		skinData->slotDataList.push_back(slotData);
	}
    return skinData;
}

SlotData* JSONDataParser::parseSlotDisplayData(const rapidjson::Value &inSlots)
{
	SlotData *slotData = new SlotData();
	slotData->name = GET_STRING(inSlots, ConstValues::A_NAME.c_str());
	const rapidjson::Value& displays = inSlots[ConstValues::DISPLAY.c_str()];
	for (rapidjson::SizeType i = 0; i < displays.Size(); i++)
	{
		DisplayData *displayData = parseDisplayData(displays[i]);
		slotData->displayDataList.push_back(displayData);
	}
	return slotData;
}

SlotData* JSONDataParser::parseSlotData(const rapidjson::Value &inSlots)
{
    SlotData *slotData = new SlotData();
    slotData->name = GET_STRING(inSlots, ConstValues::A_NAME.c_str());
    slotData->parent = GET_STRING(inSlots, ConstValues::A_PARENT.c_str());
    slotData->zOrder = GET_FLOAT(inSlots, ConstValues::A_Z_ORDER.c_str());
	slotData->displayIndex = GET_FLOAT(inSlots, ConstValues::A_DISPLAY_INDEX.c_str());
	slotData->blendMode = getBlendModeByString(GET_STRING(inSlots, ConstValues::A_BLENDMODE.c_str()));
    return slotData;
}

DisplayData* JSONDataParser::parseDisplayData(const rapidjson::Value &inDisplay)
{
    DisplayData *displayData = new DisplayData();
    displayData->name = GET_STRING(inDisplay, ConstValues::A_NAME.c_str());
    displayData->type = getDisplayTypeByString(GET_STRING(inDisplay, ConstValues::A_TYPE.c_str()));

	if (inDisplay.HasMember(ConstValues::TRANSFORM.c_str()))
	{
		const rapidjson::Value& transforms = inDisplay[ConstValues::TRANSFORM.c_str()];
		if (!transforms.IsArray() && !transforms.IsNull())
		{
			// 4.x取消pivot轴动画的支持
			parseTransform(transforms, displayData->transform);
		}
	}
    return displayData;
}

AnimationData* JSONDataParser::parseAnimationData(const rapidjson::Value &inAnimations, const ArmatureData *armatureData, int frameRate)
{
    AnimationData *animationData = new AnimationData();
    animationData->name = GET_STRING(inAnimations, ConstValues::A_NAME.c_str());
    animationData->frameRate = frameRate;
    animationData->duration = (int)(round(GET_INT(inAnimations, ConstValues::A_DURATION.c_str()) * 1000.f / frameRate));
    animationData->playTimes = GET_INT(inAnimations, ConstValues::A_LOOP.c_str());
    animationData->fadeTime = GET_FLOAT(inAnimations, ConstValues::A_FADE_IN_TIME.c_str());
    animationData->scale = GET_FLOAT(inAnimations, ConstValues::A_SCALE.c_str(), 1.f);
    // use frame tweenEase, NaN
    // overwrite frame tweenEase, [-1, 0):ease in, 0:line easing, (0, 1]:ease out, (1, 2]:ease in out
    //animationData->tweenEasing = GET_FLOAT(inAnimations, ConstValues::A_TWEEN_EASING.c_str(), USE_FRAME_TWEEN_EASING);
	if (inAnimations.HasMember(ConstValues::A_TWEEN_EASING.c_str()))
	{
		if (inAnimations[ConstValues::A_TWEEN_EASING.c_str()].IsNull())
		{
			animationData->tweenEasing = NO_TWEEN_EASING;
		}
		else
		{
			animationData->tweenEasing = inAnimations[ConstValues::A_TWEEN_EASING.c_str()].GetDouble();
		}
	}
    animationData->autoTween = (GET_INT(inAnimations, ConstValues::A_AUTO_TWEEN.c_str()) == 1) ? true : false;
    
	if (inAnimations.HasMember(ConstValues::FRAME.c_str()))
	{
		const rapidjson::Value& frames = inAnimations[ConstValues::FRAME.c_str()];
		for (rapidjson::SizeType i = 0; i < frames.Size(); i++)
		{
			Frame *frame = parseMainFrame(frames[i], frameRate);
			animationData->frameList.push_back(frame);
		}
	}
	parseTimeline(*animationData);

	int lastFrameDuration = animationData->duration;
	
	// parse bone timeline
	if (inAnimations.HasMember(ConstValues::BONE.c_str()))
	{
		const rapidjson::Value& bones = inAnimations[ConstValues::BONE.c_str()];
		for (rapidjson::SizeType i = 0; i < bones.Size(); i++)
		{
			TransformTimeline *timeline = parseTransformTimeline(bones[i], animationData->duration);
			if (timeline->frameList.size() > 0)
			{
				lastFrameDuration = std::min(lastFrameDuration, timeline->frameList[timeline->frameList.size() - 1]->duration);
				animationData->timelineList.push_back(timeline);
			}
		}
	}

	// pase slot timeline
	if (inAnimations.HasMember(ConstValues::SLOT.c_str()))
	{
		const rapidjson::Value& slots = inAnimations[ConstValues::SLOT.c_str()];
		for (rapidjson::SizeType i = 0; i < slots.Size(); i++)
		{
			SlotTimeline *slotTimeline = parseSlotTimeline(slots[i], animationData->duration);
			if (slotTimeline->frameList.size() > 0)
			{
				lastFrameDuration = std::min(lastFrameDuration, slotTimeline->frameList[slotTimeline->frameList.size() - 1]->duration);
				animationData->slotTimelineList.push_back(slotTimeline);
			}
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


TransformTimeline* JSONDataParser::parseTransformTimeline(const rapidjson::Value &inTimeline, int duration)
{
    TransformTimeline *timeline = new TransformTimeline();
    timeline->name = GET_STRING(inTimeline, ConstValues::A_NAME.c_str());
    timeline->scale = GET_FLOAT(inTimeline, ConstValues::A_SCALE.c_str(), 1.f);
    timeline->offset = GET_FLOAT(inTimeline, ConstValues::A_OFFSET.c_str());
    timeline->duration = duration;
    
	const rapidjson::Value& frames = inTimeline[ConstValues::FRAME.c_str()];
	for (rapidjson::SizeType i = 0; i < frames.Size(); i++)
	{
		TransformFrame *frame = parseTransformFrame(frames[i]);
		timeline->frameList.push_back(frame);
	}
	parseTimeline(*timeline);

    return timeline;
}

SlotTimeline* JSONDataParser::parseSlotTimeline(const rapidjson::Value &inFrame, int duration)
{
	SlotTimeline *timeline = new SlotTimeline();
	timeline->name = GET_STRING(inFrame, ConstValues::A_NAME.c_str());
	timeline->scale = GET_FLOAT(inFrame, ConstValues::A_SCALE.c_str(), 1.f);
	timeline->offset = GET_FLOAT(inFrame, ConstValues::A_OFFSET.c_str());
	timeline->duration = duration;

	const rapidjson::Value& frames = inFrame[ConstValues::FRAME.c_str()];
	for (rapidjson::SizeType i = 0; i < frames.Size(); i++)
	{
		SlotFrame *frame = parseSlotFrame(frames[i]);
		timeline->frameList.push_back(frame);
	}
	parseTimeline(*timeline);
	return timeline;
}

SlotFrame* JSONDataParser::parseSlotFrame(const rapidjson::Value &inTimeline)
{
	SlotFrame *frame = new SlotFrame();
	parseFrame(inTimeline, *frame);

	frame->visible = !GET_BOOL(inTimeline, ConstValues::A_HIDE.c_str(), false);
	//frame->tweenEasing = GET_FLOAT(inTimeline, ConstValues::A_TWEEN_EASING.c_str(), AUTO_TWEEN_EASING);
	if (inTimeline.HasMember(ConstValues::A_TWEEN_EASING.c_str()))
	{
		if (inTimeline[ConstValues::A_TWEEN_EASING.c_str()].IsNull())
		{
			frame->tweenEasing = NO_TWEEN_EASING;
		}
		else
		{
			frame->tweenEasing = inTimeline[ConstValues::A_TWEEN_EASING.c_str()].GetDouble();
		}
	}

	frame->displayIndex = GET_INT(inTimeline, ConstValues::A_DISPLAY_INDEX.c_str());
	frame->zOrder = GET_FLOAT(inTimeline, ConstValues::A_Z_ORDER.c_str(), 0.f);

	if (inTimeline.HasMember(ConstValues::COLOR_TRANSFORM.c_str()))
	{
		const rapidjson::Value& colorTransform = inTimeline[ConstValues::COLOR_TRANSFORM.c_str()];
		if (!colorTransform.IsArray() && !colorTransform.IsNull())
		{
			frame->color = new ColorTransform();
			parseColorTransform(colorTransform, *frame->color);
		}
	}
	return frame;
}

Frame* JSONDataParser::parseMainFrame(const rapidjson::Value &inFrame, int frameRate)
{
    Frame *frame = new Frame();
    parseFrame(inFrame, *frame, frameRate);
    return frame;
}

TransformFrame* JSONDataParser::parseTransformFrame(const rapidjson::Value &inFrame)
{
    TransformFrame *frame = new TransformFrame();
    parseFrame(inFrame, *frame);

    frame->visible = !GET_BOOL(inFrame, ConstValues::A_HIDE.c_str(), false);
    // NaN:no tween, 10:auto tween, [-1, 0):ease in, 0:line easing, (0, 1]:ease out, (1, 2]:ease in out
    //frame->tweenEasing = GET_FLOAT(inFrame, , AUTO_TWEEN_EASING);
	if (inFrame.HasMember(ConstValues::A_TWEEN_EASING.c_str()))
	{
		if (inFrame[ConstValues::A_TWEEN_EASING.c_str()].IsNull())
		{
			frame->tweenEasing = NO_TWEEN_EASING;
		}
		else
		{
			frame->tweenEasing = inFrame[ConstValues::A_TWEEN_EASING.c_str()].GetDouble();
		}
	}

    frame->tweenRotate = GET_INT(inFrame, ConstValues::A_TWEEN_ROTATE.c_str());
    frame->tweenScale = GET_BOOL(inFrame, ConstValues::A_TWEEN_SCALE.c_str(), true);

	if (inFrame.HasMember(ConstValues::TRANSFORM.c_str()))
	{
		const rapidjson::Value& transform = inFrame[ConstValues::TRANSFORM.c_str()];
		if (!transform.IsArray() && !transform.IsNull())
		{
			parseTransform(transform, frame->global);
			parsePivot(transform, frame->pivot);
		}
	}

	// copy
    frame->transform = frame->global;
    frame->scaleOffset.x = GET_FLOAT(inFrame, ConstValues::A_SCALE_X_OFFSET.c_str(), 0.f);
    frame->scaleOffset.y = GET_FLOAT(inFrame, ConstValues::A_SCALE_Y_OFFSET.c_str(), 0.f);
    
    return frame;
}

void JSONDataParser::parseTimeline(Timeline &timeline)
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

void JSONDataParser::parseFrame(const rapidjson::Value &inFrame, Frame &frame, int frameRate)
{
    frame.duration = (int)(round(GET_INT(inFrame, ConstValues::A_DURATION.c_str()) * 1000.f / frameRate));
	frame.action = GET_STRING(inFrame, ConstValues::A_ACTION.c_str());
	frame.event = GET_STRING(inFrame, ConstValues::A_EVENT.c_str());
    frame.sound = GET_STRING(inFrame, ConstValues::A_SOUND.c_str());
	// curve data
	if (inFrame.HasMember(ConstValues::A_CURVE.c_str()))
	{
		frame.curve = new CurveData();
		const rapidjson::Value& curves = inFrame[ConstValues::A_CURVE.c_str()];
		for (rapidjson::SizeType i = 0; i < curves.Size(); i++)
		{
			float x = GET_FLOAT(curves, ConstValues::A_X.c_str());
			float y = GET_FLOAT(curves, ConstValues::A_Y.c_str());
			frame.curve->_pointList.push_back(new Point(x, y));
		}
	}	
}

void JSONDataParser::parseTransform(const rapidjson::Value &inTransform, Transform &transform)
{
	// 0
    transform.x = GET_FLOAT(inTransform, ConstValues::A_X.c_str());
    transform.y = GET_FLOAT(inTransform, ConstValues::A_Y.c_str());
    transform.skewX = GET_FLOAT(inTransform, ConstValues::A_SKEW_X.c_str()) * ANGLE_TO_RADIAN;
    transform.skewY = GET_FLOAT(inTransform, ConstValues::A_SKEW_Y.c_str()) * ANGLE_TO_RADIAN;
	// 1
	transform.scaleX = GET_FLOAT(inTransform, ConstValues::A_SCALE_X.c_str(), 1.f);
    transform.scaleY = GET_FLOAT(inTransform, ConstValues::A_SCALE_Y.c_str(), 1.f);
}

void JSONDataParser::parsePivot(const rapidjson::Value &inTransform, Point &pivot)
{
    pivot.x = GET_FLOAT(inTransform, ConstValues::A_PIVOT_X.c_str());
    pivot.y = GET_FLOAT(inTransform, ConstValues::A_PIVOT_Y.c_str());
}

void JSONDataParser::parseColorTransform(const rapidjson::Value &inColorTransform, ColorTransform &colorTransform)
{
    colorTransform.alphaOffset = GET_INT(inColorTransform, ConstValues::A_ALPHA_OFFSET.c_str());
    colorTransform.redOffset = GET_INT(inColorTransform, ConstValues::A_RED_OFFSET.c_str());
    colorTransform.greenOffset = GET_INT(inColorTransform, ConstValues::A_GREEN_OFFSET.c_str());
    colorTransform.blueOffset = GET_INT(inColorTransform, ConstValues::A_BLUE_OFFSET.c_str());
    colorTransform.alphaMultiplier = GET_FLOAT(inColorTransform, ConstValues::A_ALPHA_MULTIPLIER.c_str()) * 0.01f;
    colorTransform.redMultiplier = GET_FLOAT(inColorTransform, ConstValues::A_RED_MULTIPLIER.c_str()) * 0.01f;
    colorTransform.greenMultiplier = GET_FLOAT(inColorTransform, ConstValues::A_GREEN_MULTIPLIER.c_str()) * 0.01f;
    colorTransform.blueMultiplier = GET_FLOAT(inColorTransform, ConstValues::A_BLUE_MULTIPLIER.c_str()) * 0.01f;
}
NAME_SPACE_DRAGON_BONES_END
