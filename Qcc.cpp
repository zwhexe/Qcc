#include "Qcc.h"
#include "Obb.h"
#include "QccView.h"
#include "ShapeHandle.h"

#include <QDebug>
#include <QToolBar>
#include <QTreeView>
#include <QMessageBox>
#include <QDockWidget>
#include <QFileDialog>

#include <IMeshTools_Parameters.hxx>
#include <IMeshTools_Context.hxx>

#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepMesh_DelabellaMeshAlgoFactory.hxx>
#include <BRepMesh_EdgeDiscret.hxx>
#include <BRepMesh_FaceDiscret.hxx>
#include <BRepMesh_Context.hxx>
#include <BRepMesh_Delaun.hxx>

#include <TopLoc_Location.hxx>
#include <STEPControl_Reader.hxx>

Qcc::Qcc(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::QccClass)
{
    ui->setupUi(this);
    myQccView = new QccView(this);
    setCentralWidget(myQccView);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    ui->menuPrimitive->addSeparator();
    /* make a cylinder with hollow */
    QAction* hollow = new QAction;
    hollow->setIconText(tr("Hollow"));
    ui->menuPrimitive->addAction(hollow); 
    connect(hollow, &QAction::triggered, this, &Qcc::makeHollow);

    /* make a box with hole */
    QAction* facehole = new QAction;
    facehole->setIconText(tr("Hole"));
    ui->menuPrimitive->addAction(facehole);
    connect(facehole, &QAction::triggered, this, &Qcc::makeFaceHole);
}

Qcc::~Qcc()
{
    delete myQccView;
    delete ui;
}

void Qcc::createActions(void)
{
    /* connect ui menu's action with slots */
    /* File */
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    /* View */
    connect(ui->actionZoom, SIGNAL(triggered()), myQccView, SLOT(zoom()));
    connect(ui->actionPan, SIGNAL(triggered()), myQccView, SLOT(pan()));
    connect(ui->actionRotate, SIGNAL(triggered()), myQccView, SLOT(rotate()));
    connect(ui->actionReset, SIGNAL(triggered()), myQccView, SLOT(reset()));
    connect(ui->actionFitAll, SIGNAL(triggered()), myQccView, SLOT(fitAll()));
    /* Primitive */
    connect(ui->actionBox, SIGNAL(triggered()), this, SLOT(makeBox()));
    connect(ui->actionCone, SIGNAL(triggered()), this, SLOT(makeCone()));
    connect(ui->actionSphere, SIGNAL(triggered()), this, SLOT(makeSphere()));
    connect(ui->actionCylinder, SIGNAL(triggered()), this, SLOT(makeCylinder()));
    connect(ui->actionTorus, SIGNAL(triggered()), this, SLOT(makeTorus()));
    connect(ui->actionWedge, &QAction::triggered, this, &Qcc::makeWedge);
    /* Modeling */
    connect(ui->actionFillet, SIGNAL(triggered()), this, SLOT(makeFillet()));
    connect(ui->actionChamfer, SIGNAL(triggered()), this, SLOT(makeChamfer()));
    connect(ui->actionExtrude, SIGNAL(triggered()), this, SLOT(makeExtrude()));
    connect(ui->actionRovelve, SIGNAL(triggered()), this, SLOT(makeRevol()));
    connect(ui->actionLoft, SIGNAL(triggered()), this, SLOT(makeLoft()));

    connect(ui->actionCut, &QAction::triggered, this, &Qcc::testCut);
    connect(ui->actionHelix, &QAction::triggered, this, &Qcc::testHelix);
    /* About */
    connect(ui->actionAbout, &QAction::triggered, this, &Qcc::about);
    connect(ui->actionTest, &QAction::triggered, this, &Qcc::test);
    
    /* myQccView signal to do "test" */
    connect(myQccView, &QccView::obbSig, this, &Qcc::obbShape);
    connect(myQccView, &QccView::meshSig, this, &Qcc::meshShape);
    connect(myQccView, &QccView::deleteSig, this, &Qcc::deleteShape);
}

void Qcc::createMenus(void)
{
    
}

void Qcc::createToolBars(void)
{
    ui->mainToolBar->addAction(ui->actionLoad);
    ui->mainToolBar->addAction(ui->actionSave);
    QToolBar* aToolBar = new QToolBar;
    
    aToolBar = addToolBar(tr("&Navigate"));
    aToolBar->addAction(ui->actionZoom);
    aToolBar->addAction(ui->actionPan);
    aToolBar->addAction(ui->actionRotate);
    
    aToolBar = addToolBar(tr("&View"));
    aToolBar->addAction(ui->actionReset);
    aToolBar->addAction(ui->actionFitAll);

    aToolBar = addToolBar(tr("&Primitive"));
    aToolBar->addAction(ui->actionBox);
    aToolBar->addAction(ui->actionCone);
    aToolBar->addAction(ui->actionSphere);
    aToolBar->addAction(ui->actionCylinder);
    aToolBar->addAction(ui->actionTorus);
    aToolBar->addAction(ui->actionWedge);

    this->addToolBarBreak();
    
    aToolBar = addToolBar(tr("&Modeling"));
    aToolBar->addAction(ui->actionFillet);
    aToolBar->addAction(ui->actionChamfer);
    aToolBar->addAction(ui->actionExtrude);
    aToolBar->addAction(ui->actionRovelve);
    aToolBar->addAction(ui->actionLoft);
    aToolBar->addAction(ui->actionHelix);
    aToolBar->addSeparator();
    aToolBar->addAction(ui->actionCut);

    aToolBar = addToolBar(tr("&Help"));
    aToolBar->addAction(ui->actionAbout);
    QAction* eraseAll = new QAction("Erase");
    connect(eraseAll, &QAction::triggered, this, &Qcc::erase);
    aToolBar->addAction(eraseAll);
}

