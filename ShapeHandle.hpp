#pragma once

#include "Qcc.h"
#include "QccView.h"
#include <QDebug>
#include <algorithm>
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <NCollection_Mat4.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TColgp_Array1OfPnt.hxx>

#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <GeomLProp_SLProps.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GCPnts_UniformAbscissa.hxx>

#include <AIS_Shape.hxx>
#include <AIS_ColoredShape.hxx>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <GeomLProp_SLProps.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepLProp_CLProps.hxx>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepPrimAPI_MakePrism.hxx>

#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_FaceDiscret.hxx>
#include <BRepMesh_Context.hxx>

#include <PrsMgr_ListOfPresentableObjects.hxx>
#include <PrsMgr_ListOfPresentations.hxx>
#include <PrsMgr_PresentableObject.hxx>
#include <PrsMgr_Presentation.hxx>
#include <BRepBndLib.hxx>
#include <BRepGProp.hxx>
#include <Bnd_OBB.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>

#define random(x) rand()%(x*2)-x+rand()/double(RAND_MAX)

using std::vector;

namespace Hand
{
    bool isSameTrsf(gp_Trsf t1, gp_Trsf t2, double precision = 0.0001);
    void displaySelected(Handle(AIS_Shape) aisObj);
    double getDihedralAngle(TopoDS_Edge& edge, TopoDS_ListOfShape& findFace, TopoDS_Shape& myShape);
    double getEdgeLength(TopoDS_Shape& edge);
    gp_Vec getEdgeNormal(TopoDS_Edge aedge, bool fromStart);
    gp_Vec getPlaneNormal(TopoDS_Face& face);
    void getSurfType(TopoDS_Face& face);

    vector<gp_Pnt> geneRandTri(void);
    double getFaceArea(const TopoDS_Shape& face);
    double getBndArea(const Bnd_OBB& bndObb, double enlarge = 0.0001);

    TopoDS_Shape getBndShape(const Bnd_OBB&);
    TopoDS_Shape TriangleGetShape(std::vector<gp_Pnt>& triPoints);
    Bnd_OBB transformOBB(Bnd_OBB&, gp_Trsf&);
    Bnd_OBB getBoxObb(TopoDS_Shape, double);
    Bnd_Box getFullAABB(Handle(AIS_Shape) shp);

    vector<TopoDS_Face> geneFaceTri(TopoDS_Face& topoFace);
    vector<gp_Pnt> transformTriPnts(std::vector<gp_Pnt>& triPnt, gp_Trsf& trsf);

    bool isAABBCollideTri(Bnd_OBB& bndObb, TopoDS_Face& triFace);
    bool isOBBCollideTri(Bnd_OBB& bndObb, TopoDS_Face& triFace);
    void displayTriangle(const QccView* myQccView, const vector<gp_Pnt>& triPnt);
}

static bool Hand::isSameTrsf(gp_Trsf t1, gp_Trsf t2, double precision)
{
    NCollection_Mat4<float> mat1, mat2;
    t1.GetMat4(mat1);
    t2.GetMat4(mat2);

    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            auto v1 = mat1.GetValue(r, c);
            auto v2 = mat2.GetValue(r, c);
            if (abs(v1 - v2) > precision)
                return false;
        }
    }

    return true;
}

static void Hand::displaySelected(Handle(AIS_Shape) aisObj)
{
    if (aisObj->Children().Size() > 0)
    {
        for (PrsMgr_ListOfPresentableObjectsIter i(aisObj->Children()); i.More(); i.Next())
        {
            // 获取当前模型
            Handle(PrsMgr_PresentableObject) t_object = i.Value();
            // 若为CModel类型则转换为CModel
            if (t_object->IsKind(STANDARD_TYPE(AIS_Shape)))
            {
                Handle(AIS_Shape) t_child_model = Handle(AIS_Shape)::DownCast(t_object);
                displaySelected(t_child_model);
            }
        }
    }
    else
    {
        aisObj->SetColor(Quantity_NOC_FIREBRICK);
        aisObj->SetTransparency(0.7);
        glbContext->Display(aisObj, Standard_True);
    }
}

