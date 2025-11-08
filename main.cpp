#include "mainwindow.h"
#include "sidebarwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    SidebarWindow w;
    w.show();
    return a.exec();
}
