#include "serverthread_master.h"
#include <QtNetwork>
#include "masterprogram.h"
#include "network_config_list.h"

/**
 * 主控机服务器线程类实现（已经不再是线程）
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2013-11-25
 */

/**
 * 服务器线程构造函数
 * 所维护的tcpSocket置为空
 * numOfThreads记录当前维护线程数
 */
MasterServerThread::MasterServerThread(QObject *parent)
 : ServerThread(parent)
{
    qDebug() << tr("构造Masterthread对象");
    //tcpSocket = NULL;
    slaveid = -1;
    //clientip = "";
}

/**
 * 服务器线程析构函数
 * 【注意】不能再这里删除socket，，，用delete tcpSocket;
 *       而是用tcpSocket的deleteLater
 */
MasterServerThread::~MasterServerThread()
{
    qDebug() << tr("析构Masterthread对象") << this->slaveid;
    qDebug() << "delete thread!" << QThread::currentThreadId();
    //if (tcpSocket != NULL)
    //    delete tcpSocket;
}

int MasterServerThread::getSlaveID() { return slaveid; }

void MasterServerThread::initTcpSocket()
{
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(emitdisconnect()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessageFromSlave()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayErrorFromSlave(QAbstractSocket::SocketError)), Qt::DirectConnection);

    // 一个主线程和n个子线程通信怎么搞?这样用消息槽搞不定，消息的参数无法传入子线程
    connect(MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer(),
            SIGNAL(msgToBeSent(const QString&)), this, SLOT(sendMessage(const QString&)), Qt::DirectConnection);
    connect(MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer(),
            SIGNAL(msgToBeSentToOneSlave(int, QString&)), this, SLOT(sendMessageToOne(int, QString&)));
    connect(MasterProgram::getMasterProgramInstance()->getMultiThreadTcpServer(),
            SIGNAL(msgToBeSentToOneSlave(QString&, QString&)), this, SLOT(sendMessageToOne(QString&, QString&)), Qt::DirectConnection);

    //connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(mineDeleteLater()));
    QHostAddress address =  tcpSocket->peerAddress();
    clientip = address.toString();
    qDebug() << "address:" << clientip;

    mutex2.lock();
    slaveid = NetworkConfigList::getNetworkConfigListInstance()->findSlaveId(address.toString());
    setClientIPStr(QString("%1").arg(slaveid));
    mutex2.unlock();
    qDebug() << "slaveid:" << slaveid << clientip << clientidstr;

    if (slaveid < 0)
    {
       qDebug() << "not find slave ip in setting list";
    }
    emit signalMsg(QString("%1,-7").arg(slaveid));
    qDebug() << "init ok";
}

/**
 * 向其中之一线程socket发消息，即给该线程维护的socketTCP消息
 */
void MasterServerThread::sendMessageToOne(int toslaveid, QString& msgStr) //发送信息
{
    // 如果slaveid号不相符，不发送消息
    if (this->slaveid != toslaveid)
        return;
    // fanxiang
    // 接收中文字符
    /*QTextCodec * code = QTextCodec::codecForName("gb2312");
    std::string strStd = msgStr.toStdString();
    if (code)
        strStd = code->fromUnicode(msgStr).data();*/
    QByteArray ba = (msgStr + "$").toLocal8Bit();

    mutex.lock();
    // qDebug() << "in send message func: " << msgStr;
    tcpSocket->write(ba.constData(), strlen(ba.constData())); //发送
    tcpSocket->waitForBytesWritten();
    mutex.unlock();
    qDebug() << "to slave " << slaveid << ":" << msgStr;
}
