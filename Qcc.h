#pragma once

#include <iostream>
#include <vector>

#include <QMainWindow>
#include <QDebug>

#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <gp_Lin2d.hxx>

#include <Geom_ConicalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>

#include <GCE2d_MakeSegment.hxx>

#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>

#include <BRepLib.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeWedge.hxx>

#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>

#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepTools.hxx>

#include <Bnd_OBB.hxx>
#include <BRepBndLib.hxx>

#include "ui_Qcc.h"

using std::vector;

namespace Ui {
    class QccClass;
}

/*
* declaration class in .h
* include class in .cpp
*/
class QccView;

class Qcc : public QMainWindow
{
    Q_OBJECT

public:
    Qcc(QWidget *parent = Q_NULLPTR);
    ~Qcc();

public slots:
    void load();
    void save();

protected:
    void createActions(void);
    void createMenus(void);
    void createToolBars(void);
    void createStatusBar(void);

    void makeCylindericalHelix(void);

private slots:
    /* Help */
    void about(void);
    void test(void);
    void erase(void);

    /* Primitive */
    void makeBox(void);
    void makeCone(void);
    void makeSphere(void);
    void makeCylinder(void);
    void makeTorus(void);
    void makeWedge(void);
    void makeHollow(void);
    void makeFaceHole(void);
    
    /* Modeling */
    void makeFillet(void);
    void makeChamfer(void);
    void makeExtrude(void);
    void makeRevol(void);
    void makeLoft(void);

    /* test tool functions */
    void testCut(void);
    void testHelix(void);
    void obbShape(void);
    void meshShape(bool);
    void deleteShape(void);

private:
    Ui::QccClass *ui;
    QccView* myQccView;
    Handle(AIS_InteractiveObject) aisBox;
    Handle(AIS_InteractiveObject) aisFace;
    TopoDS_Shape myBox;
    TopoDS_Shape myFace;
    TopoDS_Face myTri;
};

extern Handle(AIS_InteractiveContext) glbContext;
