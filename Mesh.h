#pragma once

#include "Qcc.h"
#include "QccView.h"
#include <IMeshTools_Parameters.hxx>
#include <IMeshTools_Context.hxx>

#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_Context.hxx>
#include <BRepMesh_Delaun.hxx>

#include <gp_Pnt.hxx>
#include <vector>
#include <thread>

class Mesh
{
public:
	explicit Mesh(TopoDS_Shape topoShp);
	~Mesh();

	void setMeshParam();
	void setMeshContext();
	void makeTriangle(bool isCustom = false);
	void displayTriangle();
	int countTriangle();

private:
	TopoDS_Shape meshShape;
	vector<vector<gp_Pnt>> triPnts;
	BRepMesh_IncrementalMesh mesher;
	IMeshTools_Parameters meshParam;
	Handle(IMeshTools_Context) meshContext;
};
