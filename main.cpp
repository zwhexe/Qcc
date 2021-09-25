#include "Qcc.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qcc w;
    w.show();
    return a.exec();
}
