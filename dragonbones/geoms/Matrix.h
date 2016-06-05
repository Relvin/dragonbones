#ifndef DRAGONBONES_GEOMS_MATRIX_H
#define DRAGONBONES_GEOMS_MATRIX_H

#include "DragonBones.h"
#include "Point.h"

NAME_SPACE_DRAGON_BONES_BEGIN

// 平移、缩放、旋转和倾斜

/*
	a  b  tx
	c  d  ty
	0  0  1

	translate(tx, ty)
	1 0 tx
	0 1 ty
	0 0 1

	scale(a, d):
	sx 0  0
	0  sy 0
	0  0  1
	
	rotate(q):
	cos(q)  sin(q) 0
	-sin(q) cos(q) 0
	0		0	   1

	skew(skewx, skewy)
	0			tan(skewy)  0
	tan(skewx)  0		    0
	0			0			1

*/
class Matrix
{
public:
    Matrix() : 
		a(0.f)
		,b(0.f)
		,c(0.f)
		,d(0.f)
		,tx(0.f)
		,ty(0.f)
    {}
    Matrix(const Matrix &copyData)
    {
        operator=(copyData);
    }
    void operator=(const Matrix &copyData)
    {
        a = copyData.a;
        b = copyData.b;
        c = copyData.c;
        d = copyData.d;
        tx = copyData.tx;
        ty = copyData.ty;
    }
    void concat(const Matrix &mat)
    {
        const auto aA = a;
        const auto bA = b;
        const auto cA = c;
        const auto dA = d;
        const auto txA = tx;
        const auto tyA = ty;
        const auto aB = mat.a;
        const auto bB = mat.b;
        const auto cB = mat.c;
        const auto dB = mat.d;
        const auto txB = mat.tx;
        const auto tyB = mat.ty;
        
        a = aA * aB + bA * cB;
        b = aA * bB + bA * dB;
        c = cA * aB + dA * cB;
        d = cA * bB + dA * dB;
        tx = aB * txA + cB * tyA + txB;
        ty = dB * tyA + bB * txA + tyB;
    }
    virtual ~Matrix() {}
    
    inline void invert()
    {
        const auto aA = a;
        const auto bA = b;
        const auto cA = c;
        const auto dA = d;
        const auto txA = tx;
        const auto tyA = ty;
        const auto n = aA * dA - bA * cA;
        
        a = dA / n;
        b = -bA / n;
        c = -cA / n;
        d = aA / n;
        tx = (cA * tyA - dA * txA) / n;
        ty = -(aA * tyA - bA * txA) / n;
    }
    
    inline void transformPoint(Point &point)
    {
        float x = point.x;
        float y = point.y;
        point.x = a * x + c * y + tx;
        point.y = d * y + b * x + ty;
    }
    
    inline void transformPoint(Point& result, float x, float y,bool delta = false) const
    {
        result.x = a * x + c * y;
        result.y = b * x + d * y;
        
        if (!delta)
        {
            result.x += tx;
            result.y += ty;
        }
    }

public:
	float a;
	float b;
	float c;
	float d;
	float tx;
	float ty;
};
NAME_SPACE_DRAGON_BONES_END
#endif  // DRAGONBONES_GEOMS_MATRIX_H
