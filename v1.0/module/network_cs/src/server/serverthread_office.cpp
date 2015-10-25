#include "serverthread_office.h"
#include <QtNetwork>
#include "serverprogram.h"

/**
 * 主控机服务器线程类实现（已经不再是线程）
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-2-17
 */

/**
 * 服务器线程构造函数
 * 所维护的tcpSocket置为空
 * numOfThreads记录当前维护线程数
 */
OfficeServerThread::OfficeServerThread(QObject *parent)
 : ServerThread(parent)
{
    qDebug() << tr("构造Masterthread对象");
    //tcpSocket = NULL;
    //clientip = "";
}

/**
 * 服务器线程析构函数
 * 【注意】不能再这里删除socket，，，用delete tcpSocket;
 *       而是用tcpSocket的deleteLater
 */
OfficeServerThread::~OfficeServerThread()
{
    qDebug() << tr("析构Officethread对象") << this->clientip;
    qDebug() << "delete thread!" << QThread::currentThreadId();
    //if (tcpSocket != NULL)
    //    delete tcpSocket;
}

void OfficeServerThread::initTcpSocket()
{
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(emitdisconnect()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessageFromSlave()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayErrorFromSlave(QAbstractSocket::SocketError)), Qt::DirectConnection);


    // 一个主线程和n个子线程通信怎么搞?这样用消息槽搞不定，消息的参数无法传入子线程
    connect(ServerProgram::getServerProgramInstance()->getMultiThreadTcpServer(),
            SIGNAL(msgToBeSent(const QString&)), this, SLOT(sendMessage(const QString&)), Qt::DirectConnection);
    connect(ServerProgram::getServerProgramInstance()->getMultiThreadTcpServer(),
            SIGNAL(msgToBeSentToOneSlave(QString&, QString&)), this, SLOT(sendMessageToOne(QString&, QString&)), Qt::DirectConnection);

    //connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(mineDeleteLater()));
    QHostAddress address =  tcpSocket->peerAddress();
    clientip = address.toString();
    setClientIPStr(clientip);
    qDebug()<<"address:"<<address;

    emit signalMsg(QString("%1,-7").arg(clientidstr));
    qDebug() << "init ok";
}
