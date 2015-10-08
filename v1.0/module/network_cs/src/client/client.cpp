#include "client.h"
#include <QtCore/QObject>
#include <QtCore/QtDebug>
#include <qtcore/QDateTime>

/**
 * 从控机底层网络连接类定义
 * @author 冯明晨
 * @author 熊雪
 * @author 范翔
 * @date 2014-2-12
 * @version 1.0.0
 */
Client::Client(QString masterip, bool autoconnect, int msgSendServerPort, int fileSendServerPort, int fileReceivePort, QObject *parent) : msgSendServerPort(msgSendServerPort), fileSendServerPort(fileSendServerPort), fileReceivePort(fileReceivePort)
{
    qDebug() << tr("构造Client");

    // 是否自动连接，若isautoconnect == true，每次连接断开后自动发起连接，else断开就断开
    isautoconnect = autoconnect;

    // @author 范翔添加
    // 配置一次多次调用
    masterIPAddress = QHostAddress(masterip);

    //【方案二】
    /**********************/
    m_tcpSocket = new QTcpSocket();
    connect(m_tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(m_tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readMsgFromMaster()));
    connect(m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    m_bConnected = false;
	/**********************/

    // 多文件发送类(串行发送)
    fileListSender = new FileListSender();
    connect(fileListSender, SIGNAL(error_openfile(QString, QString, int)), this, SLOT(fileopenError(QString, QString,int)));
    connect(fileListSender, SIGNAL(current_progress(QString, QString, qint64, qint64, int)), this, SLOT(updateClientProgress(QString, QString, qint64, qint64,int)));
    connect(fileListSender, SIGNAL(error_transfer(QString, QString, QString, int)), this, SLOT(fileError(QString, QString, QString, int)));
    connect(fileListSender, SIGNAL(end_transfer(QString, QString, int, int, int)), this, SLOT(fileend(QString, QString, int, int, int)));

    // 多文件接收类,多线程并行接收不同主机发来的文件
    fileReceiverServer = new FileReceiverServer(this->fileReceivePort);
    connect(fileReceiverServer, SIGNAL(error_openfile(QString, QString, int)), this, SLOT(fileopenError(QString, QString,int)));
    connect(fileReceiverServer, SIGNAL(current_progress(QString, QString, qint64, qint64, int)), this, SLOT(updateClientProgress(QString, QString, qint64, qint64,int)));
    connect(fileReceiverServer, SIGNAL(error_transfer(QString, QString, QString, int)), this, SLOT(fileError(QString, QString, QString, int)));
    connect(fileReceiverServer, SIGNAL(end_transfer(QString, QString, int, int, int)), this, SLOT(fileend(QString, QString, int, int, int)));

    // 心跳发包
    timer = new QTimer();
    timer->setInterval(100000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
    timer->start();
}

Client::~Client()
{
    qDebug() << tr("析构Client");
    if (m_tcpSocket != NULL)
        delete m_tcpSocket;

    if (fileListSender != NULL)
    	delete fileListSender;

    if (fileReceiverServer != NULL)
        delete fileReceiverServer;

    if (timer != NULL)
    {
        timer->stop();
        delete timer;
    }
}

//【方案二】
// 
// 开始尝试连接
/**
 * 开始尝试连接，若isautoconnect == true只能调用一次
 * @return 0 连接成功
 *         1 连接失败（isautoconnect == false时才会用，否则isautoconnect == true会等待直到连接成功）
 */
int Client::start()
{
    m_bConnected = false;

    bool a;
    while(!m_bConnected)
	{        
        m_tcpSocket->connectToHost(masterIPAddress, this->msgSendServerPort);
        a = m_tcpSocket->waitForConnected(10000);

        // 若不是自动连接，可以不等待开始就连接
        if (!m_bConnected && !isautoconnect)
        {
            return 1;
        }
        qDebug() << "connected?" << a;
    }
    return 0;
}

void Client::onConnected()
{
    m_bConnected = true;
    // 开启连接信号告知从控机上层应用
    emit transmitmsg("-1");
    // 告知上层界面显示
    emit signalSocketError(true);

    // 设置文件发送和接收类
    qDebug() << "ok";
}

void Client::onDisconnected()
{
    m_bConnected = false;
    // 告知上层界面显示
    emit signalSocketError(false);
    // 断开连接信号告知从控机上层应用
    emit transmitmsg("0");

    bool a;
    while(!m_bConnected)
    {
        // 若不是自动连接，断开就断开
        if (!isautoconnect)
        {
            break;
        }
        m_tcpSocket->connectToHost(masterIPAddress, this->msgSendServerPort);
        a = m_tcpSocket->waitForConnected(10000);
        qDebug() << "connected?" << a;
    }
    qDebug() << "disconnect!";
}

void Client::displayError(QAbstractSocket::SocketError) //显示错误
{
    qDebug() << "socket error:" << m_tcpSocket->errorString();
}

void Client::onTimeOut()
{
    if (m_bConnected)
        sendMsgToMaster("-100");
}

void Client::sendMsgToMaster(const QString & msg)
{
	QByteArray tempstr = (msg + "$").toLocal8Bit();
	const char *cstr = tempstr.constData();
    if (NULL == m_tcpSocket)
    {
        qDebug()<<"Can't connect to master...";
    }
    else
    {
        // @author fanxiang  @date 20140518改,防止消息合并"粘粘",加入末尾标识符$
        // @see MultiThreadServer 的 readMessageFromSlave方法
        // 接收中文字符
        mutex.lock();
        // 【注意！】QString转cchar不能用QString().toStdString().c_str()，否则可能会出问题
        //qDebug() << tempstr.constData();
		m_tcpSocket->write(cstr, strlen(cstr));
		m_tcpSocket->waitForBytesWritten();
        mutex.unlock();

        qDebug() << "msg to server" << msg << tempstr.constData();
        // qDebug()<<msg.toStdString().c_str();
    }
}

void Client::readMsgFromMaster()
{
    mutex2.lock();
    QByteArray  qba = m_tcpSocket->readAll();
    mutex2.unlock();
    QString recmsg = QVariant(qba).toString();
    // fanxiang改【注意】改不能用 recmsg.toStdString().c_str();，否则VS编译段错误
    QString ss = QVariant(qba).toString();
    //qDebug() << "in child thread slot:" << ss;
    // @author 范翔 @date 20140518 改防止tcpSocket->readAll()方法读出的消息合并粘连，用$分割解析
    QStringList slist = ss.split("$", QString::SkipEmptyParts);
    for (int i = 0; i < slist.length(); i++)
    {
        if (slist.at(i).trimmed().compare("") == 0) // 如果是空
            continue;
        else
        {
            // 发出信号给主线程
            qDebug() << slist.at(i);
            emit transmitmsg(slist.at(i));
        }
    }

}

/**
 * 向主控机服务器发送文件
 */
void Client::sendFileToServer(QString ipaddress, QString fileName)
{
    // 添加到发送文件队列
    FileToSend newtask;
    newtask.toip = ipaddress;
    newtask.toport = this->fileSendServerPort;
    newtask.filename = fileName;
    bool ret = fileListSender->pushBack(newtask);

	fileListSender->printToSendList();

    if (ret)
        qDebug() << "start send file to ip: " << ipaddress << ", filename:" << fileName << "now";
    else
        qDebug() << "start send file to ip: " << ipaddress << ", filename:" << fileName << "later";
}

// 文件传输相关，接收fileSender的消息
void Client::updateClientProgress(QString ip, QString currentfile, qint64 total, qint64 numBytes, int type)
{

}

void Client::fileopenError(QString ip, QString currentfile, int type)
{
    emit transmitmsg(QString(tr("-10,%1,%2").arg(type).arg(currentfile)));
}

void Client::fileend(QString ip, QString currentfile, int filesize, int timespendint, int type)
{
    emit transmitmsg(QString(tr("-9,%1,%2,%3,%4").arg(type).arg(currentfile).arg(filesize).arg(timespendint)));
}

void Client::fileError(QString ip, QString currentfile, QString err, int type)
{
    emit transmitmsg(QString(tr("-11,%1,%2,%3").arg(type).arg(currentfile).arg(err)));
}

/**
 * 设置文件传输-接受文件存放目录
 * @author 范翔
 */
bool Client::setCurrentSavingPath(QString newpath)
{
    return fileReceiverServer->setCurrentSavingPath(newpath);
}