void Qcc::createStatusBar()
{
    QStatusBar* aStatusBar = new QStatusBar(this);
    this->setStatusBar(aStatusBar);
    ui->actionAbout->setStatusTip(tr("Qcc Demo Info"));
    ui->actionReset->setStatusTip(tr("Reset Model View"));
    ui->actionFitAll->setStatusTip(tr("Fit Models View"));
    ui->actionWedge->setStatusTip(tr("Make Wedge Model"));
}

void Qcc::about()
{
    QMessageBox::about(this, tr("About Qcc"),
        tr("<h2>Qcc 1.0</h2>"
            "<p>Qcc is a demo application about Qt and OpenCascade</p>"));
}

void Qcc::test()
{
    
}

void Qcc::erase()
{
    myQccView->getContext()->EraseAll(Standard_True);
}

void Qcc::meshShape(bool allowLow)
{
    if (myQccView->getContext()->HasDetectedShape())
    {
        Handle(AIS_InteractiveObject) aisObj = myQccView->getContext()->DetectedInteractive();
        myQccView->getContext()->Erase(aisObj, Standard_True);

        TopoDS_Shape topoShp = myQccView->getContext()->DetectedShape();
        TopoDS_Shape meshShp = topoShp;

        IMeshTools_Parameters meshParam;
        meshParam.Angle = 10;
        meshParam.Deflection = 10;
        meshParam.AngleInterior = 10.0;
        meshParam.DeflectionInterior = 10.0;
        meshParam.MinSize = -1.0;
        meshParam.InParallel = Standard_True;
        meshParam.Relative = Standard_False;
        meshParam.InternalVerticesMode = Standard_True;
        meshParam.ControlSurfaceDeflection = Standard_True;
        meshParam.CleanModel = Standard_True;
        meshParam.AdjustMinSize = Standard_False;
        meshParam.ForceFaceDeflection = Standard_False;
        meshParam.AllowQualityDecrease = allowLow;

        Handle(IMeshTools_Context) meshContext = new BRepMesh_Context();
        meshContext->SetFaceDiscret(new BRepMesh_FaceDiscret(new BRepMesh_DelabellaMeshAlgoFactory()));

        BRepMesh_IncrementalMesh mesher;
        mesher.SetShape(meshShp);
        mesher.ChangeParameters() = meshParam;
        mesher.Perform(meshContext);

#if 1
        int index = 0, count = 0;
        for (TopExp_Explorer exp(meshShp, TopAbs_FACE); exp.More(); exp.Next())
        {
            ++index;
            TopoDS_Shape face = exp.Value();
           
            /* Check whether Bnd_OBB in this face is valid */
            Obb obbFace(face);
            if (obbFace.isValid()) 
            { 
                //qDebug() << "Face number." << index << " is valid, skip mesh\n";
                continue;   //face bnd_obb match check standard
            }

            TopLoc_Location aLoc;
            TopoDS_Face aFace = TopoDS::Face(exp.Current());
            /* Get meshed face data */
            Handle(Poly_Triangulation) triMesh = BRep_Tool::Triangulation(aFace, aLoc);
            if (triMesh)
            {
                TColgp_Array1OfPnt aTriNodes(1, triMesh->NbNodes());
                aTriNodes = triMesh->Nodes();
                Poly_Array1OfTriangle aTriangles(1, triMesh->NbTriangles());
                aTriangles = triMesh->Triangles();
                qDebug() << "Face" << index << "has" << triMesh->NbTriangles() << "triangles";
                count += triMesh->NbTriangles();
                for (int i = 1; i <= triMesh->NbTriangles(); i++)
                {
                    Poly_Triangle trian = aTriangles.Value(i);
                    Standard_Integer index1, index2, index3;
                    trian.Get(index1, index2, index3);

                    gp_Pnt pnt1, pnt2, pnt3;
                    pnt1 = aTriNodes[index1];
                    pnt2 = aTriNodes[index2];
                    pnt3 = aTriNodes[index3];

                    if (pnt1.IsEqual(pnt2, 0.01) || pnt1.IsEqual(pnt3, 0.01) || pnt2.IsEqual(pnt3, 0.01))
                        continue;
                    vector<gp_Pnt> triPnt;
                    triPnt.push_back(pnt1);
                    triPnt.push_back(pnt2);
                    triPnt.push_back(pnt3);
                    Hand::displayTriangle(myQccView, triPnt);
                }
            }
        }
        qDebug() << "Total mesh triangles are:" << count << "\n";
#endif
    }
}

