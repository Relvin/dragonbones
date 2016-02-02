#ifndef DRAGONBONES_OBJECTS_BONE_DATA_H
#define DRAGONBONES_OBJECTS_BONE_DATA_H

#include "DragonBones.h"
#include "geoms/Transform.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class BoneData
{    
public:
    BoneData() :
		inheritScale(false)
		,inheritRotation(false)
		,length(0.f)
		,name("")
		,parent("")
    {}
    BoneData(const BoneData &copyData)
    {
        operator=(copyData);
    }
    BoneData& operator=(const BoneData &copyData)
    {
        dispose();
        inheritScale = copyData.inheritScale;
        inheritRotation = copyData.inheritRotation;
        length = copyData.length;
        name = copyData.name;
        parent = copyData.parent;
        global = copyData.global;
        transform = copyData.transform;
        
        return *this;
    }
    virtual ~BoneData()
    {
        dispose();
    }
    void dispose()
    {
    }

public:
	bool inheritScale;
	bool inheritRotation;
	float length;

	std::string name;
	std::string parent;
	Transform global;
	Transform transform;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_OBJECTS_BONE_DATA_H
