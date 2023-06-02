#include "mainwindow.h"
#include "weatherairnow.h"
#include "weatherbaseinfo.h"
#include "weatherdata.h"
#include "weatherinfo.h"
#include "weatherlife.h"

#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    MainWindow w;
    w.show();
    return a.exec();
}
