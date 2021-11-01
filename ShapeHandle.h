#pragma once

#include <QDebug>
#include <vector>

#include <TopoDS_Shape.hxx>
#include <Bnd_OBB.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>

#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

namespace Hand
{
    TopoDS_Shape getBndShape(const Bnd_OBB&);
    TopoDS_Shape TriangleGetShape(std::vector<gp_Pnt>& triPoints);
    Bnd_OBB transformOBB(Bnd_OBB&, gp_Trsf&);
    void transformTriPnts(std::vector<gp_Pnt>& triPnt1, std::vector<gp_Pnt>& triPnt2, gp_Trsf& trsf);
}

static TopoDS_Shape Hand::getBndShape(const Bnd_OBB& bndBox) 
{
	Bnd_OBB tObb = bndBox;
	tObb.Enlarge(0.1);

	//Bnd_OBB vertex
	gp_Pnt vertex[8];
    tObb.GetVertex(vertex);
    BRepBuilderAPI_MakeWire aWireMaker;

    TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(vertex[0], vertex[1]);
    aWireMaker.Add(anEdge);
    TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(vertex[1], vertex[3]);
    aWireMaker.Add(anEdge2);
    TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(vertex[3], vertex[2]);
    aWireMaker.Add(anEdge3);
    TopoDS_Edge anEdge4 = BRepBuilderAPI_MakeEdge(vertex[2], vertex[0]);
    aWireMaker.Add(anEdge4);

    gp_Vec gpVec = gp_Vec(tObb.ZDirection());
    gpVec = gpVec * tObb.ZHSize() * 2;

    TopoDS_Wire aWire = aWireMaker.Wire();
    TopoDS_Face aFaceRect = BRepBuilderAPI_MakeFace(aWire);
    TopoDS_Shape BOX = BRepPrimAPI_MakePrism(aFaceRect, gpVec);
    return BOX;

}

static Bnd_OBB Hand::transformOBB(Bnd_OBB& bndObb, gp_Trsf& trsf)
{
    gp_Ax3 ax3(bndObb.Center(), bndObb.ZDirection(), bndObb.XDirection());
    ax3.Transform(trsf);

    Bnd_OBB bndret;
    bndret.SetXComponent(ax3.XDirection(), bndObb.XHSize());
    bndret.SetYComponent(ax3.YDirection(), bndObb.YHSize());
    bndret.SetZComponent(ax3.Direction(), bndObb.ZHSize());

    bndret.SetCenter(ax3.Location());

    return bndret;
}

static void Hand::transformTriPnts(std::vector<gp_Pnt>& triPnt1, std::vector<gp_Pnt>& triPnt2, gp_Trsf& trsf)
{
    triPnt2.clear();
    for (int i = 0; i < triPnt1.size(); i++)
    {
        triPnt2.push_back(triPnt2[i].Transformed(trsf));
    }
}

static TopoDS_Shape Hand::TriangleGetShape(std::vector<gp_Pnt>& triPoints)
{
    TopoDS_Edge e1 = BRepBuilderAPI_MakeEdge(triPoints[0], triPoints[1]);
    TopoDS_Edge e2 = BRepBuilderAPI_MakeEdge(triPoints[1], triPoints[2]);
    TopoDS_Edge e3 = BRepBuilderAPI_MakeEdge(triPoints[2], triPoints[0]);

    TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(e1, e2, e3);
    TopoDS_Face Face = BRepBuilderAPI_MakeFace(aWire);
    return Face;
}
