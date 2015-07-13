#include "filereceiverserver.h"

#include <QFile>

/**
 * 多文件接收服务器类实现
 * 底层为多线程TCPScoket
 * 考虑接收端不能保证同时只接收一个IP的文件，但可以保证任意时刻只接收一个IP的一个文件（@see FileListSender 定义）
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-6-6
 */
FileReceiverServer::FileReceiverServer(int initport, QObject * parent) : QObject(parent)
{
    tcpServer = new QTcpServer();

    numOfThreads = 0;

    // 默认当前目录为接收路径
    savingpath = ".";

    // 默认监听8888端口号
    port = initport;

    if (!tcpServer->listen(QHostAddress::Any, port))
    {
        qDebug() << QObject::tr("Unable to start the msg server: %1.").arg(tcpServer->errorString());
        //return; //监听任何连上19999端口的ip
    }

    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(incomingConnection()));

    tempsocket = NULL;
    /******【方案2】*******/

    // 设置文件发送和接收类
    istranferfilereceive.lock();
    istransferfile_receive = false;
    istranferfilereceive.unlock();
}

/**
 * 析构函数，删除线程数组中的实例线程
 */
FileReceiverServer::~FileReceiverServer()
{
    for (int i = 0; i < numOfThreads; i++)
    {
        slavercdmutex.lock();
        if (receivers.at(0) != NULL)
            delete receivers.at(0);
        numOfThreads--;
        receivers.removeFirst();
        slavercdmutex.unlock();
    }
    //msgSocket不需delete，其只是receivers中的socket指针，上边已经delete掉
}

void FileReceiverServer::printSlavesSockets()
{
    qDebug() << "**********printConnectedSockets*********";

    for (int i = 0; i < numOfThreads; i++)
    {
        if (receivers.at(i) != NULL)
        {
            //qDebug() << threads.at(i)->getClientIP();
            qDebug() << receivers.at(i)->getTcpSocket()->peerAddress().toString();
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
void FileReceiverServer::incomingConnection()
{
    tempsocket = tcpServer->nextPendingConnection();

    if (tempsocket == NULL)
        return;

    for (int i = 0; i < numOfThreads; i++)
    {
        QString ip = receivers.at(i)->getTcpSocket()->peerAddress().toString();
        if (ip.compare(tempsocket->peerAddress().toString()) == 0)
        {
            receivers.at(i)->setTcpSocket(tempsocket);

            qDebug() << "connection from slave";
            return;
        }
    }

    FileReceiver *thread = new FileReceiver();
    thread->setPath(this->savingpath);
    connect(thread, SIGNAL(error_openfile(QString, QString, int)), this, SLOT(fileopenError(QString, QString, int)));
    connect(thread, SIGNAL(current_progress(QString, QString, qint64, qint64, int)), this, SLOT(updateClientProgress(QString,QString, qint64, qint64, int)));
    connect(thread, SIGNAL(error_transfer(QString, QString,QString, int)), this, SLOT(fileError(QString, QString, QString, int)));
    connect(thread, SIGNAL(end_transfer(QString, QString, int, int, int)), this, SLOT(fileend(QString, QString, int, int, int)));
    connect(thread, SIGNAL(closeConnection(QString)), this, SLOT(removeServerThread(QString)));
    thread->setTcpSocket(tempsocket); // 在connet之后保证onConnection槽函数中的消息也能被接收到

    /*****************************************/
    slavercdmutex.lock();
    receivers.append(thread);
    numOfThreads++;
    slavercdmutex.unlock();
    qDebug() << "connection from slave";
}

void FileReceiverServer::removeServerThread(QString slaveip)
{
    qDebug() << "remove server thread" << slaveip << "from " << numOfThreads << "slaves";
    //printSlavesSpckets();

    int numThreads = numOfThreads;
    if (numThreads == 0)
        qDebug() << "error remove server thread, no slave" << slaveip;
    for (int i = 0; i < numThreads; i++)
    {
        if ((receivers.at(i) == NULL) || (receivers.at(i) != NULL && receivers.at(i)->getReceiveIP().compare(slaveip) == 0))
        {
            slavercdmutex.lock();
            if (receivers.at(i) != NULL)
                delete receivers.at(i);
            receivers.removeAt(i);
            numOfThreads--;
            slavercdmutex.unlock();
            qDebug() << "remove ok " << slaveip;
            break;
        }
    }
    //printSlavesSpckets();
}

void FileReceiverServer::fileopenError(QString ip, QString file, int type)
{
    emit error_openfile(ip, file, type);
}

void FileReceiverServer::fileend(QString ip, QString file, int filesize, int timespend, int type)
{
    emit end_transfer(ip, file, filesize, timespend, type);
}

void FileReceiverServer::updateClientProgress(QString ip, QString file, qint64 total, qint64 numBytes, int type)
{
    //emit
}

void FileReceiverServer::fileError(QString ip, QString file, QString err, int type)
{
    emit error_transfer(ip, file, err, type);
}

/**
 * 设置文件传输-接收文件存放目录
 */
bool FileReceiverServer::setCurrentSavingPath(QString newpath)
{
    istranferfilereceive.lock();
    if (istransferfile_receive == false)
    {
        istranferfilereceive.unlock();
        savingpath = newpath;
        return true;
    }
    else
    {
        istranferfilereceive.unlock();
        return false;
    }
}
