#ifndef MULTITHREADTCPSERVER_MASTER_H
#define MULTITHREADTCPSERVER_MASTER_H

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
class MasterMultiThreadTcpServer : public MultiThreadTcpServer
{
    Q_OBJECT
private:


public:
    MasterMultiThreadTcpServer(QObject *parent, int msgListenPort = 9424, int fileReceivePort = 7777, int fileSendPort = 8888);

    ~MasterMultiThreadTcpServer();
    
    /**
     * qDebug打印当前连接所有ip
     */
    virtual void printConnectedSockets();

    /***********网络发送文件、消息服务调用接口******************/
    // 向ServerThread列表中的其中之一线程socket发消息，即给该线程维护的socketTCP消息
    void sendMessageToOneSlave(QString slaveip, QString msgStr);
    //void sendMessageToOneSlave(int slaveid, QString msgStr); //@date 20140803 @author范翔 有 bug slaveid找不到对应

    /**
     * 返回文件传输时告知界面的主机索引，默认为IP地址
     * 这里Master将其变换为从机index号
     */
    virtual QString getFileReceiveSenderIP(QString ip);

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

#endif // MULTITHREADTCPSERVER_MASTER_H
