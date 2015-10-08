#include "multithreadtcpserver.h"
#include "serverthread.h"

#include <QFile>

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
MultiThreadTcpServer::MultiThreadTcpServer(QObject *parent, int msgListenPort, int fileReceivePort, int fileSendPort) : QObject(parent), listenPort(msgListenPort), fileReceivePort(fileReceivePort), fileSendPort(fileSendPort)
{
    tcpServer = new QTcpServer(this);

    numOfThreads = 0;

    if (!tcpServer->listen(QHostAddress::Any, this->listenPort))
    {
        qDebug() << QObject::tr("Unable to start the msg server: %1.").arg(tcpServer->errorString());
        //return; //监听任何连上19999端口的ip
    }

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(incomingConnection()));

    tempsocket = NULL;

    // 多文件发送类（串行）
    fileListSender = new FileListSender();
    connect(fileListSender, SIGNAL(error_openfile(QString, QString, int)), this, SLOT(fileopenError(QString, QString, int)));
    connect(fileListSender, SIGNAL(current_progress(QString, QString, qint64, qint64, int)), this, SLOT(updateClientProgress(QString, QString, qint64, qint64, int)));
    connect(fileListSender, SIGNAL(error_transfer(QString, QString, QString, int)), this, SLOT(fileError(QString, QString, QString, int)));
    connect(fileListSender, SIGNAL(end_transfer(QString, QString, int, int, int)), this, SLOT(fileend(QString, QString, int, int, int)));

    // 多文件接收类，多线程并行接收不同主机发来的文件
    fileReceiverServer = new FileReceiverServer(this->fileReceivePort);
    connect(fileReceiverServer, SIGNAL(error_openfile(QString, QString, int)), this, SLOT(fileopenError(QString, QString, int)));
    connect(fileReceiverServer, SIGNAL(current_progress(QString, QString, qint64, qint64, int)), this, SLOT(updateClientProgress(QString, QString, qint64, qint64, int)));
    connect(fileReceiverServer, SIGNAL(error_transfer(QString, QString, QString, int)), this, SLOT(fileError(QString, QString, QString, int)));
    connect(fileReceiverServer, SIGNAL(end_transfer(QString, QString, int, int, int)), this, SLOT(fileend(QString, QString, int, int, int)));

    // 心跳发包计时器
    timer = new QTimer();
    timer->setInterval(100000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    timer->start();
}

/**
 * 析构函数，删除线程数组中的实例线程
 */
MultiThreadTcpServer::~MultiThreadTcpServer()
{
    for (int i = 0; i < numOfThreads; i++)
    {
        slavercdmutex.lock();
        if (threads.at(0) != NULL)
            delete threads.at(0);
        numOfThreads--;
        threads.removeFirst();
        slavercdmutex.unlock();
    }
    // msgSocket不需delete，其只是threads中的socket指针，上边已经delete掉

    // 销毁文件发送和接收类
    if (fileReceiverServer != NULL)
        delete fileReceiverServer;

    if (fileListSender != NULL)
        delete fileListSender;

    if (timer != NULL)
    {
        timer->stop();
        delete timer;
    }
}

int MultiThreadTcpServer::getNumOfThreads()
{
    int ret = 0;
    slavercdmutex.lock();
    ret = numOfThreads;
    slavercdmutex.unlock();
    return ret;
}

void MultiThreadTcpServer::printConnectedSockets()
{
    qDebug() << "**********printConnectedSockets*********";

    for (int i = 0; i < numOfThreads; i++)
    {
        if (threads.at(i) != NULL)
        {
            //qDebug() << threads.at(i)->getClientIP();
            qDebug() << threads.at(i)->getTcpSocket()->peerAddress().toString();
        }
    }
}

/**
 * TcpServer获得新的从控TcpSocket连接
 * (1) tempSocket = tcpServer->nextPendingConnection();
 * (2) tempSocket 赋给 自定义的ServerThread，并设置信号槽
 * (3) 将ServerThread保存到内存threads队列中
 * @override QTcpServer的槽函数:收到TCP连接请求
 */
