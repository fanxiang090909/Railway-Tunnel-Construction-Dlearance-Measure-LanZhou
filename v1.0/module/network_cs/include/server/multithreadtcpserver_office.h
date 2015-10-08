#ifndef MULTITHREADTCPSERVER_OFFICE_H
#define MULTITHREADTCPSERVER_OFFICE_H

#include <QTcpServer>
#include <QList>
#include <QFile>

#include "multithreadtcpserver.h"
#include "serverthread.h"

#include "filesender.h"
#include "filereceiver.h"

/**
 * 多线程(已经不再是多线程，只是QTcpSocket本身有多线程机制)TCP服务器类设计
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-25
 */
class OfficeMultiThreadTcpServer : public MultiThreadTcpServer
{
    Q_OBJECT

public:
    OfficeMultiThreadTcpServer(QObject *parent, int msgListenPort = 9424, int fileReceivePort = 7777, int fileSendPort = 8888);

    ~OfficeMultiThreadTcpServer();

private slots:
    /**
     * TcpServer获得新的从控TcpSocket连接
     * (1) tempSocket = tcpServer->nextPendingConnection();
     * (2) tempSocket 赋给 自定义的ServerThread，并设置信号槽
     * (3) 将ServerThread保存到内存threads队列中
     * @override MultiThreadTcpServer的槽函数:收到TCP连接请求
     */
    virtual void incomingConnection();
};

#endif // MULTITHREADTCPSERVER_OFFICE_H