static double Hand::getDihedralAngle(TopoDS_Edge& edge, TopoDS_ListOfShape& findFace, TopoDS_Shape& myShape)
{
    Standard_Real first, last;
    Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);
    GeomAdaptor_Curve GAC(curve);
    GeomAbs_CurveType curveType = GAC.GetType();
    gp_Dir nor0 = gp_Dir(Hand::getEdgeNormal(edge, true));
    double angle = 0.0;
    
    //TopoDS_Face face1 = TopoDS::Face(findFace.First());
    //Handle(Geom_Surface) gfc1 = BRep_Tool::Surface(face1);
    //GeomAdaptor_Surface GAS1(gfc1);
    //GeomAbs_SurfaceType faceType1 = GAS1.GetType();

    //TopoDS_Face face2 = TopoDS::Face(findFace.Last());
    //Handle(Geom_Surface) gfc2 = BRep_Tool::Surface(face2);
    //GeomAdaptor_Surface GAS2(gfc2);
    //GeomAbs_SurfaceType faceType2 = GAS2.GetType();

    //if (curveType == GeomAbs_Line)
    //{
    //    gp_Vec normal1 = Hand::getPlaneNormal(face1);
    //    gp_Vec normal2 = Hand::getPlaneNormal(face2);
    //    gp_Dir nor12 = gp_Dir(normal1.Crossed(normal2));
    //    gp_Dir nor21 = gp_Dir(normal2.Crossed(normal1));
    //    angle = M_PI - normal1.Angle(normal2);

    //    if (nor21.IsEqual(nor0, 1e-5))
    //        angle =  angle / M_PI * 180.0;
    //    else
    //        angle = (2 * M_PI - angle) / M_PI * 180.0;
    //}

    BRepAdaptor_Curve adpCurve = BRepAdaptor_Curve(edge);
    gp_Dir norMid;
    Standard_Real aparam;
    aparam = (adpCurve.FirstParameter() + adpCurve.LastParameter()) / 2.0;
    BRepLProp_CLProps props = BRepLProp_CLProps(adpCurve, aparam, 1, 1.0e-7);
    props.Tangent(norMid);

    gp_Pnt circleCtr = curve->Value((first + last) / 2.0);
    gp_Dir circleDir = norMid;
    TopoDS_Shape wire1 = BRepBuilderAPI_MakeEdge(gp_Circ(gp_Ax2(circleCtr, circleDir), 0.1)).Shape();
    TopoDS_Shape wire2 = BRepAlgoAPI_Cut(wire1, myShape).Shape();

    Handle(AIS_Shape) aisWire = new AIS_Shape(wire2);
    glbContext->Display(aisWire, Standard_True);

    double len1 = Hand::getEdgeLength(wire1);
    double len2 = Hand::getEdgeLength(wire2);
    angle = len2 / len1 * 360;

    return angle;
}

static double Hand::getEdgeLength(TopoDS_Shape& edge)
{
    GProp_GProps lprop;
    BRepGProp::LinearProperties(edge, lprop);
    return lprop.Mass();

    //Standard_Real first, last;
    //Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);
    //GeomAdaptor_Curve GAC(curve);
    //return GCPnts_AbscissaPoint::Length(GAC);
}

static gp_Vec Hand::getEdgeNormal(TopoDS_Edge aedge, bool fromStart = true)
{
    BRepAdaptor_Curve adpCurve = BRepAdaptor_Curve(aedge);
    gp_Dir normalStart;
    Standard_Real aparam;
    if (fromStart)
        aparam = adpCurve.FirstParameter();
    else
        aparam = adpCurve.LastParameter();
    BRepLProp_CLProps props = BRepLProp_CLProps(adpCurve, aparam, 1, 1.0e-7);
    props.Tangent(normalStart);

    gp_Vec normal = gp_Vec(normalStart);
    return aedge.Orientation() ? normal.Reversed() : normal;
}

