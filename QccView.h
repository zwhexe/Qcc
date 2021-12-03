#pragma once

#include <QWidget>
#include <QGLWidget>
#ifdef _WIN32
#include <WNT_Window.hxx>
#else
#undef None
#include <Xw_Window.hxx>
#endif

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <AIS_Shape.hxx>
#include <AIS_ViewController.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Manipulator.hxx>

class QMenu;
class QRubberBand;
class RotCircle;

class QccView : public QGLWidget
{
    Q_OBJECT

public:
    /* mouse actions */
    enum class CurrentAction3d
    {
        CurAction3d_Nothing,
        CurAction3d_DynamicZooming,
        CurAction3d_WindowZooming,
        CurAction3d_DynamicPanning,
        CurAction3d_GlobalPanning,
        CurAction3d_DynamicRotation,
        CurAction3d_Manipulating,
    };

public:
    QccView(QWidget* parent);
    const Handle(AIS_InteractiveContext)& getContext() const;
    const Standard_Integer getSelectMode() const;

signals:
    void selectionChanged(void);
    void obbSig(void);
    void anlsSig(void);
    void meshSig(bool);
    void deleteSig(void);

public slots:
    /* operations for the view */
    void show(const TopoDS_Shape&);
    void zoom(void);
    void pan(void);
    void rotate(void);
    void reset(void);
    void fitAll(void);

    /* mouse select mode */
    void selectShape(void);
    void selectSolid(void);
    void selectShell(void);
    void selectFace(void);
    void selectWire(void);
    void selectEdge(void);
    void selectVertex(void);

    /* ais_manipulator */
    void initManipulator(void);

protected:
    /* paint events */
    virtual QPaintEngine* paintEngine() const;
    virtual void paintEvent(QPaintEvent* theEvent);
    virtual void resizeEvent(QResizeEvent* theEvent);

    /* Mouse events */
    virtual void mousePressEvent(QMouseEvent* theEvent);
    virtual void mouseReleaseEvent(QMouseEvent* theEvent);
    virtual void mouseMoveEvent(QMouseEvent* theEvent);
    virtual void wheelEvent(QWheelEvent* theEvent);

    /* Key events */
    virtual void keyPressEvent(QKeyEvent* theEvent);
    virtual void keyReleaseEvent(QKeyEvent* theEvent);

    /* Button events */
    virtual void onLButtonDown(const int theFlags, const QPoint thePoint);
    virtual void onMButtonDown(const int theFlags, const QPoint thePoint);
    virtual void onRButtonDown(const int theFlags, const QPoint thePoint);
    virtual void onMouseWheel(const int theFlags, const int theDelta, const QPoint thePoint);
    virtual void onLButtonUp(const int theFlags, const QPoint thePoint);
    virtual void onMButtonUp(const int theFlags, const QPoint thePoint);
    virtual void onRButtonUp(const int theFlags, const QPoint thePoint);
    virtual void onMouseMove(const int theFlags, const QPoint thePoint);

    /* Popup menu */
    virtual void addItemInPopup(QMenu* theMenu);

protected:
    void initContext();
    void popup(const int x, const int y);

    void multiDragEvent(const int x, const int y);
    void dragEvent(const int x, const int y);

    void multiMoveEvent(const int x, const int y);
    void moveEvent(const int x, const int y);

    void multiInputEvent(const int x, const int y);
    void inputEvent(const int x, const int y);

    void drawRubberBand(const int minX, const int minY, const int maxX, const int maxY);
    void panByMiddleButton(const QPoint& thePoint);

private:
    Handle(AIS_InteractiveContext) myContext;
    Handle(AIS_Manipulator) myManipulator;
    Handle(V3d_Viewer) myViewer;
    Handle(V3d_View) myView;
    Handle(Graphic3d_GraphicDriver) myDriver;

    Standard_Integer mySelectMode;

    Standard_Integer myXmin;
    Standard_Integer myYmin;
    Standard_Integer myXmax;
    Standard_Integer myYmax;

    /* the mouse current mode */
    CurrentAction3d myCurrentMode;
    /* save the degenerate mode state */
    Standard_Boolean myDegenerateModeIsOn;
    /* rubber rectangle for the mouse selection */
    QRubberBand* myRectBand;
};

