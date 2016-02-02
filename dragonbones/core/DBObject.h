#ifndef DRAGONBONES_CORE_OBJECT_H
#define DRAGONBONES_CORE_OBJECT_H

#include "DragonBones.h"
#include "geoms/Matrix.h"
#include "geoms/Transform.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class Armature;
class Bone;

class DBObject
{
public:
	DBObject();
	virtual ~DBObject();
	virtual void dispose();

	virtual bool getVisible() const;
	virtual void setVisible(bool vislble);

	virtual Armature* getArmature() const;
	virtual Bone* getParentBone() const;

protected:
	virtual void setArmature(Armature *armature);
	virtual void setParentBone(Bone *bone);
	// change
	virtual void calculateRelativeParentTransform();
	virtual void calculateParentTransform(Transform &transform, Matrix &matrix);
	virtual void updateGlobal(Transform &transform, Matrix &matrix);

public:
    bool inheritRotation;
    bool inheritScale;
	bool inheritTranslation;
    
    std::string name;
    Transform global;
    Transform origin;
    Transform offset;
	Matrix globalTransformMatrix;

    void *userData;
    


protected:
    bool _visible;

    Armature *_armature;
    Bone *_parentBone;
    

private:
    DRAGON_BONES_DISALLOW_COPY_AND_ASSIGN(DBObject);
};

NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_CORE_OBJECT_H
