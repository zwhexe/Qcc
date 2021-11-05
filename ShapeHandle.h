#pragma once

#include <QDebug>
#include <algorithm>
#include <vector>
#include <cmath>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopExp_Explorer.hxx>
#include <GProp_GProps.hxx>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepPrimAPI_MakePrism.hxx>

#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_FaceDiscret.hxx>
#include <BRepMesh_Context.hxx>

#include <BRepGProp.hxx>
#include <Bnd_OBB.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

namespace Hand
{
    double getFaceArea(const TopoDS_Shape& face);
    double getBndArea(const Bnd_OBB& bndObb, double enlarge = 0.001);
    TopoDS_Shape getBndShape(const Bnd_OBB&);
    TopoDS_Shape TriangleGetShape(std::vector<gp_Pnt>& triPoints);
    Bnd_OBB transformOBB(Bnd_OBB&, gp_Trsf&);
    Bnd_OBB getBoxObb(TopoDS_Shape, double);

    vector<TopoDS_Face> geneFaceTri(TopoDS_Face& topoFace);
    bool isObbCollideTri(Bnd_OBB& bndObb, TopoDS_Face& triFace);
    void displayTriangle(const QccView* myQccView, const vector<gp_Pnt>& triPnt);
    void transformTriPnts(std::vector<gp_Pnt>& triPnt1, std::vector<gp_Pnt>& triPnt2, gp_Trsf& trsf);
}

static vector<TopoDS_Face> Hand::geneFaceTri(TopoDS_Face& topoFace)
{
    vector<TopoDS_Face> ret;
    
    //0.create mesher
    BRepMesh_IncrementalMesh brepMesh;

    //1.set TopoDS_Shape to mesher
    TopoDS_Shape topoShp = topoFace;
    brepMesh.SetShape(topoShp);
   
    //2.set IMeshTools_Parameters to mesher
    IMeshTools_Parameters meshParam;
    meshParam.Angle = 1;
    meshParam.Deflection = 1;
    //meshParam.AllowQualityDecrease = Standard_True;
    brepMesh.ChangeParameters() = meshParam;

    //3.set mesh algo to mesh context and mesher perform this context
    Handle(IMeshTools_Context) meshContext = new BRepMesh_Context();
    meshContext->SetFaceDiscret(new BRepMesh_FaceDiscret(new BRepMesh_DelabellaMeshAlgoFactory()));
    brepMesh.Perform(meshContext);

    TopoDS_Face meshFace = TopoDS::Face(topoShp);
    TopLoc_Location aLoc;
    Handle(Poly_Triangulation) triMesh = BRep_Tool::Triangulation(meshFace, aLoc);
    if (triMesh)
    {
        //for-loop mesh data and take triangle points
        TColgp_Array1OfPnt aTriNodes(1, triMesh->NbNodes());
        aTriNodes = triMesh->Nodes();
        Poly_Array1OfTriangle aTriangles(1, triMesh->NbTriangles());
        aTriangles = triMesh->Triangles();
        for (int i = 1; i <= triMesh->NbTriangles(); i++)
        {
            Poly_Triangle trian = aTriangles.Value(i);
            Standard_Integer index1, index2, index3;
            trian.Get(index1, index2, index3);

            gp_Pnt pnt1, pnt2, pnt3;
            pnt1 = aTriNodes[index1];
            pnt2 = aTriNodes[index2];
            pnt3 = aTriNodes[index3];

            vector<gp_Pnt> triPnt;
            if (pnt1.IsEqual(pnt2, 0.01) || pnt1.IsEqual(pnt3, 0.01) || pnt2.IsEqual(pnt3, 0.01))
                continue;
            triPnt.push_back(pnt1);
            triPnt.push_back(pnt2);
            triPnt.push_back(pnt3);

            //construct triangle face
            BRepBuilderAPI_MakePolygon mkPoly;
            mkPoly.Add(triPnt[0]);
            mkPoly.Add(triPnt[1]);
            mkPoly.Add(triPnt[2]);
            mkPoly.Add(triPnt[0]);

            BRepBuilderAPI_MakeFace mkFace(mkPoly.Wire());
            ret.push_back(mkFace.Face());
        }
    }
    return ret;
}

static bool Hand::isObbCollideTri(Bnd_OBB& bndObb, TopoDS_Face& triFace)
{
    vector<gp_Pnt> triPoints;
    for (TopExp_Explorer exp(triFace, TopAbs_VERTEX); exp.More(); exp.Next())
    {
        TopoDS_Shape shape = exp.Current();
        TopoDS_Vertex V = TopoDS::Vertex(shape);
        gp_Pnt P = BRep_Tool::Pnt(V);
        triPoints.push_back(P);
    }
}

static double Hand::getFaceArea(const TopoDS_Shape& face)
{
    GProp_GProps property;
    BRepGProp::SurfaceProperties(face, property);
    return property.Mass();
}

static double Hand::getBndArea(const Bnd_OBB& bndObb, double enlarge)
{
    enlarge += 0.0005;
    if (bndObb.XHSize() < enlarge || bndObb.YHSize() < enlarge || bndObb.ZHSize() < enlarge)
    {
        double area1 = bndObb.XHSize() * bndObb.YHSize() * 4;        //半长轴
        double area2 = bndObb.XHSize() * bndObb.ZHSize() * 4;
        double area3 = bndObb.YHSize() * bndObb.ZHSize() * 4;
        double area = std::max(area1, area2);
        return std::max(area, area3);
    }
    else                                                             //半球面、球面的面包围盒是长方体
    {
        double area1 = bndObb.XHSize() * bndObb.YHSize() * 4;
        double area2 = bndObb.XHSize() * bndObb.ZHSize() * 4;
        double area3 = bndObb.YHSize() * bndObb.ZHSize() * 4;

        double area = (area1 + area2 + area3) * 2;                     //长方体表面积
        return area;
    }
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

static Bnd_OBB Hand::getBoxObb(TopoDS_Shape shp, double enlargeGap = 0.001)
{
    Bnd_OBB boxObb;
    BRepBndLib brepBnd;
    brepBnd.AddOBB(shp, boxObb, true, true, false);

    return boxObb;
}

static void Hand::displayTriangle(const QccView* myQccView, const vector<gp_Pnt>& triPnt)
{
    BRepBuilderAPI_MakePolygon mkPoly;
    mkPoly.Add(triPnt[0]);
    mkPoly.Add(triPnt[1]);
    mkPoly.Add(triPnt[2]);
    mkPoly.Add(triPnt[0]);

    BRepBuilderAPI_MakeFace mkFace(mkPoly.Wire());
    TopoDS_Shape topoFace = mkFace.Shape();
    Handle(AIS_Shape) aisFace = new AIS_Shape(topoFace);
    myQccView->getContext()->Display(aisFace, Standard_True);
}