void MultiThreadTcpServer::incomingConnection()
{
    tempsocket = tcpServer->nextPendingConnection();

    if (tempsocket == NULL)
        return;

    int numOfConnectionThreads = numOfThreads;
    for (int i = 0; i < numOfConnectionThreads; i++)
    {
        if (threads.at(i) == NULL)
            continue;
        QString ip = threads.at(i)->getTcpSocket()->peerAddress().toString();
        if (ip.compare(tempsocket->peerAddress().toString()) == 0)
        {
            threads.at(i)->setTcpSocket(tempsocket);

            qDebug() << "connection from slave";
            //emit signalMsgToMaster("-7, new connection");
            return;
        }
    }
    ServerThread *thread = new ServerThread();
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

void MultiThreadTcpServer::removeServerThread(QString slaveip)
{
    qDebug() << "remove server thread" << slaveip << "from " << numOfThreads << "slaves";
    //printSlavesSpckets();

    int numThreads = numOfThreads;
    if (numThreads == 0)
        qDebug() << "error remove server thread, no slave" << slaveip;
    for (int i = 0; i < numThreads; i++)
    {
        if ((threads.at(i) == NULL) || (threads.at(i) != NULL && threads.at(i)->getClientIP().compare(slaveip) == 0))
        {
            ServerThread * todelete = threads.at(i);
            slavercdmutex.lock();
            threads.removeAt(i);
            numOfThreads--;
            slavercdmutex.unlock();
            //delete todelete;?? 删除出错@author 范翔
            qDebug() << "remove ok " << slaveip;
            return;
        }
    }
    //printSlavesSpckets();
}

/**
 * 心跳发包计时器
 */
void MultiThreadTcpServer::onTimeOut()
{
    if (numOfThreads < 0)
        return;
    sendMessageToSlaves("-100");
}

/**
 * 向ServerThread列表中的每个线程socket发消息，也就是给从控机当前连接的所有socketTCP消息
 */
void MultiThreadTcpServer::sendMessageToSlaves(QString msgStr)
{
    if (numOfThreads == 0)
    {
        qDebug() << "no slaves, couldn't send message!";
        // 为什么没反应消息***********************？
        emit signalErrorToMaster("0, no slaves");
        return;
    }

    qDebug() << "to " << numOfThreads << " slaves: " << msgStr;

    // @author 范翔 @date 20140528，一个消息发送者，多个ServerThread作为消息接受者
    // @see MasterServerThread::initTcpSocket()
    //emit msgToBeSent(msgStr);
    for (int i = 0; i < numOfThreads; i++)
    {
        QString ip = threads.at(i)->getTcpSocket()->peerAddress().toString();
        threads.at(i)->sendMessageToOne(ip, msgStr);
        //emit msgToBeSentToOneSlave(slaveip, msgStr);
    }
}

/**
 * 向ServerThread列表中的其中之一线程socket发消息，即给该线程维护的socketTCP消息
 */
void MultiThreadTcpServer::sendMessageToOneSlave(QString slaveip, QString msgStr)
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
        QString ip = threads.at(i)->getTcpSocket()->peerAddress().toString();
        qDebug() << "to slave " << slaveip << " : " << ip;

        if (slaveip.compare(ip) == 0)
        {
            threads.at(i)->sendMessageToOne(slaveip, msgStr);
            //emit msgToBeSentToOneSlave(slaveip, msgStr);
            return;
        }
    }
    qDebug() << "can not send to slaveip = " << slaveip;
    //emit signalErrorToMaster("0, no slaves");
}

/**
 * 向ServerThread列表中的每个线程socket发文件，也就是给从控机当前连接的所有socketTCP文件
 */
