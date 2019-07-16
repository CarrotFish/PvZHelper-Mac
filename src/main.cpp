#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setSetuidAllowed(true);
    QApplication a(argc, argv);
    
    MainWindow w;
    w.show();
    return QApplication::exec();
}