void Qcc::obbShape()
{
    if (myQccView->getContext()->HasDetectedShape())
    {
        Handle(AIS_InteractiveContext) aisContext = myQccView->getContext();
        if (!aisContext)
            return;

        TopoDS_Shape topoShp = aisContext->DetectedShape();
        if (topoShp.IsNull()) 
            return;
            
#if 0
        /* delete original TopoDS_Shape AIS_InteractiveObject */
        Handle(AIS_InteractiveObject) aisObj = myQccView->getContext()->DetectedInteractive();
        myQccView->getContext()->Erase(aisObj, Standard_True);
        /* display triangle face by face from Obb */
        Obb obbShp(topoShp);
        obbShp.displayObb(myQccView);

#elif 1
        /* create bndOBB for selected shape */
        BRepBndLib ret;
        Bnd_OBB obbShape;
        ret.AddOBB(topoShp, obbShape, true, true, false);

        /* convert BndOBB to TopoShape for display */
        TopoDS_Shape topoObb = Hand::getBndShape(obbShape);
        Handle(AIS_Shape) abox = new AIS_Shape(topoObb);
        abox->SetColor(Quantity_NOC_GREEN);
        abox->SetTransparency(0.9);
        myQccView->getContext()->Display(abox, Standard_True);
#endif
    }
}

void Qcc::load()
{
    QString t_occ = "*.stp *.step";
    QString all_filter;
    all_filter += t_occ;

    // get open file's path
    QString filename = QFileDialog::getOpenFileName(this, tr("Load File"), "D:/model.stp", all_filter);
    if (filename.isEmpty())  // 若文件名为空，则不执行操作
    {
        return;
    }
    else
    {
        filename = filename.toLower();
    }

    STEPControl_Reader stepReader;  //only load English filename
    IFSelect_ReturnStatus status = stepReader.ReadFile(filename.toLatin1().data());
    stepReader.PrintCheckLoad(Standard_False, IFSelect_ItemsByEntity);
    for (Standard_Integer i = 1; i <= stepReader.NbRootsForTransfer(); i++)
        stepReader.TransferRoot(i);
    
    for (Standard_Integer i = 1; i <= stepReader.NbShapes(); i++)
    {
        TopoDS_Shape stepShp = stepReader.Shape(i);
        myQccView->show(stepShp);
    }
    return;
}

void Qcc::save()
{

}

void Qcc::makeBox()
{
    TopoDS_Shape aTopoBox = BRepPrimAPI_MakeBox(30.0, 40.0, 50.0).Shape();
    Handle(AIS_Shape) anAisBox = new AIS_Shape(aTopoBox);
    anAisBox->SetColor(Quantity_NOC_AZURE);
    myQccView->getContext()->Display(anAisBox, Standard_True);
}

void Qcc::makeCone()
{
    gp_Ax2 anAxis;
    anAxis.SetLocation(gp_Pnt(0.0, 10.0, 0.0));

    TopoDS_Shape aTopoReducer = BRepPrimAPI_MakeCone(anAxis, 3.0, 1.5, 5.0).Shape();
    Handle(AIS_Shape) anAisReducer = new AIS_Shape(aTopoReducer);

    anAisReducer->SetColor(Quantity_NOC_BISQUE);
    myQccView->getContext()->Display(anAisReducer, Standard_True);

    anAxis.SetLocation(gp_Pnt(8.0, 10.0, 0.0));
    TopoDS_Shape aTopoCone = BRepPrimAPI_MakeCone(anAxis, 3.0, 0.0, 5.0).Shape();
    Handle(AIS_Shape) anAisCone = new AIS_Shape(aTopoCone);

    anAisCone->SetColor(Quantity_NOC_CHOCOLATE);
    myQccView->getContext()->Display(anAisCone, Standard_True);
}

void Qcc::makeSphere()
{
    gp_Ax2 anAxis;
    anAxis.SetLocation(gp_Pnt(6.0, 6.0, 30));

    TopoDS_Shape aTopoSphere = BRepPrimAPI_MakeSphere(anAxis, 2.5).Shape();
    Handle(AIS_Shape) anAisShpere = new AIS_Shape(aTopoSphere);

    anAisShpere->SetColor(Quantity_NOC_BLANCHEDALMOND);
    myQccView->getContext()->Display(anAisShpere, Standard_True);
}

void Qcc::makeCylinder()
{
    gp_Ax2 anAxis;
    anAxis.SetLocation(gp_Pnt(0.0, 30.0, 0.0));

    TopoDS_Shape aTopoCylinder = BRepPrimAPI_MakeCylinder(anAxis, 3.0, 5.0).Shape();
    Handle(AIS_Shape) anAisCylinder = new AIS_Shape(aTopoCylinder);

    anAisCylinder->SetColor(Quantity_NOC_RED);

    anAxis.SetLocation(gp_Pnt(15.0, 30.0, 0.0));
    TopoDS_Shape aTopoPie = BRepPrimAPI_MakeCylinder(anAxis, 6.0, 5.0, M_PI_2 * 3.0).Shape();
    Handle(AIS_Shape) anAisPie = new AIS_Shape(aTopoPie);

    anAisPie->SetColor(Quantity_NOC_TAN);

    myQccView->getContext()->Display(anAisCylinder, Standard_True);
    myQccView->getContext()->Display(anAisPie, Standard_True);
}

