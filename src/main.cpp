#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[]) {
    QCoreApplication::setSetuidAllowed(true);
    QApplication a(argc, argv);
    
    QTranslator translator;
    if (translator.load("qt_zh_CN.qm", QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        QApplication::installTranslator(&translator);
    }
    
    MainWindow w;
    QApplication::setActiveWindow(&w);
    w.show();
    return QApplication::exec();
}
