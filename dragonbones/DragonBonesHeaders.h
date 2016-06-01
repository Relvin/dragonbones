#ifndef DRAGONBONES_DRAGON_BONES_HEADERS_H
#define DRAGONBONES_DRAGON_BONES_HEADERS_H

#include "DragonBones.h"

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

// textures
#include "textures/TextureData.h"
#include "textures/TextureAtlasData.h"
#include "textures/ITextureAtlas.h"

// events
#include "events/DBEventData.h"

//renderer
#include "renderer/DBArmature.h"
#include "renderer/DBBone.h"
#include "renderer/DBSkin.h"
#include "renderer/DBSlot.h"
#include "renderer/DBCCFactory.h"
// animation

// core


// parsers
#include "parsers/BaseDataParser.h"
#include "parsers/XMLDataParser.h"
#include "parsers/JSONDataParser.h"
#include "parsers/BinaryParser.h"

// factories
#include "factories/BaseFactory.h"

#endif  // DRAGONBONES_DRAGON_BONES_HEADERS_H
