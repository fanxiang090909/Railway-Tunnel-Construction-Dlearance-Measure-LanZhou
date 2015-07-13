#ifndef SERVERTHREAD_OFFICE_H
#define SERVERTHREAD_OFFICE_H

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
class OfficeServerThread : public ServerThread
{
    Q_OBJECT

public:
    OfficeServerThread(QObject *parent = 0);

    /**
     * 服务器线程析构函数
     * 用tcpSocket的deleteLater销毁该线程维护的tcpsocket
     */
    ~OfficeServerThread();

    void initTcpSocket();

};

#endif // SERVERTHREAD_OFFICE_H