void Qcc::makeTorus()
{
    gp_Ax2 anAxis;
    anAxis.SetLocation(gp_Pnt(0.0, 40.0, 0.0));

    TopoDS_Shape aTopoTorus = BRepPrimAPI_MakeTorus(anAxis, 3.0, 1.0).Shape();
    Handle(AIS_Shape) anAisTorus = new AIS_Shape(aTopoTorus);

    anAisTorus->SetColor(Quantity_NOC_YELLOW);

    anAxis.SetLocation(gp_Pnt(8.0, 40.0, 0.0));
    TopoDS_Shape aTopoElbow = BRepPrimAPI_MakeTorus(anAxis, 3.0, 1.0, M_PI_2).Shape();
    Handle(AIS_Shape) anAisElbow = new AIS_Shape(aTopoElbow);

    anAisElbow->SetColor(Quantity_NOC_THISTLE);

    myQccView->getContext()->Display(anAisTorus, Standard_True);
    myQccView->getContext()->Display(anAisElbow, Standard_True);
}

void Qcc::makeWedge()
{
    Standard_Real dx = 12.0;
    Standard_Real dy = 8.0;
    Standard_Real dz = 1.5;
    Standard_Real xmin = 3.0;
    Standard_Real zmin = 0.0;
    Standard_Real xmax = 9.0;
    Standard_Real zmax = 1.5;
    Standard_Real ltx = 4.0;

    /* First Wedge method: anAx2, dx, dy, dz, ltx */
    gp_Ax2 anAx2;
    anAx2.SetLocation(gp_Pnt(12.0, 12.0, 0.0));
    TopoDS_Shape aTopoWedge1 = BRepPrimAPI_MakeWedge(anAx2, dx, dy, dz, ltx).Shape();
    Handle(AIS_Shape) anAisWedge1 = new AIS_Shape(aTopoWedge1);
    anAisWedge1->SetColor(Quantity_NOC_YELLOW);
    myQccView->getContext()->Display(anAisWedge1, Standard_True);

    /* Second Wedge method: anAx2, dx, dy, dz, xmin, xmax, zmix, zmax */
    anAx2.SetLocation(gp_Pnt(0.0, 0.0, 0.0));
    TopoDS_Shape aTopoWedge2 = BRepPrimAPI_MakeWedge(anAx2, dx, dy, dz, xmin, zmin, xmax, zmax);
   
    gp_Trsf aTrsf1, aTrsf2; // rotation and translation method
    gp_Ax1 anAx1(gp_Pnt(0.0, 0.0, dz), gp_Dir(1.0, 0.0, 0.0));
    aTrsf1.SetRotation(anAx1, M_PI_2);
    aTrsf2.SetTranslation(gp_Vec(0.0, 5.5, 0.0));

    /* overloaded '*' that transformation matrix to multiply */
    BRepBuilderAPI_Transform aTransform1(aTopoWedge2, aTrsf2*aTrsf1);
    TopoDS_Shape sheet1 = aTransform1.Shape();

    gp_Trsf aTrsf3; // rotation sheet1 to get sheet2
    gp_Ax1 anAx3(gp_Pnt(6.0, 6.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
    aTrsf3.SetRotation(anAx3, M_PI_2);
    BRepBuilderAPI_Transform aTransform2(sheet1, aTrsf3);
    TopoDS_Shape sheet2 = aTransform2.Shape();

    /* display TopoDS_Shape sheet1, sheet2 */
    Handle(AIS_Shape) anAisSheet1 = new AIS_Shape(sheet1);
    Handle(AIS_Shape) anAisSheet2 = new AIS_Shape(sheet2);
    anAisSheet1->SetColor(Quantity_NOC_YELLOW1);
    anAisSheet2->SetColor(Quantity_NOC_YELLOW2);
    myQccView->getContext()->Display(anAisSheet1, Standard_True);
    myQccView->getContext()->Display(anAisSheet2, Standard_True);
}

void Qcc::makeHollow()
{
    gp_Ax1 anAxis;
    anAxis.SetLocation(gp_Pnt(6.0, 6.0, 1.5));
    Standard_Real ra = 1.0;
    Standard_Real rb = 2.0;

    // revol a vertex result is a circle edge.
    TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(ra+6.0, 6.0, 1.5));
    TopoDS_Shape aRevolVertex = BRepPrimAPI_MakeRevol(aVertex, anAxis);
    Handle(AIS_Shape) aAisRevolVertex = new AIS_Shape(aRevolVertex);

    // revol a vertex with M_PI result in half circle edge
    TopoDS_Vertex bVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(rb+6.0, 6.0, 1.5));
    TopoDS_Shape bRevolVertex = BRepPrimAPI_MakeRevol(bVertex, anAxis, M_PI);
    Handle(AIS_Shape) bAisRevolVertex = new AIS_Shape(bRevolVertex);

    aAisRevolVertex->SetColor(Quantity_NOC_LIMEGREEN);
    bAisRevolVertex->SetColor(Quantity_NOC_LIMEGREEN);
    myQccView->getContext()->Display(aAisRevolVertex, Standard_True);
    myQccView->getContext()->Display(bAisRevolVertex, Standard_True);

    // prism a vertex result is an straight edge
    TopoDS_Vertex cVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(6.0, 6.0+ra, 1.5));
    TopoDS_Shape aPrismVertex = BRepPrimAPI_MakePrism(cVertex, gp_Vec(0.0, rb - ra, 0.0));
    Handle(AIS_Shape) anAisPrismVertex = new AIS_Shape(aPrismVertex);
    
    anAisPrismVertex->SetColor(Quantity_NOC_LIMEGREEN);
    myQccView->getContext()->Display(anAisPrismVertex, Standard_True);

    // revol an edge result is a circle face and extrude it to a solid
    TopoDS_Edge anRadius = BRepBuilderAPI_MakeEdge(gp_Pnt(ra + 6.0, 6.0, 1.5), gp_Pnt(rb + 6.0, 6.0, 1.5));
    TopoDS_Shape revolRadius = BRepPrimAPI_MakeRevol(anRadius, anAxis);
    TopoDS_Face revolFace = TopoDS::Face(revolRadius);
    TopoDS_Shape extrudeSolid = BRepPrimAPI_MakePrism(revolFace, gp_Vec(0.0, 0.0, 8.0));

    Handle(AIS_Shape) anAisRadius = new AIS_Shape(revolRadius);
    Handle(AIS_Shape) anAisSolid = new AIS_Shape(extrudeSolid);
    anAisRadius->SetColor(Quantity_NOC_LIMEGREEN);
    anAisSolid->SetColor(Quantity_NOC_LIMEGREEN);
    myQccView->getContext()->Display(anAisRadius, Standard_True);
    myQccView->getContext()->Display(anAisSolid, Standard_True);
}

