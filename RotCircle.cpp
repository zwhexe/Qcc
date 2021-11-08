#include "RotCircle.h"
#include <Bnd_OBB.hxx>

RotCircle::RotCircle(TopoDS_Shape& topoShp) : AIS_ColoredShape(topoShp)
{
	Handle(AIS_Shape) aisShp = new AIS_Shape(topoShp);
	Bnd_OBB bbox = Hand::getFullAABB(aisShp);

}

RotCircle::~RotCircle()
{

}
