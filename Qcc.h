#pragma once

#include <AIS_InteractiveContext.hxx>
#include <QMainWindow>
#include "ui_Qcc.h"

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

protected:
    void createActions(void);
    void createMenus(void);
    void createToolBars(void);
    void createStatusBar(void);

    void makeCylindericalHelix(void);

private slots:
    /* Help */
    void about(void);

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

    void testCut(void);
    void testHelix(void);

private:
    Ui::QccClass *ui;

    QccView* myQccView;
};
