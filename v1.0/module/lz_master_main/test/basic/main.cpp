//#include <QtCore/QCoreApplication>

#include <QtGui/QApplication>
#include "masterprogram.h"
#include "xmlfileloader.h"
#include "xmlnetworkfileloader.h"
#include "xmltaskfileloader.h"
#include <QDebug>
#include <QTextCodec>
#include "form.h"
#include "status.h"

int main(int argc, char *argv[])
{
    //QCoreApplication a(argc, argv);

    //MasterProgram m;
    //m.run();

    QApplication a(argc, argv);

    /*******************************/
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GBK")); //当QString变量中含有中文时，需要增加这两行代码
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));

    Form f;
    f.show();

    /*************************/
    // 读取实际采集队列的方法
    /*list<CollectRecord>::iterator it = Status::getStatusInstance()->collectListBegin();
    while (it != Status::getStatusInstance()->collectListEnd())
    {
        (*it).seqno; // 初始为1，后边新的隧道++
        (*it).record_tunnelid;
        (*it).record_remark;
        (*it).isvalid;
        (*it).corrected_tunnelid; // 经过校正之后的tunnelid
        (*it).returnnum; // 已经有几个从控机发送过采集完成此隧道的消息，读取时正常应该是9
        (*it).start_framecounter; 
        (*it).end_framecounter;
        it++;
    }*/

    return a.exec();
}
