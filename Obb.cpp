#include "Obb.h"
#include "ShapeHandle.h"

class QccView;

Obb::Obb(TopoDS_Shape topoShp)
{
    if (topoShp.IsNull())
        return;

    BRepBndLib repbnd;
    repbnd.AddOBB(topoShp, obbShape, true, true, false);
}

Obb::~Obb()
{

}

void Obb::displayOBB(QccView* myQccView, ObbLevel obblv)
{
    Handle(AIS_InteractiveContext) aisContext = myQccView->getContext();
    if (!aisContext)
        return;

    TopoDS_Shape topoShp = aisContext->DetectedShape();
    if (topoShp.IsNull())
        return;

    /* create bndOBB for selected shape */
    BRepBndLib ret;
    ret.AddOBB(topoShp, obbShape, true, true, false);

    /* convert BndOBB to TopoShape for display */
    TopoDS_Shape topoObb = Hand::getBndShape(obbShape);
    Handle(AIS_Shape) abox = new AIS_Shape(topoObb);
    abox->SetTransparency();
    myQccView->getContext()->Display(abox, Standard_True);
}

void Obb::isValid()
{

}
