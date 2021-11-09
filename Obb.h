#pragma once

#include "Qcc.h"
#include "QccView.h"
#include <vector>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <AIS_Shape.hxx>

#include <Bnd_OBB.hxx>
#include <BRepBndLib.hxx>

using std::vector;

enum class ObbLevel
{
	ObbShape,
	ObbFace,
	ObbTriangle
};

class Obb
{
public:
	explicit Obb(TopoDS_Shape);
	~Obb();

	void displayObb(const QccView* myQccView, ObbLevel obblv = ObbLevel::ObbTriangle);
	double getArea(void);
	Standard_Boolean isValid(void);

public:
	TopoDS_Shape topoShape;
	Bnd_OBB obbShape;	      //topoShape Bnd_OBB
	vector<Bnd_OBB> obbList;  //topoShape faces Bnd_OBB
	vector<vector<TopoDS_Face>> triList;  //topoShape faces triangle list
};