static gp_Vec Hand::getPlaneNormal(TopoDS_Face& face)
{
    //Method 1
    Handle(Geom_Surface) gfc = BRep_Tool::Surface(face);
    Standard_Real umin, umax, vmin, vmax;
    BRepTools::UVBounds(face, umin, umax, vmin, vmax);
    GeomLProp_SLProps props(gfc, umin, vmin, 1, 0.01);
    gp_Vec normal1 = gp_Vec(props.Normal());

    //Method 2
    BRepAdaptor_Surface aSurf(face);
    Standard_Real u1, u2, v1, v2;
    u1 = aSurf.FirstUParameter();
    u2 = aSurf.LastUParameter();
    v1 = aSurf.FirstVParameter();
    v2 = aSurf.LastVParameter();
    gp_Pnt aCenter;
    gp_Vec aVec1, aVec2, normal2;
    aSurf.D1((u1 + u2) / 2.0, (v1 + v2) / 2.0, aCenter, aVec1, aVec2);
    normal2 = aVec1 ^ aVec2;

    return face.Orientation() ? normal1.Reversed() : normal1;
}

static void Hand::getSurfType(TopoDS_Face& face)
{
    Handle(Geom_Surface) gfc = BRep_Tool::Surface(face);
    GeomAdaptor_Surface GAS(gfc);
    GeomAbs_SurfaceType faceType = GAS.GetType();

    switch (faceType)
    {
    case 0:
        qDebug() << "Face type is: GeomAbs_Plane";
        break;
    case 1:
        qDebug() << "Face type is: GeomAbs_Cylinder";
        break;
    case 2:
        qDebug() << "Face type is: GeomAbs_Cone";
        break;
    case 3:
        qDebug() << "Face type is: GeomAbs_Sphere";
        break;
    case 4:
        qDebug() << "Face type is: GeomAbs_Torus";
        break;
    case 5:
        qDebug() << "Face type is: GeomAbs_BezierSurface";
        break;
    case 6:
        qDebug() << "Face type is: GeomAbs_BSplineSurface";
        break;
    case 7:
        qDebug() << "Face type is: GeomAbs_SurfaceOfRevolution";
        break;
    case 8:
        qDebug() << "Face type is: GeomAbs_SurfaceOfExtrusion";
        break;
    case 9:
        qDebug() << "Face type is: GeomAbs_OffsetSurface";
        break;
    case 10:
        qDebug() << "Face type is: GeomAbs_OtherSurface";
        break;
    }
}

