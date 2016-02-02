#include "DBObject.h"
#include "Armature.h"
#include "core/Bone.h"

NAME_SPACE_DRAGON_BONES_BEGIN

DBObject::DBObject() :
	inheritRotation(true)
	,inheritScale(true)
	,inheritTranslation(true)
	,name("")
	,userData(nullptr)
	,_visible(true)
	,_armature(nullptr)
	,_parent(nullptr)
{
	offset.scaleX = offset.scaleY = 1.f;
}

DBObject::~DBObject()
{
	dispose();
}

void DBObject::dispose()
{
	_armature = nullptr;
	_parent = nullptr;

	if (userData)
	{
		delete userData;
		userData = nullptr;
	}
}

bool DBObject::getVisible() const
{
    return _visible;
}

void DBObject::setVisible(bool visible)
{
    _visible = visible;
}

Armature* DBObject::getArmature() const
{
    return _armature;
}

Bone* DBObject::getParent() const
{
    return _parent;
}

void DBObject::setArmature(Armature *armature)
{
    _armature = armature;
}

void DBObject::setParent(Bone *bone)
{
    _parent = bone;
}

void DBObject::calculateRelativeParentTransform()
{
}

void DBObject::calculateParentTransform( Transform &transform, Matrix &matrix )
{
	if (_parent && (inheritTranslation || inheritRotation || inheritScale))
	{
		transform = _parent->_globalTransformForChild;
		matrix = _parent->_globalTransformMatrixForChild;

		if (!inheritTranslation || !inheritRotation || !inheritScale)
		{
			if (!inheritTranslation)
			{
				transform.x = 0.f;
				transform.y = 0.f;
			}
			if (!inheritScale)
			{
				transform.scaleX = 1.f;
				transform.scaleY = 1.f;
			}
			if (!inheritRotation)
			{
				transform.skewX = 0.f;
				transform.skewY = 0.f;
			}
			transform.toMatrix(matrix, true);
		}
	}
}

void DBObject::updateGlobal( Transform &transform, Matrix &matrix )
{
	calculateRelativeParentTransform();
	
	// 计算parent bone的绝对坐标
	calculateParentTransform(transform, matrix);
	
	// 计算绝对坐标
	float x = global.x;
	float y = global.y;

	global.x = matrix.a * x + matrix.c * y + matrix.tx;
	global.y = matrix.d * y + matrix.b * x + matrix.ty;

	if (inheritRotation)
	{
		global.skewX += transform.skewX;
		global.skewY += transform.skewY;
	}
	if (inheritScale)
	{
		global.scaleX *= transform.scaleX;
		global.scaleY *= transform.scaleY;
	}
	global.toMatrix(globalTransformMatrix, true);
}


NAME_SPACE_DRAGON_BONES_END
