#pragma once

#include <vector>
#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>

#include <Bnd_OBB.hxx>
#include <BRepBndLib.hxx>
#include "QccView.h"

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

	void displayOBB(QccView* myQccView, ObbLevel obblv = ObbLevel::ObbFace);
	void isValid();

private:
	Bnd_OBB obbShape;
	vector<Bnd_OBB> obbFace;
	vector<vector<TopoDS_Face>> triFace;
};

