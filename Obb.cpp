#include "Obb.h"
#include "ShapeHandle.hpp"
#include <TopExp_Explorer.hxx>

class QccView;

Obb::Obb(TopoDS_Shape topoShp) : topoShape(topoShp)
{
    if (topoShp.IsNull())
        return;

    /* obbShape contruction */
    BRepBndLib repbnd;
    repbnd.AddOBB(topoShape, obbShape, true, true, false);

    /* obbList construction */
    for (TopExp_Explorer exp(topoShape, TopAbs_FACE); exp.More(); exp.Next())
    {
        BRepBndLib facebnd;
        Bnd_OBB obbface;
        facebnd.AddOBB(exp.Value(), obbface, true, true, false);
        obbList.push_back(obbface);
        //discrete face to triangles and save to triList 
        TopoDS_Face topoface = TopoDS::Face(exp.Current());
        triList.push_back(Hand::geneFaceTri(topoface));
    }
}

Obb::~Obb()
{

}

void Obb::displayObb(const QccView* myQccView, ObbLevel obblv)
{
    int count = 0;
    if (obblv == ObbLevel::ObbTriangle)
    {
        if (triList.size() == 0)
            return;
        /* create bndOBB for selected shape */
        for (auto tri : triList)
        {
            count += tri.size();
            for (auto t : tri)
            {
                TopoDS_Shape topo(t);
                Handle(AIS_Shape) aistri = new AIS_Shape(topo);
                aistri->SetColor(Quantity_NOC_GREEN);
                aistri->SetTransparency(0.9);
                myQccView->getContext()->Display(aistri, Standard_False);
            }
            myQccView->getContext()->UpdateCurrentViewer();
        }
        qDebug() << "Obb total triangles are:" << count;
    }
    else if (obblv == ObbLevel::ObbFace)
    {
        for (const auto& fbb : obbList)
        {
            count++;
            TopoDS_Shape fObb = Hand::getBndShape(fbb);
            Handle(AIS_Shape) aface = new AIS_Shape(fObb);
            aface->SetColor(Quantity_NOC_GREEN);
            aface->SetTransparency(0.9);
            myQccView->getContext()->Display(aface, Standard_False);
        }
        myQccView->getContext()->UpdateCurrentViewer();
        qDebug() << "Obb total faces are:" << count;
    }
    else if (obblv == ObbLevel::ObbShape)
    {
        obbShape.Enlarge(5);

        gp_Dir xDir = obbShape.XDirection();
        gp_Dir yDir = obbShape.YDirection();
        gp_Dir zDir = obbShape.ZDirection();
        Standard_Real x = obbShape.XHSize();
        Standard_Real y = obbShape.YHSize();
        Standard_Real z = obbShape.ZHSize();
        obbShape.SetXComponent(xDir, x - 5);
        obbShape.SetYComponent(yDir, y - 5);
        obbShape.SetZComponent(zDir, z - 5);

        TopoDS_Shape topoObb = Hand::getBndShape(obbShape);
        Handle(AIS_Shape) abox = new AIS_Shape(topoObb);
        abox->SetColor(Quantity_NOC_GREEN);
        abox->SetTransparency(0.9);
        myQccView->getContext()->Display(abox, Standard_True);
    }
}

double Obb::getArea()
{
    return Hand::getBndArea(obbShape);
}

Standard_Boolean Obb::isValid()
{
    double faceArea = Hand::getFaceArea(topoShape);
    double obbArea = Hand::getBndArea(obbShape);
    if ((faceArea / obbArea) <= 3.14159 / 4.0)   //有可能是表面或者长方体，都用同一个参数    
    {
        return Standard_False;
    }
    else
    {
        return Standard_True;
    }
}