void MultiThreadTcpServer::sendFileToSlaves(QString sendFilename)
{
    if (numOfThreads == 0)
    {
        qDebug() << "no slaves, couldn't send file!";
        // 为什么没反应消息***********************？
        emit signalErrorToMaster("-2, no slaves");
        return;
    }
    qDebug() << "send file to " << numOfThreads << " slaves: " << sendFilename;
    QString msgToGUI;

    QFile localFile2(sendFilename);
    if (!localFile2.exists())
    {
        qDebug() << "open local file (to be sent) error" << sendFilename;
        return;
    }

    for (int i = 0; i < numOfThreads; i++)
    {
        QString ipadd = threads.at(i)->getClientIP();
        qDebug() << "file " << sendFilename << " to ip address: " << ipadd;

        this->sendFileToOneSlave(ipadd, sendFilename);
    }
}

void MultiThreadTcpServer::sendFileToOneSlave(QString ipaddress, QString fileName)
{
    bool cansend = false;

    // 查找待发文件是否存在
    QFile qf(fileName);
    if (!qf.exists())
    {
        qDebug() << "no file to be send, filename:" << fileName;
        return;
    }

    // 查找发往IP机是否连接
    for (int i = 0; i < numOfThreads; i++)
    {
        if (ipaddress.compare(threads.at(i)->getClientIP()) == 0)
        {
            cansend = true;
            qDebug() << "file " << fileName << " to finded ip address: " << ipaddress;
        }
    }
    if (cansend == false)
    {
        qDebug() << "no slave ip" << ipaddress << ", can not send file";
        return;
    }

    // 添加到发送文件队列
    FileToSend newtask;
    newtask.toip = ipaddress;
    newtask.toport = fileSendPort;
    newtask.filename = fileName;
    bool ret = fileListSender->pushBack(newtask);
    if (ret)
        qDebug() << "start send file to ip: " << ipaddress << ", filename:" << fileName << "now";
    else
        qDebug() << "start send file to ip: " << ipaddress << ", filename:" << fileName << "later";
}

FileListSender* MultiThreadTcpServer::getCurrentFileListSender()
{
    return fileListSender;
}

FileReceiverServer * MultiThreadTcpServer::getCurrentFileReceiverServer()
{
    return fileReceiverServer;
}

QString MultiThreadTcpServer::getFileReceiveSenderIP(QString ip)
{
    return ip;
}

void MultiThreadTcpServer::fileopenError(QString ip, QString file, int type)
{
    qDebug() << "file open error" << file;
    // 从控机i
    printReceivedError("-1, 8,");
    QString msgToGUI = QString(QObject::tr("%1,-10,%2,%3")).arg(getFileReceiveSenderIP(ip)).arg(type).arg(file);
    emit signalErrorToMaster(msgToGUI);
}

void MultiThreadTcpServer::fileend(QString ip, QString file, int filesize, int timespend, int type)
{
    qDebug() << "file end" << file;
    QString msgToGUI = QString(QObject::tr("%1,-9,%2,%3,%4,%5")).arg(getFileReceiveSenderIP(ip)).arg(type).arg(file).arg(filesize).arg(timespend);
    emit signalMsgToMaster(msgToGUI);
}

void MultiThreadTcpServer::updateClientProgress(QString ip, QString file, qint64 total, qint64 numBytes, int type)
{
    //QString filename = fileSender->getCurrentFilename();
}

void MultiThreadTcpServer::fileError(QString ip, QString file, QString err, int type)
{
    qDebug() << "file error" << file << err;
    QString msgToGUI = QString(QObject::tr("%1,-11,%2,%3,%4")).arg(getFileReceiveSenderIP(ip)).arg(type).arg(file).arg(err);
    emit signalErrorToMaster(msgToGUI);
    // 从控机i
    printReceivedError("-1, 8");
}

void MultiThreadTcpServer::printReceivedMsg(QString receivedMsg)
{
    qDebug() << "in server slot: " << receivedMsg;
    emit signalMsgToMaster(receivedMsg);
}

void MultiThreadTcpServer::printReceivedError(QString receivedError)
{
    qDebug() << "in server slot error: " << receivedError;
    emit signalErrorToMaster(receivedError);
}

/**
 * 设置文件传输-接收文件存放目录
 */
bool MultiThreadTcpServer::setReceiveFileSavingPath(QString newpath)
{
    return fileReceiverServer->setCurrentSavingPath(newpath);
}
