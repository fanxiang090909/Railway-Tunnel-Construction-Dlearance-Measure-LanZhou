#include "multithreadtcpserver_office.h"
#include "serverthread_office.h"

#include <QFile>

/**
 * 多线程(已经不再是多线程，只是QTcpSocket本身有多线程机制)TCP服务器类实现
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-2-17
 */

OfficeMultiThreadTcpServer::OfficeMultiThreadTcpServer(QObject *parent, int msgListenPort, int fileReceivePort, int fileSendPort) : MultiThreadTcpServer(parent, msgListenPort, fileReceivePort, fileSendPort)
{

}

/**
 * 析构函数，删除线程数组中的实例线程
 */
OfficeMultiThreadTcpServer::~OfficeMultiThreadTcpServer()
{
}

/**
 * @override QTcpServer的槽函数:收到TCP连接请求
 *
 * 创建子线程，每个线程的销毁槽函数
 */
void OfficeMultiThreadTcpServer::incomingConnection()
{
    tempsocket = tcpServer->nextPendingConnection();

    if (tempsocket == NULL)
        return;

    for (int i = 0; i < numOfThreads; i++)
    {
        QString ip = threads.at(i)->getTcpSocket()->peerAddress().toString();
        if (ip == tempsocket->peerAddress().toString())
        {
            threads.at(i)->setTcpSocket(tempsocket);

            qDebug() << "connection from slave";
            //emit signalMsgToMaster("-7, new connection");
            return;
        }
    }
    OfficeServerThread *thread = new OfficeServerThread();
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
