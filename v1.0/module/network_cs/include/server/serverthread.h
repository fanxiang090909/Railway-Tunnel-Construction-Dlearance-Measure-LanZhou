#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMutex>
#include <QFile>

/**
 * 办公室服务器线程类设计
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-2-11
 */
class ServerThread : public QObject
{
    Q_OBJECT

public:
    ServerThread(QObject *parent = 0);

    /**
     * 服务器线程析构函数
     * 用tcpSocket的deleteLater销毁该线程维护的tcpsocket
     */
    virtual ~ServerThread();

    /**
     * 得到该线程维护的socket，每个线程维护一个socket
     */
    virtual QTcpSocket * getTcpSocket();
    virtual void setTcpSocket(QTcpSocket* newsocket);

    virtual QString getClientIP();
    virtual void setClientIPStr(QString str);

    // 纯虚函数，需要用到信号槽
    virtual void initTcpSocket();

signals:
    void error(QTcpSocket::SocketError socketError);

    void signalMsg(const QString& str);
    void signalError(const QString& str);

    /**
     * 关闭连接信号，给上层multithreadServer捕获
     */
    void closeConnection(QString);

protected:
    // 发送消息的互斥信号量
    QMutex mutex;
    // 接收消息的互斥信号量
    QMutex mutex3;

    QMutex mutex2;

    bool receive;

    QTcpSocket* tcpSocket;

    /**
     * tcp连接的对方客户端IP
     */
    QString clientip;
    QString clientidstr;

public slots:
    /**
     * 给每一个服务器thread维护的tcpSocket发送消息
     */
    virtual void sendMessage(const QString& msgStr);
    /**
     * 向其中之一线程socket发消息，即给该线程维护的socketTCP消息
     */
    virtual void sendMessageToOne(QString& toslaveip, QString& msgStr);

private slots:

    /**
     * 槽函数 readMessageFromSlave
     */
    virtual void readMessageFromSlave();

    /**
     * 槽函数 displayErrorFromSlave
     */
    void displayErrorFromSlave(QAbstractSocket::SocketError);
    /**
     * 如果为断开连接，发送关闭连接closeConnection信号，告知上层MultiThreadServer
     */
    void emitdisconnect();
};

#endif // SERVERTHREAD_H