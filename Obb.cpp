#include "Obb.h"
#include "ShapeHandle.h"
#include <TopExp_Explorer.hxx>

class QccView;

Obb::Obb(TopoDS_Shape topoShp) : topoShape(topoShp)
{
    if (topoShp.IsNull())
        return;

    BRepBndLib repbnd;
    repbnd.AddOBB(topoShape, obbShape, true, true, false);

    for (TopExp_Explorer exp(topoShape, TopAbs_FACE); exp.More(); exp.Next())
    {
        BRepBndLib facebnd;
        Bnd_OBB obbface;
        TopoDS_Shape toposhp = exp.Current();

        facebnd.AddOBB(toposhp, obbface, true, true, true);
        obbList.push_back(obbface);
       
        TopoDS_Face topoface = TopoDS::Face(toposhp);
        triList.push_back(Hand::geneTriFace(topoface));
    }
}

Obb::~Obb()
{

}

void Obb::displayObb(QccView* myQccView, ObbLevel obblv)
{
    Handle(AIS_InteractiveContext) aisContext = myQccView->getContext();
    if (!aisContext)
        return;

    if (triList.size() == 0)
        return;

    /* create bndOBB for selected shape */
    for (auto tri : triList)
    {
        for (auto t : tri)
        {
            TopoDS_Shape topo(t);
            Handle(AIS_Shape) aistri = new AIS_Shape(topo);
            myQccView->getContext()->Display(aistri, Standard_True);
        }
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