static vector<gp_Pnt> Hand::geneRandTri()
{
    /* create three point as triangle */
    vector<gp_Pnt> ret;
    srand((int)time(0));
    double x = random(15);
    double y = random(15);
    double z = random(15);
    gp_Pnt p(x, y, z);
    ret.push_back(p);

    gp_Trsf trsf1;
    trsf1.SetTranslation(gp_Vec(random(2), random(2), random(2)));
    ret.push_back(p.Transformed(trsf1));

    gp_Trsf trsf2;
    trsf2.SetTranslation(gp_Vec(random(2), random(2), random(2)));
    ret.push_back(p.Transformed(trsf2));
    
    return ret;
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

static bool Hand::isOBBCollideTri(Bnd_OBB& bndObb, TopoDS_Face& triFace)
{
    vector<gp_Pnt> triPoints;
    int index = 0;
    for (TopExp_Explorer exp(triFace, TopAbs_VERTEX); exp.More(); exp.Next())
    {
        if ((++index) % 2 == 0)
            continue;
        TopoDS_Shape shape = exp.Current();
        TopoDS_Vertex V = TopoDS::Vertex(shape);
        gp_Pnt P = BRep_Tool::Pnt(V);
        triPoints.push_back(P);
    }

    gp_Vec f0(triPoints[0], triPoints[1]);
    gp_Vec f1(triPoints[1], triPoints[2]);
    gp_Vec f2(triPoints[2], triPoints[0]);
    std::vector<gp_Vec> triVecs{ f0, f1, f2 };

    gp_Vec bndXv = gp_Vec(bndObb.XDirection());
    gp_Vec bndYv = gp_Vec(bndObb.YDirection());
    gp_Vec bndZv = gp_Vec(bndObb.ZDirection());
    gp_Vec bndOv = gp_Vec(bndObb.Center());
    double bndX = bndObb.XHSize();
    double bndY = bndObb.YHSize();
    double bndZ = bndObb.ZHSize();
    /* the bndVecs are all normalized */
    std::vector<gp_Vec> boxVecs{ bndXv, bndYv, bndZv };
   
    /* 1.check 9 crossed vectors results */
    for (int i = 0; i < 3; i++) //i is loop triVecs
    {
        for (int j = 0; j < 3; j++) //j is loop boxVecs
        {
            gp_Vec vec = triVecs[i].Crossed(boxVecs[j]);
            double dBox = bndOv.Dot(vec);
            double rBox = bndX * abs(bndXv.Dot(vec)) + bndY * abs(bndYv.Dot(vec)) + bndZ * abs(bndZv.Dot(vec));
            double p0 = triPoints[0].X() * vec.X() + triPoints[0].Y() * vec.Y() + triPoints[0].Z() * vec.Z();
            double p1 = triPoints[1].X() * vec.X() + triPoints[1].Y() * vec.Y() + triPoints[1].Z() * vec.Z();
            double p2 = triPoints[2].X() * vec.X() + triPoints[2].Y() * vec.Y() + triPoints[2].Z() * vec.Z();
            double pmin, pmax = p0;
            switch (i) {
                case 0:
                    pmin = std::min(p0, p2);
                    pmax = std::max(p0, p2);
                    break;
                case 1:
                    pmin = std::min(p0, p2);
                    pmax = std::max(p0, p2);
                    break;
                case 2:
                    pmin = std::min(p0, p1);
                    pmax = std::max(p0, p1);
                    break;
            }
            if (pmin > rBox+dBox || pmax < -rBox+dBox)
                return false;
        }
    }

    /* 2.check 3 box face vectors results */
    for (int j = 0; j < 3; j++) //j is loop boxVecs
    {
        gp_Vec vec = boxVecs[j];
        double rBox = 0.0;
        double dBox = bndOv.Dot(vec);
        double p0 = triPoints[0].X() * vec.X() + triPoints[0].Y() * vec.Y() + triPoints[0].Z() * vec.Z();
        double p1 = triPoints[1].X() * vec.X() + triPoints[1].Y() * vec.Y() + triPoints[1].Z() * vec.Z();
        double p2 = triPoints[2].X() * vec.X() + triPoints[2].Y() * vec.Y() + triPoints[2].Z() * vec.Z();
        double pmin = std::min(p0, std::min(p1, p2));
        double pmax = std::max(p0, std::max(p1, p2));
        switch(j) {
            case 0:
                rBox = bndX * abs(bndXv.Dot(vec));
                break;
            case 1:
                rBox = bndY * abs(bndYv.Dot(vec));
                break;
            case 2:
                rBox = bndZ * abs(bndZv.Dot(vec));
                break;
        }
        if (pmin > rBox+dBox || pmax < -rBox+dBox)
            return false;
    }

    /* 3.check 1 triangle face vector result */
    gp_Vec vec = f0.Crossed(f1);
    double p = triPoints[0].X() * vec.X() + triPoints[0].Y() * vec.Y() + triPoints[0].Z() * vec.Z();
    double rBox = bndX * abs(bndXv.Dot(vec)) + bndY * abs(bndYv.Dot(vec)) + bndZ * abs(bndZv.Dot(vec));
    double dBox = bndOv.Dot(vec);
    if (p > rBox + dBox || p < -rBox + dBox)
        return false;

    return true;
}

static bool Hand::isAABBCollideTri(Bnd_OBB& bndObb, TopoDS_Face& triFace)
{
    //transform Obb and Tri to AABB status
    vector<gp_Pnt> triPoints;
    int index = 0;
    for (TopExp_Explorer exp(triFace, TopAbs_VERTEX); exp.More(); exp.Next())
    {
        if ((++index) % 2 == 0)
            continue;
        TopoDS_Shape shape = exp.Current();
        TopoDS_Vertex V = TopoDS::Vertex(shape);
        gp_Pnt P = BRep_Tool::Pnt(V);
        triPoints.push_back(P);
    }

    gp_Trsf loc, locInvs;
    loc.SetTransformation(bndObb.Position(), gp::XOY());
    loc.Invert();

    Bnd_OBB tObb = Hand::transformOBB(bndObb, loc);
    vector<gp_Pnt> tTri = Hand::transformTriPnts(triPoints, loc);

    //OBB旋转后的三个轴向量
    gp_Vec e0(gp_Dir(1, 0, 0));
    gp_Vec e1(gp_Dir(0, 1, 0));
    gp_Vec e2(gp_Dir(0, 0, 1));
    std::vector<gp_Vec> vecs{ e0,e1,e2 };

    //三角形的3个边向量(不作测试)
    gp_Vec f0(tTri[0], tTri[1]);
    gp_Vec f1(tTri[1], tTri[2]);
    gp_Vec f2(tTri[2], tTri[0]);
    std::vector<gp_Vec> triVecs{ f0,f1,f2 };

    //三角形的一个法向量
    gp_Vec e3 = f0.Crossed(f1).Normalized();
    vecs.push_back(e3);

    //边x边的九个向量
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            //just for-loop e0, e1, e2
            gp_Vec e = vecs[i].Crossed(triVecs[j]);
            if (e.X() != 0 && e.Y() != 0 && e.Z() != 0)
                e.Normalize();
            vecs.push_back(e);
        }
    }

    //AABB的三个面的轴向单位的判断，直接构建三角面的包围盒，前三个向量
    Bnd_OBB triObb;
    TColgp_Array1OfPnt pnts(1, 3);
    pnts.SetValue(1, tTri[0]);
    pnts.SetValue(2, tTri[1]);
    pnts.SetValue(3, tTri[2]);
    triObb.ReBuild(pnts);
    if (triObb.IsOut(tObb))
        return false;

    //三角形法向量投影是否重合 vecs[3]
    gp_Vec p(tObb.Center(), tTri[0]);
    double d = abs(p.Dot(e3));  //中心点到三角面的距离
    double dd = tObb.XHSize() * abs(e3.X()) + tObb.YHSize() * abs(e3.Y()) + tObb.ZHSize() * abs(e3.Z());
    if (d > dd)
        return false;

    //边x边叉积的九个向量 vecs[4~12]
    for (int i = 4; i < 13; i++)
    {
        //三角形顶点在vecs[i]的投影
        double p0 = vecs[i].X() * tTri[0].X() + vecs[i].Y() * tTri[0].Y() + vecs[i].Z() * tTri[0].Z();
        double p1 = vecs[i].X() * tTri[1].X() + vecs[i].Y() * tTri[1].Y() + vecs[i].Z() * tTri[1].Z();
        double p2 = vecs[i].X() * tTri[2].X() + vecs[i].Y() * tTri[2].Y() + vecs[i].Z() * tTri[2].Z();
        
        double minTep = p0, maxTep = p0;

        minTep = std::min(p0, p1);
        minTep = std::min(minTep, p2);

        maxTep = std::max(p0, p1);
        maxTep = std::max(maxTep, p2);

        double r = tObb.XHSize() * abs(vecs[i].X()) + tObb.YHSize() * abs(vecs[i].Y()) + tObb.ZHSize() * abs(vecs[i].Z());

        if (minTep > r || maxTep < -r)
            return false;
    }

    return true;
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
	tObb.Enlarge(0.01); //just for display bndBox

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

