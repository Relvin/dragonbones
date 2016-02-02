#ifndef DRAGONBONES_OBJECTS_CURVEDATA_H
#define DRAGONBONES_OBJECTS_CURVEDATA_H

#include "DragonBones.h"

NAME_SPACE_DRAGON_BONES_BEGIN

class Point;

const int SamplingTimes = 20;
const float SamplingStep = 0.05f;

class CurveData
{
public:
	CurveData() : _dataChanged(false)
	{}
	virtual ~CurveData()
	{
	}

	float getValueByProgress(float progress)
	{
		if(_dataChanged)
		{
			refreshSampling();
		}

		Point *point = nullptr;
		for (int i = 0; i < SamplingTimes-1; i++) 
		{
			point= sampling[i];
			if (point->x >= progress) 
			{
				if(i == 0)
				{
					return point->y * progress / point->x;
				}
				else
				{
					Point *prevPoint = sampling[i-1];
					return prevPoint->y + (point->y - prevPoint->y) * (progress - prevPoint->x) / (point->x - prevPoint->x);
				}
			}
		}
		return point->y + (1 - point->y) * (progress - point->x) / (1 - point->x);
	}

	void refreshSampling()
	{
		for(int i = 0; i < SamplingTimes-1; i++)
		{
			bezierCurve(SamplingStep * (i+1), sampling[i]);
		}
		_dataChanged = false;
	}

	void bezierCurve(float t, Point *outputPoint)
	{
		float l_t = 1-t;
		outputPoint->x = 3* _pointList[0]->x*t*l_t*l_t + 3*_pointList[1]->x*t*t*l_t + pow(t,3);
		outputPoint->y = 3* _pointList[0]->y*t*l_t*l_t + 3*_pointList[1]->y*t*t*l_t + pow(t,3);
	}

	bool isCurve()
	{
		return _pointList[0]->x != 0 || _pointList[0]->y != 0 || _pointList[1]->x != 1 || _pointList[1]->y != 1;
	}

public:
	std::vector<Point*> _pointList;

private:
	bool _dataChanged;
	std::vector<Point*> sampling;
};

NAME_SPACE_DRAGON_BONES_END

#endif DRAGONBONES_OBJECTS_CURVEDATA_H
