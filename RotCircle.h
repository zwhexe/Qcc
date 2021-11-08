#pragma once

#include "ShapeHandle.h"
#include <AIS_Shape.hxx>
#include <AIS_ColoredShape.hxx>

class RotCircle : public AIS_ColoredShape
{
public:
	RotCircle(TopoDS_Shape& topoShp);
	~RotCircle();

private:
	//for rotated three circles
};

