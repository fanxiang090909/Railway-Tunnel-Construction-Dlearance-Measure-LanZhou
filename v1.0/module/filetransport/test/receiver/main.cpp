#include <QtCore/QCoreApplication>
#include "filereceiverserver.h"
#include <QtCore/QTextCodec>
#pragma comment(lib,"QtCored4.lib")
#pragma comment(lib,"QtNetworkd4.lib")

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    FileReceiverServer * filebackupserver = new FileReceiverServer(8888);

    return a.exec();
}
