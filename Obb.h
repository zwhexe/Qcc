#pragma once

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>

#include <Bnd_OBB.hxx>
#include <BRepBndLib.hxx>
#include "QccView.h"

class Obb
{
public:
	explicit Obb(TopoDS_Shape);
	~Obb();

	void displayOBB(QccView*);

private:
	Bnd_OBB obbShape;
	Bnd_OBB obbFace;
	Bnd_OBB obbTri;
};

