#include "Obb.h"
#include "ShapeHandle.hpp"
#include <TopExp_Explorer.hxx>

class QccView;

Obb::Obb(TopoDS_Shape topoShp) : topoShape(topoShp)
{
    if (topoShp.IsNull())
        return;

    /* shape obb contruction */
    BRepBndLib repbnd;  //obbShape is Bnd_OBB
    repbnd.AddOBB(topoShape, obbShape, true, true, false);

    /* face obb list construction */
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

Obb::Obb(std::vector<TopoDS_Shape> topoShps)
{
    std::vector<gp_Pnt> pntList;
    for (auto shp : topoShps)
    {
        Bnd_OBB obb;
        BRepBndLib build_bnd;
        build_bnd.AddOBB(shp, obb, true, true, false);

        // get vertex from bnd vertex
        gp_Pnt pnt[8];
        obb.GetVertex(pnt);
        for (int i = 0; i < 8; i++)
            pntList.push_back(pnt[i]);
    }

   obbShape = Bnd_OBB_genWithPoints(pntList);
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

Bnd_OBB Obb::Bnd_OBB_genWithPoints(std::vector<gp_Pnt> Points)
{
    //主成分分析法(PCA)
    Bnd_OBB retObb;
    //构建 Nx3 的矩阵
    math_Matrix X(0, Points.size()-1, 0, 2);
    for (int i = 0; i < Points.size(); i++)
    {
        X(i, 0) = Points[i].X();
        X(i, 1) = Points[i].Y();
        X(i, 2) = Points[i].Z();
    }
    //计算一维度均值
    math_Vector meanVal(0, 2, 0.0);
    for (int i = 0; i < X.RowNumber(); i++)
    {
        meanVal += X.Row(i);
    }
    meanVal /= X.RowNumber();
    math_Vector meanVecRow(meanVal);
    //样本均值化为0
    math_Matrix X2(X);
    for (int i = 0; i < X2.RowNumber(); i++)
    {
        X2.Row(i) -= meanVecRow;
    }
    //计算协方差矩阵: C= XT*X/(n-1)
    math_Matrix C2 = X2.Transposed() * X2;
    C2 = C2 / (X2.RowNumber() - 1.0);
    //获取特征值和特征向量 


    return retObb;
}