void Qcc::makeFillet()
{
    gp_Ax2 anAxis;
    anAxis.SetLocation(gp_Pnt(0.0, 50.0, 0.0));

    TopoDS_Shape aTopoBox = BRepPrimAPI_MakeBox(anAxis, 3.0, 4.0, 5.0).Shape();
    BRepFilletAPI_MakeFillet MF(aTopoBox);

    for (TopExp_Explorer ex(aTopoBox, TopAbs_EDGE); ex.More(); ex.Next())
    {
        MF.Add(1.0, TopoDS::Edge(ex.Current()));
    }

    Handle(AIS_Shape) anAisShape = new AIS_Shape(MF.Shape());
    anAisShape->SetColor(Quantity_NOC_VIOLET);
    myQccView->getContext()->Display(anAisShape, Standard_True);
}

void Qcc::makeChamfer()
{
    gp_Ax2 anAxis;
    anAxis.SetLocation(gp_Pnt(8.0, 5.0, 0.0));

    TopoDS_Shape aTopoBox = BRepPrimAPI_MakeBox(anAxis, 3.0, 4.0, 5.0).Shape();
    BRepFilletAPI_MakeChamfer MC(aTopoBox);
    
    TopTools_IndexedDataMapOfShapeListOfShape aEdgeFaceMap;
    TopExp::MapShapesAndAncestors(aTopoBox, TopAbs_EDGE, TopAbs_FACE, aEdgeFaceMap);

    for (Standard_Integer i = 1; i <= aEdgeFaceMap.Extent(); ++i)
    {
        TopoDS_Edge anEdge = TopoDS::Edge(aEdgeFaceMap.FindKey(i));
        TopoDS_Face aFace = TopoDS::Face(aEdgeFaceMap.FindFromIndex(i).First());
        MC.Add(0.6, 0.6, anEdge, aFace);
    }

    Handle(AIS_Shape) anAisShape = new AIS_Shape(MC.Shape());
    anAisShape->SetColor(Quantity_NOC_TOMATO);
    myQccView->getContext()->Display(anAisShape, Standard_True);
}

void Qcc::makeExtrude()
{
    // prism a vertex result is an edge.
    TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(0.0, 60.0, 0.0));
    TopoDS_Shape aPrismVertex = BRepPrimAPI_MakePrism(aVertex, gp_Vec(0.0, 0.0, 5.0));
    Handle(AIS_Shape) anAisPrismVertex = new AIS_Shape(aPrismVertex);

    // prism an edge result is a face.
    TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(5.0, 60.0, 0.0), gp_Pnt(10.0, 60.0, 0.0));
    TopoDS_Shape aPrismEdge = BRepPrimAPI_MakePrism(anEdge, gp_Vec(0.0, 0.0, 5.0));
    Handle(AIS_Shape) anAisPrismEdge = new AIS_Shape(aPrismEdge);

    // prism a wire result is a shell.
    gp_Ax2 anAxis;
    anAxis.SetLocation(gp_Pnt(16.0, 60.0, 0.0));

    TopoDS_Edge aCircleEdge = BRepBuilderAPI_MakeEdge(gp_Circ(anAxis, 3.0));
    TopoDS_Wire aCircleWire = BRepBuilderAPI_MakeWire(aCircleEdge);
    TopoDS_Shape aPrismCircle = BRepPrimAPI_MakePrism(aCircleWire, gp_Vec(0.0, 0.0, 5.0));
    Handle(AIS_Shape) anAisPrismCircle = new AIS_Shape(aPrismCircle);

    // prism a face or a shell result is a solid.
    anAxis.SetLocation(gp_Pnt(24.0, 60.0, 0.0));
    TopoDS_Edge aEllipseEdge = BRepBuilderAPI_MakeEdge(gp_Elips(anAxis, 3.0, 2.0));
    TopoDS_Wire aEllipseWire = BRepBuilderAPI_MakeWire(aEllipseEdge);
    TopoDS_Face aEllipseFace = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), aEllipseWire);
    TopoDS_Shape aPrismEllipse = BRepPrimAPI_MakePrism(aEllipseFace, gp_Vec(0.0, 0.0, 5.0));
    Handle(AIS_Shape) anAisPrismEllipse = new AIS_Shape(aPrismEllipse);

    anAisPrismVertex->SetColor(Quantity_NOC_PAPAYAWHIP);
    anAisPrismEdge->SetColor(Quantity_NOC_PEACHPUFF);
    anAisPrismCircle->SetColor(Quantity_NOC_PERU);
    anAisPrismEllipse->SetColor(Quantity_NOC_PINK);

    myQccView->getContext()->Display(anAisPrismVertex, Standard_True);
    myQccView->getContext()->Display(anAisPrismEdge, Standard_True);
    myQccView->getContext()->Display(anAisPrismCircle, Standard_True);
    myQccView->getContext()->Display(anAisPrismEllipse, Standard_True);
}

