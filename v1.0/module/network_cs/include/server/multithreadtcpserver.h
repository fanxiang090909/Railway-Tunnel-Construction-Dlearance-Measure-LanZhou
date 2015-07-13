#ifndef MULTITHREADTCPSERVER_H
#define MULTITHREADTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include <QFile>
#include "serverthread.h"

#include "filelistsender.h"
#include "filereceiverserver.h"

/**
 * 多线程(已经不再是多线程，只是QTcpSocket本身有多线程机制)TCP服务器类设计
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-2-11
 */
class MultiThreadTcpServer : public QObject
{
    Q_OBJECT
protected:
    /**
     * Tcp 多线程服务器类
     */
    QTcpServer * tcpServer;
    /**
     * 临时设置发送接收各从机的tcpsocket
     * 后边会加入到 threads 中
     */
    QTcpSocket * tempsocket;

    /**
     * 静态变量：threads主控服务程序维护的线程列表
     */
    QList<ServerThread*> threads;

    /**
     * 静态变量：numOfThreads记录当前维护线程数
     */
    int numOfThreads;

    /******【方案2】*******/
    // 使用方案二，获得线程的socket指针，发送网络消息
    //QTcpSocket * msgSocket;
    QMutex slavercdmutex;

    /**
     * 发送文件的类
     */
    FileListSender * fileListSender;
    /**
     * 接收文件的类
     */
    FileReceiverServer * fileReceiverServer;

    /**
     * 心跳发包计时器
     */
    QTimer * timer;

public:
    MultiThreadTcpServer(QObject *parent);

    virtual ~MultiThreadTcpServer();

    int getNumOfThreads();

    /**
     * qDebug打印当前连接所有ip
     */
    virtual void printConnectedSockets();

    /***********网络发送文件、消息服务调用接口******************/
    // 向ServerThread列表中的每个线程socket发消息，也就是给从控机当前连接的所有socketTCP消息
    void sendMessageToSlaves(QString msgStr);
    // 向ServerThread列表中的其中之一线程socket发消息，即给该线程维护的socketTCP消息
    virtual void sendMessageToOneSlave(QString slaveip, QString msgStr);

    // 向ServerThread列表中的每个线程socket发文件，也就是给从控机当前连接的所有socketTCP文件
    void sendFileToSlaves(QString sendFilename);
    void sendFileToOneSlave(QString ipaddress, QString sendFilename);
    /******************************/
    /**
     * 得到当前的多文件发送类对象
     */
    FileListSender* getCurrentFileListSender();
    /**
     * 得到当前的文件接收服务器对象
     */
    FileReceiverServer * getCurrentFileReceiverServer();

    /**
     * 设置文件传输-接收文件存放目录
     * @return true 设置成功，不成功可能由于正在接受文件
     */
    bool setReceiveFileSavingPath(QString newpath);

    /**
     * 返回文件传输时告知界面的主机索引，默认为IP地址
     */
    virtual QString getFileReceiveSenderIP(QString ip);
	
signals:
    void signalMsgToMaster(const QString& str);
    void signalErrorToMaster(const QString& str);

    /******【方案1】*******/
    // 准备发给子线程的消息，依然需要通过消息槽机制来实现
    void msgToBeSent(const QString& str);
    void msgToBeSentToOneSlave(int slaveid, QString& msgStr);
    void msgToBeSentToOneSlave(QString& slaveip, QString& msgStr);

private slots:
    /**
     * TcpServer获得新的从控TcpSocket连接
     * (1) tempSocket = tcpServer->nextPendingConnection();
     * (2) tempSocket 赋给 自定义的ServerThread，并设置信号槽
     * (3) 将ServerThread保存到内存threads队列中
     * @override QTcpServer的槽函数:收到TCP连接请求
     */
    virtual void incomingConnection();
    /**
     * 从内存threads队列中销毁对应slaveip的ServerThread
     * @param slaveip 即将销毁的从机ip地址
     */
    virtual void removeServerThread(QString slaveip);
    
    /**
     * 心跳发包计时器
     */
    void onTimeOut();

protected slots:

    /**
     * 文件传输相关，接收FileSender和FileReceiverServer的消息
     */
    virtual void updateClientProgress(QString ip, QString file, qint64 total, qint64 numBytes, int type);
    virtual void fileError(QString ip, QString file, QString err, int type);
    virtual void fileopenError(QString ip, QString file, int type);
    virtual void fileend(QString ip, QString file, int filesize, int timespend, int type);

    virtual void printReceivedMsg(QString receivedMsg);
    virtual void printReceivedError(QString receivedError);
};

#endif // MULTITHREADTCPSERVER_H
