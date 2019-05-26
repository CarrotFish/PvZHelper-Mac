#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

// std::string ProcessName = "Plants vs. Zombi";
// xnu_proc *GameProc = new xnu_proc();
// uint32_t base = 0x35EE64;
// int pid = 0;

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
    // GameProc->Detach();
    //delete GameProc;
}
