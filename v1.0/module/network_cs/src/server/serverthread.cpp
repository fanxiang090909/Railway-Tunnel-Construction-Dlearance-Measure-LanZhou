#include "serverthread.h"
#include <QtNetwork>
//#include "serverprogram.h"

/**
 * 主控机服务器线程类实现（已经不再是线程）
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-2-11
 */

/**
 * 服务器线程构造函数
 * 所维护的tcpSocket置为空
 * numOfThreads记录当前维护线程数
 */
ServerThread::ServerThread(QObject *parent)
 : QObject(parent)
{
    qDebug() << tr("构造thread对象");
    tcpSocket = NULL;
    clientip = "";
    clientidstr = "";
}

/**
 * 服务器线程析构函数
 * 【注意】不能再这里删除socket，，，用delete tcpSocket;
 *       而是用tcpSocket的deleteLater
 */
ServerThread::~ServerThread()
{
    qDebug() << tr("析构thread对象") << this->clientip;
    qDebug() << "delete thread!" << QThread::currentThreadId();
    if (tcpSocket != NULL)
        delete tcpSocket;
}

/**
 * 槽函数 readMessageFromSlave
 */
void ServerThread::readMessageFromSlave() //读取信息
{
    //qDebug() << "ServerThread::readMessageFromSlave" << QThread::currentThreadId();
    mutex3.lock();
    QByteArray qba = tcpSocket->readAll(); //读取
    mutex3.unlock();
    QString ss = QVariant(qba).toString();
    // qDebug() << ss << "in child thread slot:" << currentThreadId();
    // @author 范翔 @date 20140518 改防止tcpSocket->readAll()方法读出的消息合并粘连，用$分割解析
    QStringList slist = ss.split("$", QString::SkipEmptyParts);
    for (int i = 0; i < slist.length(); i++)
    {
        if (slist.at(i).trimmed().compare("") == 0) // 如果是空
            continue;
        else
            // 发出信号给主线程
            emit signalMsg(QString("%1,%2").arg(clientidstr).arg(slist.at(i)));
    }
}

/**
 * 槽函数 displayErrorFromSlave
 */
void ServerThread::displayErrorFromSlave(QAbstractSocket::SocketError)
{
    // 输出错误信息
    qDebug() << "error from:" << QThread::currentThreadId() << tcpSocket->errorString();

    // 发出信号给主线程
    if (tcpSocket->errorString().compare("The remote host closed the connection") == 0)
    {
        ;//tcpSocket->close();
        //emit signalMsg(QString("%1,-8").arg(clientidstr));
        //@author范翔【注】此处不能用以下三句，因为这三句话会调用析构函数，delete tcpSocket;tcpSocket = NULL;deleteLater();
        // 【注意】 emit closeConnection会触发delete本对象socket的函数，因此tcpSocket->close() 必须在之前操作
        //emit closeConnection(clientidstr);
    }
    else
        emit signalError(QString("%1,%2").arg(clientidstr).arg(tcpSocket->errorString()));
}

void ServerThread::emitdisconnect()
{
    emit signalMsg(QString("%1,-8").arg(clientidstr));
    //@author范翔【注】此处不能用以下三句，因为这三句话会调用析构函数，delete tcpSocket;tcpSocket = NULL;deleteLater();
    // 【注意】 emit closeConnection会触发delete本对象socket的函数，因此tcpSocket->close() 必须在之前操作
    emit closeConnection(clientip);
}

/**
 * 得到该线程维护的socket，每个线程维护一个socket
 */
QTcpSocket * ServerThread::getTcpSocket()
{
    if (tcpSocket != NULL)
        return tcpSocket;
    else
        return NULL;
}
void ServerThread::setTcpSocket(QTcpSocket* newsocket)
{
    if (tcpSocket != NULL)
        delete tcpSocket;
    tcpSocket = newsocket;
    initTcpSocket();
}

QString ServerThread::getClientIP() { return clientip; }
void ServerThread::setClientIPStr(QString str) { clientidstr = str; }

void ServerThread::initTcpSocket()
{
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(emitdisconnect()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessageFromSlave()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayErrorFromSlave(QAbstractSocket::SocketError)), Qt::DirectConnection);

    /*
    // 一个主线程和n个子线程通信怎么搞?这样用消息槽搞不定，消息的参数无法传入子线程
    connect(ServerProgram::getServerProgramInstance()->getMultiThreadTcpServer(),
            SIGNAL(msgToBeSent(const QString&)), this, SLOT(sendMessage(const QString&)), Qt::DirectConnection);
    connect(ServerProgram::getServerProgramInstance()->getMultiThreadTcpServer(),
            SIGNAL(msgToBeSentToOneSlave(QString&, QString&)), this, SLOT(sendMessageToOne(QString&, QString&)), Qt::DirectConnection);
    */
    QHostAddress address = tcpSocket->peerAddress();
    clientip = address.toString();
    setClientIPStr(clientip);
    qDebug() << "address:" << clientip;

    emit signalMsg(QString("%1,-7").arg(clientidstr));
    qDebug() << "init ok";
}

/**
 * 给每一个服务器thread维护的TcpSocket发送消息
 */
void ServerThread::sendMessage(const QString& msgStr) //发送信息
{
    qDebug() << "ServerThread::sendMessage" << QThread::currentThreadId();

    // 接收中文字符
    // 【作废】
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
}

/**
 * 向其中之一线程socket发消息，即给该线程维护的socketTCP消息
 */
void ServerThread::sendMessageToOne(QString& toslaveip, QString& msgStr) //发送信息
{
    // 如果clientip号不相符，不发送消息
    if (this->clientip != toslaveip)
        return;
    // 接收中文字符
    // 【作废】
    /*QTextCodec * code = QTextCodec::codecForName("gb2312");
    std::string strStd = msgStr.toStdString();
    if (code)
        strStd = code->fromUnicode(msgStr).data();*/
    QByteArray ba = (msgStr + "$").toLocal8Bit();
        // const char *cstr = strcpy(ba.constData());

    mutex.lock();
    // qDebug() << "in send message func: " << msgStr;
    tcpSocket->write(ba.constData(), strlen(ba.constData())); //发送
    tcpSocket->waitForBytesWritten();
    mutex.unlock();
    //qDebug() << "to client " << clientip << ":" << msgStr;
}
