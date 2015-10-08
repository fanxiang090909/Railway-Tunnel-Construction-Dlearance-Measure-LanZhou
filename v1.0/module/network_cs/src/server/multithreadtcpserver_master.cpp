#include "multithreadtcpserver_master.h"
#include "serverthread_master.h"

#include <QFile>
#include "network_config_list.h"

// @author 范翔
// Qt中要简单实现暂停一段时间，可以使用<windows.h>头文件中的函数Sleep()，
// 但要注意调用格式是::Sleep(time_ms);，这个函数使用的较少，且不跨平台，更多的需要学习QTread类中的。
//#include <Windows.h>

/**
 * 多线程(已经不再是多线程，只是QTcpSocket本身有多线程机制)TCP服务器类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-25
 */

MasterMultiThreadTcpServer::MasterMultiThreadTcpServer(QObject *parent, int msgListenPort, int fileReceivePort, int fileSendPort) : MultiThreadTcpServer(parent, msgListenPort, fileReceivePort, fileSendPort)
{

}

/**
 * 析构函数，删除线程数组中的实例线程
 */
MasterMultiThreadTcpServer::~MasterMultiThreadTcpServer()
{
}

/**
 * qDebug打印当前连接所有ip
 */
void MasterMultiThreadTcpServer::printConnectedSockets()
{
    qDebug() << "**********printConnectedSockets*********";

    for (int i = 0; i < numOfThreads; i++)
    {
        if (threads.at(i) != NULL)
            qDebug() << "\t getTcpSocket()->peerAddress().toString():" << threads.at(i)->getTcpSocket()->peerAddress().toString() <<
                                                          ",ClientIP:" << threads.at(i)->getClientIP();
    }
}

/**
 * @override QTcpServer的槽函数:收到TCP连接请求
 *
 * 创建子线程，每个线程的销毁槽函数
 */
void MasterMultiThreadTcpServer::incomingConnection()
{
    tempsocket = tcpServer->nextPendingConnection();

    if (tempsocket == NULL)
        return;

    for (int i = 0; i < numOfThreads; i++)
    {
        QString ip = threads.at(i)->getClientIP();
        if (ip.compare(tempsocket->peerAddress().toString()) == 0)
        {
            threads.at(i)->setTcpSocket(tempsocket);

            qDebug() << "connection from slave";
            //emit signalMsgToMaster("-7, new connection");
            return;
        }
    }
    MasterServerThread *thread = new MasterServerThread();
    connect(thread, SIGNAL(signalMsg(QString)), this, SLOT(printReceivedMsg(QString)), Qt::QueuedConnection);
    connect(thread, SIGNAL(signalError(QString)), this, SLOT(printReceivedError(QString)), Qt::QueuedConnection);
    connect(thread, SIGNAL(closeConnection(QString)), this, SLOT(removeServerThread(QString)));
    thread->setTcpSocket(tempsocket); // 在connet之后保证onConnection槽函数中的消息也能被接收到

    /*****************************************/
    slavercdmutex.lock();
    threads.append(thread);
    numOfThreads++;
    slavercdmutex.unlock();
    qDebug() << "connection from slave";
    //emit signalMsgToMaster("-7, new connection");
}

void MasterMultiThreadTcpServer::sendMessageToOneSlave(QString slaveip, QString msgStr)
{
    qDebug() << "MultiThreadTcpServer::sendMessageToOneSlave" << QThread::currentThreadId();
    if (numOfThreads == 0)
    {
        qDebug() << "no client";
        return;
    }
    qDebug() << "to slave " << slaveip << " : " << msgStr << "current connection" << numOfThreads;
    for (int i = 0; i < numOfThreads; i++)
    {
        QString ip = threads.at(i)->getClientIP();
        qDebug() << "to slave " << slaveip << " : " << ip;

        if (slaveip.compare(ip) == 0)
        {
            threads.at(i)->sendMessage(msgStr);
            //emit msgToBeSentToOneSlave(slaveip, msgStr);
            return;
        }
    }
    qDebug() << "can not send to slaveip = " << slaveip;
    //emit signalErrorToMaster("0, no slaves");
}

/**
 * 向ServerThread列表中的其中之一线程socket发消息，即给该线程维护的socketTCP消息
 */
/*void MasterMultiThreadTcpServer::sendMessageToOneSlave(int slaveid, QString msgStr)
{
    qDebug() << "MasterMultiThreadTcpServer::sendMessageToOneSlave" << QThread::currentThreadId();
    qDebug() << "to slave " << slaveid << " : " << msgStr << "current connection" << numOfThreads;

    for (int i = 0; i < numOfThreads; i++)
    {

        MasterServerThread & thread = (MasterServerThread) threads.at(i);
        qDebug() << "to slave " << slaveid << " : " << thread.getSlaveID();

        if (slaveid == thread.getSlaveID())
        {
            thread.sendMessage(msgStr);
            qDebug() << "MultiThreadTcpServer::sendMessageToOneSlave" << slaveid <<  msgStr;
            //emit msgToBeSentToOneSlave(slaveid, msgStr);
            return;
        }
    }
    //qDebug() << "to slave " << slaveid << " : " << msgStr;
    //emit signalErrorToMaster("0, no slaves");
}*/

QString MasterMultiThreadTcpServer::getFileReceiveSenderIP(QString ip)
{
    int slaveid = NetworkConfigList::getNetworkConfigListInstance()->findSlaveId(ip);
    return QString("%1").arg(slaveid);
}