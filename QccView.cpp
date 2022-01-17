#include "Qcc.h"
#include "QccView.h"
#include <OpenGl_GraphicDriver.hxx>

#include <QMenu>
#include <QMouseEvent>
#include <QRubberBand>
#include <QStyleFactory>

#include <Aspect_Handle.hxx>
#include <Aspect_DisplayConnection.hxx>

QccView::QccView(QWidget* parent)
	: QGLWidget(parent),
	myXmin(0),
	myYmin(0),
	myXmax(0),
	myYmax(0),
	myCurrentMode(CurrentAction3d::CurAction3d_DynamicRotation),
	myDegenerateModeIsOn(Standard_True),
	myRectBand(NULL),
	myManipulator(NULL)
{
	setBackgroundRole(QPalette::NoRole);
	/* set focus policy to threat QContextMenuEvent from keyboard */
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	/* enable the mouse tracking, by default the mouse tracking is disable */
	setMouseTracking(true);

	initContext();
	mySelectMode = -1;	//default mode
	myManipulator = new AIS_Manipulator();
}

void QccView::initContext() 
{
	if (myContext.IsNull())
	{
		Handle(Aspect_DisplayConnection) aDisplayConnection = new Aspect_DisplayConnection();
		
		if (myDriver.IsNull())
			myDriver = new OpenGl_GraphicDriver(aDisplayConnection);

		WId window_handle = (WId)winId();

#ifdef _WIN32
		Handle(WNT_Window) wind = new WNT_Window((Aspect_Handle)window_handle);
#else
		Handle(Xw_Window) wind = new Xw_Window(aDisplayConnection, (Window)window_handle);
#endif

		myViewer = new V3d_Viewer(myDriver);
		myContext = new AIS_InteractiveContext(myViewer);

		myViewer->SetDefaultLights();
		myViewer->SetLightOn();

		myContext->SetDisplayMode(AIS_Shaded, Standard_True);

		myView = myViewer->CreateView();
		myView->SetWindow(wind);
		if (!wind->IsMapped())
		{
			wind->Map();
		}

		myView->SetBackgroundColor(Quantity_NOC_BLACK);
		myView->MustBeResized();
		myView->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_GOLD, 0.08, V3d_ZBUFFER);
	}

	myContext->SetPixelTolerance(1);

	glbContext = myContext;
}

const Handle(AIS_InteractiveContext)& QccView::getContext() const
{
	return myContext;
}

const Standard_Integer QccView::getSelectMode() const
{
	return mySelectMode;
}

QPaintEngine* QccView::paintEngine() const
{
	return 0;
}

void QccView::paintEvent(QPaintEvent* /*theEvent*/)
{
	myView->Redraw();
}

void QccView::resizeEvent(QResizeEvent* /*theEvent*/)
{
	if (!myView.IsNull())
		myView->MustBeResized();
}

void QccView::show(const TopoDS_Shape& shp)
{
	Handle(AIS_Shape) aisShp = new AIS_Shape(shp);
	this->myContext->Display(aisShp, Standard_True);
}

void QccView::zoom(void)
{
	myCurrentMode = CurrentAction3d::CurAction3d_DynamicZooming;
}

void QccView::pan(void)
{
	myCurrentMode = CurrentAction3d::CurAction3d_DynamicPanning;
}

void QccView::rotate(void)
{
	myCurrentMode = CurrentAction3d::CurAction3d_DynamicRotation;
}

void QccView::reset(void)
{
	myView->Reset();
}

void QccView::fitAll(void)
{
	myView->FitAll();
	myView->ZFitAll();
	//myView->Redraw();
}

void QccView::redraw(void)
{
	myView->Redraw();
}

