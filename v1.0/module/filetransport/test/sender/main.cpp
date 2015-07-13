#include "filelistsender.h"
#include "form.h"

#include <QtGui/QApplication>
#pragma comment(lib,"QtCored4.lib")
#pragma comment(lib,"QtGuid4.lib")
#pragma comment(lib,"QtNetworkd4.lib")


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Form f;
    f.show();
    return a.exec();
}