static vector<gp_Pnt> Hand::transformTriPnts(std::vector<gp_Pnt>& triPnt, gp_Trsf& trsf)
{
    vector<gp_Pnt> tTri;
    for (int i = 0; i < triPnt.size(); i++)
    {
        tTri.push_back(triPnt[i].Transformed(trsf));
    }

    return tTri;
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

static Bnd_Box Hand::getFullAABB(Handle(AIS_Shape) shp)
{
    Bnd_Box t_box;
    if (shp->Children().Size())
    {
        for (PrsMgr_ListOfPresentableObjectsIter it(shp->Children()); it.More(); it.Next())
        {
            Handle(PrsMgr_PresentableObject) t_obj = it.Value();
            if (t_obj->IsKind(STANDARD_TYPE(AIS_ColoredShape)))
            {
                Handle(AIS_ColoredShape) t_child = Handle(AIS_ColoredShape)::DownCast(t_obj);
                Bnd_Box t_child_box = getFullAABB(t_child);
                t_box.Add(t_child_box);
            }
        }
    }
    else
    {
        if (!shp->Shape().IsNull())
        {
            t_box = shp->BoundingBox();
            t_box = t_box.Transformed(shp->Transformation());
        }
    }
    return t_box;
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
    glbContext->Display(aisFace, Standard_True);
}