void QccView::initManipulator()
{
	if (myContext->HasDetectedShape())
	{
		Handle(AIS_InteractiveObject) aisObj = myContext->DetectedInteractive();
		if (myManipulator->IsAttached()) 
		{
			myManipulator->Detach();
			myContext->Erase(myManipulator, Standard_True);
		}
		else
		{
			// 可以用 SetPart 禁用或启用某些轴的平移、旋转或缩放的可视部分
			myManipulator->SetPart(0, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);  // 禁用了 X 轴的缩放
			myManipulator->SetPart(1, AIS_ManipulatorMode::AIS_MM_Rotation, Standard_False); // 禁用了 Y 轴的旋转
			// 将操纵器附在创建的Shape上
			myManipulator->Attach(aisObj);
			// 启用指定的操纵模式
			myManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Translation);  // 启用移动
			myManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Rotation);     // 启用旋转
			myManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Scaling);      // 启用缩放
			// 激活操纵器
			myManipulator->SetModeActivationOnDetection(Standard_True);

			myContext->UpdateCurrentViewer();
		}
	}
}

void QccView::mousePressEvent(QMouseEvent* theEvent)
{
	if (theEvent->button() == Qt::LeftButton)
	{
		onLButtonDown(theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
	}
	else if (theEvent->button() == Qt::MidButton)
	{
		onMButtonDown(theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
	}
	else if (theEvent->button() == Qt::RightButton)
	{
		onRButtonDown(theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
	}
}

void QccView::mouseReleaseEvent(QMouseEvent* theEvent)
{
	if (theEvent->button() == Qt::LeftButton)
	{
		onLButtonUp(theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
	}
	else if (theEvent->button() == Qt::MidButton)
	{
		onMButtonUp(theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
	}
	else if (theEvent->button() == Qt::RightButton)
	{
		onRButtonUp(theEvent->buttons() | theEvent->modifiers(), theEvent->pos());
	}
}

void QccView::mouseMoveEvent(QMouseEvent* theEvent)
{
	onMouseMove(theEvent->buttons(), theEvent->pos());
}

void QccView::wheelEvent(QWheelEvent* theEvent)
{
	onMouseWheel(theEvent->buttons(), theEvent->delta(), theEvent->pos());
}

void QccView::keyPressEvent(QKeyEvent* theEvent)
{
	if (theEvent->modifiers() == Qt::ShiftModifier)
	{
		myCurrentMode = CurrentAction3d::CurAction3d_DynamicPanning;
	}
	else if (theEvent->modifiers() == Qt::AltModifier)
	{
		myCurrentMode = CurrentAction3d::CurAction3d_DynamicZooming;
	}
}

void QccView::keyReleaseEvent(QKeyEvent* theEvent)
{
	myCurrentMode = CurrentAction3d::CurAction3d_DynamicRotation;
	myManipulator->DeactivateCurrentMode();
}

void QccView::onLButtonDown(const int theFlags, const QPoint thePoint)
{
	/* save the current mouse coordinate in min */
	myXmin = thePoint.x();
	myYmin = thePoint.y();
	myXmax = thePoint.x();
	myYmax = thePoint.y();

	if (theFlags & Qt::ControlModifier)
	{
		if (myManipulator->HasActiveMode())
		{
			myCurrentMode = CurrentAction3d::CurAction3d_Manipulating;
			myManipulator->StartTransform(thePoint.x(), thePoint.y(), myView);
		}
	}
}

void QccView::onRButtonDown(const int theFlags, const QPoint /*thePoint*/)
{
	QMenu menu;
	if (theFlags & Qt::ControlModifier)
	{
		QAction* actionShape = menu.addAction("Select Shape");
		QAction* actionSolid = menu.addAction("Select Solid");
		//QAction* actionShell = menu.addAction("Select Shell");
		QAction* actionFace = menu.addAction("Select Face");
		//QAction* actionWire = menu.addAction("Select Wire");
		QAction* actionEdge = menu.addAction("Select Edge");
		QAction* actionVertex = menu.addAction("Select Vertex");
		connect(actionShape, &QAction::triggered, this, &QccView::selectShape);
		connect(actionSolid, &QAction::triggered, this, &QccView::selectSolid);
		//connect(actionShell, &QAction::triggered, this, &QccView::selectShell);
		connect(actionFace, &QAction::triggered, this, &QccView::selectFace);
		//connect(actionWire, &QAction::triggered, this, &QccView::selectWire);
		connect(actionEdge, &QAction::triggered, this, &QccView::selectEdge);
		connect(actionVertex, &QAction::triggered, this, &QccView::selectVertex); 
		menu.exec(QCursor::pos());
	}
	else if (myContext->HasDetected())
	{
		QAction* actionANLS = menu.addAction("Analyse Selection");
		QAction* actionOBB = menu.addAction("BndBox Selection");
		QAction* actionHiMesh = menu.addAction("Default Mesh");
		QAction* actionLoMesh = menu.addAction("Custom Mesh");
		QAction* actionMan = menu.addAction("Manipulator");
		QAction* actionErase = menu.addAction("Delete Selection");
		connect(actionMan, &QAction::triggered, this, &QccView::initManipulator);
		connect(actionOBB, &QAction::triggered, this, &QccView::obbSig);
		connect(actionANLS, &QAction::triggered, this, &QccView::anlsSig);
		connect(actionHiMesh, &QAction::triggered, this, [=]() { emit meshSig(false); });
		connect(actionLoMesh, &QAction::triggered, this, [=]() { emit meshSig(true); });
		connect(actionErase, &QAction::triggered, this, &QccView::deleteSig);
		menu.exec(QCursor::pos());
	}
}

void QccView::onMButtonDown(const int /*theFlags*/, const QPoint thePoint)
{
	/* save the current mouse coordinate in min */
	myXmin = thePoint.x();
	myYmin = thePoint.y();
	myXmax = thePoint.x();
	myYmax = thePoint.y();

	myCurrentMode = CurrentAction3d::CurAction3d_DynamicRotation;
	myView->StartRotation(thePoint.x(), thePoint.y());
}

void QccView::onLButtonUp(const int theFlags, const QPoint thePoint)
{
	/* Hide the QRubberBand */
	if (myRectBand)
	{
		myRectBand->hide();
	}

	/* reset myManipulator */
	myManipulator->StopTransform(Standard_True);

	/* Ctrl for multi selection */
	if (thePoint.x() == myXmin && thePoint.y() == myYmin)
	{
		if (theFlags & Qt::ControlModifier)
		{
			multiInputEvent(thePoint.x(), thePoint.y());
		}
		else
		{
			inputEvent(thePoint.x(), thePoint.y());
		}
	}
}

void QccView::onMButtonUp(const int theFlags, const QPoint thePoint)
{
	if (theFlags & Qt::ShiftModifier)
	{
		this->fitAll();
	}
	else if (thePoint.x() == myXmin && thePoint.y() == myYmin)
	{
		panByMiddleButton(thePoint);
	}
}

void QccView::onRButtonUp(const int theFlags, const QPoint thePoint)
{
	popup(thePoint.x(), thePoint.y());
}

void QccView::onMouseMove(const int theFlags, const QPoint thePoint)
{
	/* Drag the rubber band */
	if (theFlags & Qt::LeftButton)
	{	
		if (myCurrentMode == CurrentAction3d::CurAction3d_DynamicPanning)
		{
			myView->Pan(thePoint.x() - myXmax, myYmax - thePoint.y());
			myXmax = thePoint.x();
			myYmax = thePoint.y();
		}
		else if (myCurrentMode == CurrentAction3d::CurAction3d_DynamicZooming)
		{
			myView->Zoom(myXmin, myYmin, thePoint.x(), thePoint.y());
		}
		else if (myCurrentMode == CurrentAction3d::CurAction3d_Manipulating)
		{
			if (myManipulator->HasActiveMode())
			{
				myManipulator->Transform(thePoint.x(), thePoint.y(), myView); // 应用鼠标从起始位置开始移动而产生的变换
				myView->Redraw();
			}
		}
		else
		{
			drawRubberBand(myXmin, myYmin, thePoint.x(), thePoint.y());
			dragEvent(thePoint.x(), thePoint.y());
		}
	}

	/* Middle button */
	if (theFlags & Qt::MidButton)
	{
		if (myCurrentMode == CurrentAction3d::CurAction3d_DynamicRotation)
			myView->Rotation(thePoint.x(), thePoint.y());
	}

	/* Ctrl for multi selection */
	if (theFlags & Qt::ControlModifier)
	{
		moveEvent(thePoint.x(), thePoint.y());
	}
	else
	{
		multiMoveEvent(thePoint.x(), thePoint.y());
	}
}

void QccView::onMouseWheel(const int /*theFlags*/, const int theDelta, const QPoint thePoint)
{
	Standard_Integer aFactor = 16;
	Standard_Integer aX = thePoint.x();
	Standard_Integer aY = thePoint.y();

	if (theDelta > 0)
	{
		aX += aFactor;
		aY += aFactor;
	}
	else
	{
		aX -= aFactor;
		aY -= aFactor;
	}

	myView->Zoom(thePoint.x(), thePoint.y(), aX, aY);
}

void QccView::drawRubberBand(const int minX, const int minY, const int maxX, const int maxY)
{
	QRect aRect;

	/* Set the rectangle correctly */
	(minX < maxX) ? (aRect.setX(minX)) : (aRect.setX(maxX));
	(minY < maxY) ? (aRect.setY(minY)) : (aRect.setY(maxY));

	aRect.setWidth(abs(maxX - minX));
	aRect.setHeight(abs(maxY - minY));

	if (!myRectBand)
	{
		myRectBand = new QRubberBand(QRubberBand::Rectangle, this);
		/*
		* setStyle is important, set to windows style will just draw
		* rectangle frame, otherwise will draw a solid rectangle
		*/
		myRectBand->setStyle(QStyleFactory::create("window"));
	}

	myRectBand->setGeometry(aRect);
	myRectBand->show();
}

void QccView::multiDragEvent(const int x, const int y)
{
	myContext->ShiftSelect(myXmin, myYmin, x, y, myView, Standard_True);

	emit selectionChanged();
}

void QccView::dragEvent(const int x, const int y)
{
	myContext->Select(myXmin, myYmin, x, y, myView, Standard_True);
	
	emit selectionChanged();
}

void QccView::multiMoveEvent(const int x, const int y)
{
	myContext->MoveTo(x, y, myView, Standard_True);
}

void QccView::moveEvent(const int x, const int y)
{
	myContext->MoveTo(x, y, myView, Standard_True);
}

void QccView::multiInputEvent(const int x, const int y)
{
	Q_UNUSED(x);
	Q_UNUSED(y);

	myContext->ShiftSelect(Standard_True);

	emit selectionChanged();
}

void QccView::inputEvent(const int x, const int y)
{
	Q_UNUSED(x);
	Q_UNUSED(y);

	myContext->Select(Standard_True);

	emit selectionChanged();
}

void QccView::addItemInPopup(QMenu* /*theMenu*/)
{
}

void QccView::popup(const int /*x*/, const int /*y*/)
{
}

void QccView::panByMiddleButton(const QPoint& thePoint)
{
	Standard_Integer aCenterX = 0;
	Standard_Integer aCenterY = 0;

	QSize aSize = size();

	aCenterX = aSize.width() / 2;
	aCenterY = aSize.height() / 2;

	myView->Pan(aCenterX - thePoint.x(), thePoint.y() - aCenterY);
}

void QccView::selectShape()
{
	mySelectMode = AIS_Shape::SelectionMode(TopAbs_SHAPE);
	myContext->Deactivate();
	myContext->Activate(mySelectMode);
}

void QccView::selectSolid()
{
	mySelectMode = AIS_Shape::SelectionMode(TopAbs_SOLID);
	myContext->Deactivate();
	myContext->Activate(mySelectMode);
}

void QccView::selectShell()
{
	mySelectMode = AIS_Shape::SelectionMode(TopAbs_SHELL);
	myContext->Deactivate();
	myContext->Activate(mySelectMode);
}

void QccView::selectFace()
{	
	mySelectMode = AIS_Shape::SelectionMode(TopAbs_FACE);
	myContext->Deactivate();
	myContext->Activate(mySelectMode);
}

void QccView::selectWire()
{
	mySelectMode = AIS_Shape::SelectionMode(TopAbs_WIRE);
	myContext->Deactivate();
	myContext->Activate(mySelectMode);
}

void QccView::selectEdge()
{
	mySelectMode = AIS_Shape::SelectionMode(TopAbs_EDGE);
	myContext->Deactivate();
	myContext->Activate(mySelectMode);
}

void QccView::selectVertex()
{
	mySelectMode = AIS_Shape::SelectionMode(TopAbs_VERTEX);
	myContext->Deactivate();
	myContext->Activate(mySelectMode);
}