void Qcc::makeRevol()
{
    gp_Ax1 anAxis;

    // revol a vertex result is an edge.
    anAxis.SetLocation(gp_Pnt(0.0, 70.0, 0.0));
    TopoDS_Vertex aVertex = BRepBuilderAPI_MakeVertex(gp_Pnt(2.0, 70.0, 0.0));
    TopoDS_Shape aRevolVertex = BRepPrimAPI_MakeRevol(aVertex, anAxis);
    Handle(AIS_Shape) anAisRevolVertex = new AIS_Shape(aRevolVertex);

    // revol an edge result is a face.
    anAxis.SetLocation(gp_Pnt(8.0, 70.0, 0.0));
    TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(gp_Pnt(6.0, 70.0, 0.0), gp_Pnt(6.0, 70.0, 5.0));
    TopoDS_Shape aRevolEdge = BRepPrimAPI_MakeRevol(anEdge, anAxis);
    Handle(AIS_Shape) anAisRevolEdge = new AIS_Shape(aRevolEdge);

    // revol a wire result is a shell.
    anAxis.SetLocation(gp_Pnt(20.0, 70.0, 0.0));
    anAxis.SetDirection(gp::DY());

    TopoDS_Edge aCircleEdge = BRepBuilderAPI_MakeEdge(gp_Circ(gp_Ax2(gp_Pnt(15.0, 70.0, 0.0), gp::DZ()), 1.5));
    TopoDS_Wire aCircleWire = BRepBuilderAPI_MakeWire(aCircleEdge);
    TopoDS_Shape aRevolCircle = BRepPrimAPI_MakeRevol(aCircleWire, anAxis, M_PI_2);
    Handle(AIS_Shape) anAisRevolCircle = new AIS_Shape(aRevolCircle);

    // revol a face result is a solid.
    anAxis.SetLocation(gp_Pnt(30.0, 70.0, 0.0));
    anAxis.SetDirection(gp::DY());

    TopoDS_Edge aEllipseEdge = BRepBuilderAPI_MakeEdge(gp_Elips(gp_Ax2(gp_Pnt(25.0, 70.0, 0.0), gp::DZ()), 3.0, 2.0));
    TopoDS_Wire aEllipseWire = BRepBuilderAPI_MakeWire(aEllipseEdge);
    TopoDS_Face aEllipseFace = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), aEllipseWire);
    TopoDS_Shape aRevolEllipse = BRepPrimAPI_MakeRevol(aEllipseFace, anAxis, M_PI_4);
    Handle(AIS_Shape) anAisRevolEllipse = new AIS_Shape(aRevolEllipse);

    anAisRevolVertex->SetColor(Quantity_NOC_LIMEGREEN);
    anAisRevolEdge->SetColor(Quantity_NOC_LINEN);
    anAisRevolCircle->SetColor(Quantity_NOC_MAGENTA1);
    anAisRevolEllipse->SetColor(Quantity_NOC_MAROON);

    myQccView->getContext()->Display(anAisRevolVertex, Standard_True);
    myQccView->getContext()->Display(anAisRevolEdge, Standard_True);
    myQccView->getContext()->Display(anAisRevolCircle, Standard_True);
    myQccView->getContext()->Display(anAisRevolEllipse, Standard_True);
}

