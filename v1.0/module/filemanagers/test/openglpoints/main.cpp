#include <QtGui/QApplication>
#include "glwidget.h"

#include "show_3dpoints.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Show3DPointsWidget *vectorWidget
            = new Show3DPointsWidget();
    vectorWidget->read("Output.txt");

    vectorWidget->show();



    return a.exec();
}
