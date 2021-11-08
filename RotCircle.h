#pragma once

#include <AIS_ColoredShape.hxx>

class RotCircle : public AIS_ColoredShape
{
public:
	RotCircle(TopoDS_Shape& topoShp);
	~RotCircle();

private:
	//for rotated three circles
};

