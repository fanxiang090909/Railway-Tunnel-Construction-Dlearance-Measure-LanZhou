#ifndef SERVERTHREAD_MASTER_H
#define SERVERTHREAD_MASTER_H

#include <QThread>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMutex>
#include <QFile>

#include "serverthread.h"

/**
 * 主控机服务器线程类设计（已经不再是线程）
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-23
 */
class MasterServerThread : public ServerThread
{
    Q_OBJECT

public:
    MasterServerThread(QObject *parent = 0);

    /**
     * 服务器线程析构函数
     * 用tcpSocket的deleteLater销毁该线程维护的tcpsocket
     */
    ~MasterServerThread();

    /********【方案1】thread的list作为静态变量*****/
    static QList<MasterServerThread*> threads;

    int getSlaveID();

    void initTcpSocket();

private:

    // 标识该从控机id号，在新建连接initTcpSocket方法中查找Setting::slaveList中存储的id号
    int slaveid;

public slots:

    /**
     * 向其中之一线程socket发消息，即给该线程维护的socketTCP消息
     */
    virtual void sendMessageToOne(int slaveid, QString& msgStr);

};

#endif // SERVERTHREAD_MASTER_H

