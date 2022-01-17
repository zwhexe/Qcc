#include "Mesh.h"
#include "ShapeHandle.hpp"
#include <TopLoc_Location.hxx>

#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>
#include <BRepMesh_EdgeDiscret.hxx>
#include <BRepMesh_FaceDiscret.hxx>

Mesh::Mesh(TopoDS_Shape topoShp) : meshShape(topoShp)
{
	if (meshShape.IsNull())
		return;

	BRepMesh_IncrementalMesh mesher;
	mesher.SetShape(meshShape);
}

Mesh::~Mesh()
{

}

void Mesh::setMeshParam()
{
    meshParam.Angle = 1;
    meshParam.Deflection = 1;
    meshParam.AngleInterior = 1.0;
    meshParam.DeflectionInterior = 1.0;
    meshParam.MinSize = -1.0;
    meshParam.InParallel = Standard_True;
    meshParam.Relative = Standard_False;
    meshParam.InternalVerticesMode = Standard_True;
    meshParam.ControlSurfaceDeflection = Standard_True;
    meshParam.CleanModel = Standard_True;
    meshParam.AdjustMinSize = Standard_False;
    meshParam.ForceFaceDeflection = Standard_False;
    meshParam.AllowQualityDecrease = Standard_True;
    
    mesher.ChangeParameters() = meshParam;
}

void Mesh::setMeshContext()
{
    meshContext = new BRepMesh_Context();
    meshContext->SetFaceDiscret(new BRepMesh_FaceDiscret(new BRepMesh_DelabellaMeshAlgoFactory()));
    
    mesher.Perform(meshContext);
}

void Mesh::makeTriangle(bool isCustom)
{
    if (isCustom)
    {
        setMeshParam();
        setMeshContext();
    }

    for (TopExp_Explorer exp(meshShape, TopAbs_FACE); exp.More(); exp.Next())
    {
        TopLoc_Location aLoc;
        TopoDS_Face aFace = TopoDS::Face(exp.Current());
        Handle(Poly_Triangulation) triMesh = BRep_Tool::Triangulation(aFace, aLoc);
        if (triMesh)
        {
            TColgp_Array1OfPnt aTriNodes(1, triMesh->NbNodes());
            aTriNodes = triMesh->Nodes();

            Poly_Array1OfTriangle aTriangles(1, triMesh->NbTriangles());
            aTriangles = triMesh->Triangles();

            //index start from 1 to 3 (not 0)
            for (int i = 1; i <= triMesh->NbTriangles(); i++)
            {
                Poly_Triangle tri = aTriangles.Value(i);
                Standard_Integer index1, index2, index3;
                tri.Get(index1, index2, index3);

                gp_Pnt pnt1, pnt2, pnt3;
                pnt1 = aTriNodes[index1].Transformed(aLoc.Transformation());
                pnt2 = aTriNodes[index2].Transformed(aLoc.Transformation());
                pnt3 = aTriNodes[index3].Transformed(aLoc.Transformation());

                if (pnt1.IsEqual(pnt2, 0.0001) || pnt2.IsEqual(pnt3, 0.0001) || pnt3.IsEqual(pnt1, 0.0001))
                    continue;
               
                vector<gp_Pnt> triPnt;
                triPnt.push_back(pnt1);
                triPnt.push_back(pnt2);
                triPnt.push_back(pnt3);
                triPnts.push_back(triPnt);
            }
        }
    } 
}

void Mesh::displayTriangle()
{
    for (auto triPnt : triPnts)
    {
        BRepBuilderAPI_MakePolygon mkPoly;
        mkPoly.Add(triPnt[0]);
        mkPoly.Add(triPnt[1]);
        mkPoly.Add(triPnt[2]);
        mkPoly.Add(triPnt[0]);

        BRepBuilderAPI_MakeFace mkFace(mkPoly.Wire());
        TopoDS_Shape topoFace = mkFace.Shape();
        Handle(AIS_Shape) aisFace = new AIS_Shape(topoFace);
        aisFace->SetColor(Quantity_NOC_DARKOLIVEGREEN4);
        glbContext->Display(aisFace, Standard_False);
    }
}

int Mesh::countTriangle()
{
    return triPnts.size();
}