void Qcc::makeLoft()
{
    // bottom wire.
    TopoDS_Edge aCircleEdge = BRepBuilderAPI_MakeEdge(gp_Circ(gp_Ax2(gp_Pnt(0.0, 80.0, 0.0), gp::DZ()), 1.5));
    TopoDS_Wire aCircleWire = BRepBuilderAPI_MakeWire(aCircleEdge);

    // top wire.
    BRepBuilderAPI_MakePolygon aPolygon;
    aPolygon.Add(gp_Pnt(-3.0, 77.0, 6.0));
    aPolygon.Add(gp_Pnt(3.0, 77.0, 6.0));
    aPolygon.Add(gp_Pnt(3.0, 83.0, 6.0));
    aPolygon.Add(gp_Pnt(-3.0, 83.0, 6.0));
    aPolygon.Close();

    BRepOffsetAPI_ThruSections aShellGenerator;
    BRepOffsetAPI_ThruSections aSolidGenerator(true);

    aShellGenerator.AddWire(aCircleWire);
    aShellGenerator.AddWire(aPolygon.Wire());

    aSolidGenerator.AddWire(aCircleWire);
    aSolidGenerator.AddWire(aPolygon.Wire());

    // translate the solid.
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(18.0, 0.0, 0.0));
    BRepBuilderAPI_Transform aTransform(aSolidGenerator.Shape(), aTrsf);

    Handle(AIS_Shape) anAisShell = new AIS_Shape(aShellGenerator.Shape());
    Handle(AIS_Shape) anAisSolid = new AIS_Shape(aTransform.Shape());

    anAisShell->SetColor(Quantity_NOC_OLIVEDRAB);
    anAisSolid->SetColor(Quantity_NOC_PEACHPUFF);

    myQccView->getContext()->Display(anAisShell, Standard_True);
    myQccView->getContext()->Display(anAisSolid, Standard_True);
}

void Qcc::testCut()
{
    Standard_Real gap = 2.0;
    Standard_Real radius = 1.0;
    Standard_Real width = 12.0;
    Standard_Real height = 3.0;
    
    gp_Ax2 anAx2;
    anAx2.SetLocation(gp_Pnt(gap, gap, 0.0));
    TopoDS_Shape aTopoCylinder = BRepPrimAPI_MakeCylinder(anAx2, radius, height*2.0).Shape();
   
    anAx2.SetLocation(gp_Pnt(0.0, 0.0, 0.0));
    TopoDS_Shape aTopoBox = BRepPrimAPI_MakeBox(anAx2, width, width, height).Shape();
    
    Handle(AIS_Shape) anAisCylinder = new AIS_Shape(aTopoCylinder);
    Handle(AIS_Shape) anAisBox = new AIS_Shape(aTopoBox);
    anAisCylinder->SetColor(Quantity_NOC_BEIGE);
    anAisBox->SetColor(Quantity_NOC_BEET);
    myQccView->getContext()->Display(anAisCylinder, Standard_True);
    myQccView->getContext()->Display(anAisBox, Standard_True);

    gp_Trsf aTrsf;
    TopoDS_Shape aTopoHole;
    /* Box cut Cylinder at (gap, gap, 0) */
    aTopoHole = BRepAlgoAPI_Cut(aTopoBox, aTopoCylinder);

    /* Box cut Cylinder at (width - gap, gap, 0)*/
    aTrsf.SetTranslation(gp_Vec(width - 2 * gap, 0.0, 0.0));
    BRepBuilderAPI_Transform aBRepTrsf1(aTopoCylinder, aTrsf);
    aTopoHole = BRepAlgoAPI_Cut(aTopoHole, aBRepTrsf1.Shape());

    /* Box cut Cylinder at (width - gap, width - gap, 0) */
    aTrsf.SetTranslation(gp_Vec(width - 2 * gap, width - 2 * gap, 0.0));
    BRepBuilderAPI_Transform aBRepTrsf2(aTopoCylinder, aTrsf);
    aTopoHole = BRepAlgoAPI_Cut(aTopoHole, aBRepTrsf2.Shape());

    /* Box cut Cylinder at (gap, width - gap, 0) */
    aTrsf.SetTranslation(gp_Vec(0.0, width - 2 * gap, 0.0));
    BRepBuilderAPI_Transform aBRepTrsf3(aTopoCylinder, aTrsf);
    aTopoHole = BRepAlgoAPI_Cut(aTopoHole, aBRepTrsf3.Shape());

    aTrsf.SetTranslation(gp_Vec(width * 1.5, 0.0, 0.0));
    BRepBuilderAPI_Transform aBRepTrsfHole(aTopoHole, aTrsf);
    Handle(AIS_Shape) anAisHole = new AIS_Shape(aBRepTrsfHole.Shape());
    anAisHole->SetColor(Quantity_NOC_CHOCOLATE);
    myQccView->getContext()->Display(anAisHole, Standard_True);
}

void Qcc::testHelix()
{
    makeCylindericalHelix();
}

