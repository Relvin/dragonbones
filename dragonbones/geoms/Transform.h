#ifndef DRAGONBONES_GEOMS_TRANSFORM_H
#define DRAGONBONES_GEOMS_TRANSFORM_H

#include "DragonBones.h"
#include "Matrix.h"

NAME_SPACE_DRAGON_BONES_BEGIN
class Transform
{
public:

	//这个算法如果用于骨骼间的绝对转相对请改为DBTransform.divParent()方法
	static void globalToLocal(Transform &transform, Transform &parent)
	{
		Matrix helpTransformMatrix;
		Matrix helpParentTransformMatrix;
		transform.toMatrix(helpTransformMatrix, true);
		parent.toMatrix(helpParentTransformMatrix, true);

		helpParentTransformMatrix.invert();

		matrixToTransform(helpTransformMatrix, transform, transform.scaleX * parent.scaleX >= 0, transform.scaleY * parent.scaleY >= 0);
	}

	static void matrixToTransform(const Matrix &matrix, Transform &transform, bool scaleXF, bool scaleYF)
	{
		transform.x = matrix.tx;
		transform.y = matrix.ty;
		transform.scaleX = sqrt(matrix.a * matrix.a + matrix.b * matrix.b) * (scaleXF ? 1 : -1);
		transform.scaleY = sqrt(matrix.d * matrix.d + matrix.c * matrix.c) * (scaleYF ? 1 : -1);

		float skew0 = acos(matrix.d / transform.scaleY);
		float skew1 = -skew0;
		float skew2 = asin(-matrix.c / transform.scaleY);
		float skew3 = skew2 >= 0 ? PI - skew2 : skew2 - PI;

		if(skew0 == skew2 || skew0 == skew3)
		{
			transform.skewX = skew0;
		}
		else 
		{
			transform.skewX = skew1;
		}

		float skewY0 = acos(matrix.a / transform.scaleX);
		float skewY1 = -skewY0;
		float skewY2 = asin(matrix.b / transform.scaleX);
		float skewY3 = skewY2 >= 0 ? PI - skewY2 : skewY2 - PI;

		if(skewY0 == skewY2 || skewY0 == skewY3)
		{
			transform.skewY = skewY0;
		}
		else 
		{
			transform.skewY = skewY1;
		}
	}
    
public:
    Transform() :
		x(0.f)
		,y(0.f)
		,skewX(0.f)
		,skewY(0.f)
		,scaleX(1.f)
		,scaleY(1.f)
    {}
    Transform(const Transform &copyData)
    {
        operator=(copyData);
    }
    Transform& operator=(const Transform &copyData)
    {
        x = copyData.x;
        y = copyData.y;
        skewX = copyData.skewX;
        skewY = copyData.skewY;
        scaleX = copyData.scaleX;
        scaleY = copyData.scaleY;
        return *this;
    }
    virtual ~Transform() {}
    
    void toMatrix(Matrix &matrix, bool keepScale = false) const
    {
        if (keepScale)
        {
            matrix.a = scaleX * cos(skewY);
            matrix.b = scaleX * sin(skewY);
            matrix.c = -scaleY * sin(skewX);
            matrix.d = scaleY * cos(skewX);
        }
        else
        {
            matrix.a = cos(skewY);
            matrix.b = sin(skewY);
            matrix.c = -sin(skewX);
            matrix.d = cos(skewX);
        }
        
        matrix.tx = x;
        matrix.ty = y;
    }
    
    void transformWith(const Transform &parent)
    {
        Matrix matrix;
        parent.toMatrix(matrix, true);
        matrix.invert();
        const float x0 = x;
        const float y0 = y;
        x = matrix.a * x0 + matrix.c * y0 + matrix.tx;
        y = matrix.d * y0 + matrix.b * x0 + matrix.ty;
        skewX = formatRadian(skewX - parent.skewX);
        skewY = formatRadian(skewY - parent.skewY);
    }

	void normalizeRotation()
	{
		skewX = _normalizeRotation(skewX);
		skewY = _normalizeRotation(skewY);
	}

	float getRotation()
	{
		return skewX;
	}
	void setRotation(float value)
	{
		skewX = skewY = value;
	}

	void divParent(const Transform &transform, bool createNew = false)
	{
		//var output:DBTransform = createNew ? new DBTransform() : this;
		//var parentMatrix:Matrix = new Matrix();

		//TransformUtil.transformToMatrix(transform, parentMatrix);
		//var xtx:Number = x - parentMatrix.tx;
		//var yty:Number = y - parentMatrix.ty;
		//var adcb:Number = parentMatrix.a * parentMatrix.d - parentMatrix.c * parentMatrix.b;

		//output.x = (xtx * parentMatrix.d - yty * parentMatrix.c)/adcb;
		//output.y = (yty * parentMatrix.a - xtx * parentMatrix.b)/adcb;
		//output.scaleX = scaleX / transform.scaleX;
		//output.scaleY = scaleY / transform.scaleY;
		//output.skewX = skewX - transform.skewX;
		//output.skewY = skewY - transform.skewY;
		//return output;
	}

	void minus(const Transform &transform)
	{

	}

	void add(const Transform &transform)
	{

	}

private:
	// -180 ~ 180
	float _normalizeRotation(float rotation)
	{
        rotation = fmod(rotation + PI,2 * PI);// (float)((int)(rotation + PI) % (int)(2 * PI));
		rotation = rotation > 0 ? rotation : 2 * PI + rotation;
		return rotation - PI;
	}

public:
	float x;
	float y;
	float skewX;
	float skewY;
	float scaleX;
	float scaleY;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_GEOMS_TRANSFORM_H
