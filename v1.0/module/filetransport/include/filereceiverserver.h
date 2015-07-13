#ifndef FILE_RECEIVER_SERVER_H
#define FILE_RECEIVER_SERVER_H

#include <QTcpServer>
#include <QList>
#include <QFile>

#include "filereceiver.h"

/**
 * 多文件接收服务器类设计
 * 底层为多线程TCPScoket
 * 考虑接收端不能保证同时只接收一个IP的文件，但可以保证任意时刻只接收一个IP的一个文件（@see FileListSender 定义）
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-6-6
 */
class FileReceiverServer : public QObject
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
     * 静态变量：threads服务器服务程序维护的线程列表
     */
    QList<FileReceiver*> receivers;

    /**
     * 静态变量：numOfThreads记录当前维护线程数
     */
    int numOfThreads;

    /******【方案2】*******/
    // 使用方案二，获得线程的socket指针，发送网络消息
    //QTcpSocket * msgSocket;
    QMutex slavercdmutex;

    // 是否正在发送接收
    QMutex istranferfilereceive;
    bool istransferfile_receive;

    /**
     * 所有接收文件存放路径
     */
    QString savingpath;

    /**
     * 监听端口配置
     */
    int port;

public:
    /**
     * 构造函数
     * @param initport Server监听端口号 初始化参数
     */
    FileReceiverServer(int initport, QObject * parent = 0);

    virtual ~FileReceiverServer();

    /**
     * qDebug打印当前连接所有ip
     */
    void printSlavesSockets();

    /**
     * 设置文件传输-接收文件存放目录
     * @return true 设置成功，不成功可能由于正在接受文件
     */
    bool setCurrentSavingPath(QString newpath);

private slots:
    /**
     * TcpServer获得新的从控TcpSocket连接
     * (1) tempSocket = tcpServer->nextPendingConnection();
     * (2) tempSocket 赋给 自定义的FileReceiver，并设置信号槽
     * (3) 将FileReceiver保存到内存threads队列中
     * @override QTcpServer的槽函数:收到TCP连接请求
     */
    virtual void incomingConnection();
    /**
     * 从内存receivers队列中销毁对应slaveip的FileReceiver
     * @param slaveip 即将销毁的从机ip地址
     */
    void removeServerThread(QString slaveip);
    
    /**
     * 文件传输相关，接收FileReceiver的消息
     * 不做处理，继续向上抛出，上层再做处理解析
     */
    virtual void updateClientProgress(QString ip, QString file, qint64 total, qint64 numBytes, int type);
    virtual void fileError(QString ip, QString file, QString err, int type);
    virtual void fileopenError(QString ip, QString file, int type);
    virtual void fileend(QString ip, QString file, int filesize, int timespend, int type);

signals:
    /**
     * 文件接收有关信号-文件打开错误
     * @param _ip ip地址
     * @param file 文件名
     * @param type integer型type 0表示接受，1表示发送
     */
    void error_openfile(QString ip, QString file, int type);
    void current_progress(QString ip, QString file, qint64 total, qint64 written, int type);
    void end_transfer(QString ip ,QString file, int filesize, int timespend, int type);
    void error_transfer(QString ip, QString file, QString err, int type);
};

#endif // FILE_RECEIVER_SERVER_H