void Qcc::makeCylindericalHelix()
{
    Standard_Real aRadius = 3.0;
    Standard_Real aPitch = 1.0;

    // the pcurve is a 2d line in the parametric space.
    // gp_Dir2d is normalized a gp_Vec2d()
    gp_Lin2d aLine2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(aRadius, aPitch));
    
    Handle(Geom2d_TrimmedCurve) aSegment = GCE2d_MakeSegment(aLine2d, 0.0, M_PI * 2.0).Value();

    Handle(Geom_CylindricalSurface) aCylinder = new Geom_CylindricalSurface(gp::XOY(), aRadius);

    TopoDS_Edge aHelixEdge = BRepBuilderAPI_MakeEdge(aSegment, aCylinder, 0.0, 6.0 * M_PI).Edge();

    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(0.0, 12.0, 0.0));
    BRepBuilderAPI_Transform aTransform(aHelixEdge, aTrsf);

    Handle(AIS_Shape) anAisHelixCurve = new AIS_Shape(aTransform.Shape());
    Handle(AIS_Shape) anAisHelixEdge = new AIS_Shape(aHelixEdge);

    myQccView->getContext()->Display(anAisHelixCurve, Standard_True);
    myQccView->getContext()->Display(anAisHelixEdge, Standard_True);

    // sweep a circle profile along the helix curve.
    // there is no curve3d in the pcurve edge, so approx one.
    BRepLib::BuildCurve3d(aHelixEdge);

    gp_Ax2 anAxis;
    anAxis.SetDirection(gp_Dir(0.0, 4.0, 1.0));
    anAxis.SetLocation(gp_Pnt(aRadius, 0.0, 0.0));

    gp_Circ aProfileCircle(anAxis, 0.3);

    TopoDS_Edge aProfileEdge = BRepBuilderAPI_MakeEdge(aProfileCircle).Edge();
    TopoDS_Wire aProfileWire = BRepBuilderAPI_MakeWire(aProfileEdge).Wire();
    TopoDS_Face aProfileFace = BRepBuilderAPI_MakeFace(aProfileWire).Face();

    TopoDS_Wire aHelixWire = BRepBuilderAPI_MakeWire(aHelixEdge).Wire();

    BRepOffsetAPI_MakePipe aPipeMaker(aHelixWire, aProfileFace);

    if (aPipeMaker.IsDone())
    {
        aTrsf.SetTranslation(gp_Vec(8.0, 120.0, 0.0));
        BRepBuilderAPI_Transform aPipeTransform(aPipeMaker.Shape(), aTrsf);

        Handle(AIS_Shape) anAisPipe = new AIS_Shape(aPipeTransform.Shape());
        anAisPipe->SetColor(Quantity_NOC_CORAL);
        myQccView->getContext()->Display(anAisPipe, Standard_True);
    }
}

void Qcc::makeFaceHole()
{
    Standard_Real gap = 2.0;
    Standard_Real radius = 1.0;
    Standard_Real width = 12.0;
    Standard_Real height = 1.5;

    gp_Pln aPlane;

    gp_Circ aCircle1(gp::XOY(), radius);
    gp_Circ aCircle2(gp::XOY(), radius);
    gp_Circ aCircle3(gp::XOY(), radius);
    gp_Circ aCircle4(gp::XOY(), radius);

    aCircle1.SetLocation(gp_Pnt(gap, gap, 0.0));
    aCircle2.SetLocation(gp_Pnt(width - gap, gap, 0.0));
    aCircle3.SetLocation(gp_Pnt(gap, width - gap, 0.0));
    aCircle4.SetLocation(gp_Pnt(width - gap, width - gap, 0.0));

    BRepBuilderAPI_MakeEdge anEdgeMaker1(aCircle1);
    BRepBuilderAPI_MakeEdge anEdgeMaker2(aCircle2);
    BRepBuilderAPI_MakeEdge anEdgeMaker3(aCircle3);
    BRepBuilderAPI_MakeEdge anEdgeMaker4(aCircle4);

    BRepBuilderAPI_MakeWire aWireMaker1(anEdgeMaker1.Edge());
    BRepBuilderAPI_MakeWire aWireMaker2(anEdgeMaker2.Edge());
    BRepBuilderAPI_MakeWire aWireMaker3(anEdgeMaker3.Edge());
    BRepBuilderAPI_MakeWire aWireMaker4(anEdgeMaker4.Edge());

    BRepBuilderAPI_MakeFace aFaceMaker(aPlane, 0.0, width, 0.0, width);

    if (aWireMaker1.IsDone()) {
        TopoDS_Wire aWire1 = aWireMaker1.Wire();
        aWire1.Reverse();
        aFaceMaker.Add(aWire1);
    }

    if (aWireMaker2.IsDone()) {
        TopoDS_Wire aWire2 = aWireMaker2.Wire();
        aWire2.Reverse();
        aFaceMaker.Add(aWire2);
    }

    if (aWireMaker3.IsDone()) {
        TopoDS_Wire aWire3 = aWireMaker3.Wire();
        aWire3.Reverse();
        aFaceMaker.Add(aWire3);
    }

    if (aWireMaker4.IsDone()) {
        TopoDS_Wire aWire4 = aWireMaker4.Wire();
        aWire4.Reverse();
        aFaceMaker.Add(aWire4);
    }

    if (aFaceMaker.IsDone()) {
        TopoDS_Shape aTopoFace = aFaceMaker.Shape();
        // BRepTools::Write(aTopoFace, "D:/face.brep");
        Handle(AIS_Shape) anAisFace = new AIS_Shape(aTopoFace);
        myQccView->getContext()->Display(anAisFace, Standard_True);
    }

    TopoDS_Shape aTopoHole = BRepPrimAPI_MakePrism(aFaceMaker.Face(), gp_Vec(0.0, 0.0, height));
    Handle(AIS_Shape) anAisHole = new AIS_Shape(aTopoHole);
    myQccView->getContext()->Display(anAisHole, Standard_True);
}

void Qcc::deleteShape()
{
    if (myQccView->getContext()->HasDetectedShape())
    {
        Handle(AIS_InteractiveObject) aisObj = myQccView->getContext()->DetectedInteractive();
        myQccView->getContext()->Erase(aisObj, Standard_True);
    }
}