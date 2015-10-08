#ifndef CLIENT_H
#define CLIENT_H
#include <QtNetwork>
#include <QHostaddress>
#include <QTimer>
#include "filelistsender.h"
#include "filereceiverserver.h"

/**
 * 从控机底层网络连接类声明
 * @author 冯明晨
 * @author 熊雪
 * @author 范翔
 * @date 2014-2-12
 * @version 1.0.0
 */
class Client : public QObject  //这个类用来与主控交互，互相传送消息、文件
{
    Q_OBJECT
public:
    explicit Client(QString masterip, bool autoconnect = true, int msgSendServerPort = 9424, int fileSendServerPort = 7777, int fileReceivePort = 8888, QObject *parent = 0);
    
    ~Client();
	
    /**
     * 开始尝试连接，若isautoconnect == true只能调用一次
     * @return 0 连接成功
     * @return 1 连接失败（isautoconnect == false时才会用，否则isautoconnect == true会等待直到连接成功）
     */
    int start();

    /**
     * 向服务器发送消息
     * @param msg 要发送的消息
     */
    void sendMsgToMaster(const QString & msg);

private:

    /**
     * 消息发送TcpSocket的服务器端端口号
     */
    int msgSendServerPort;

    /**
     * 文件发送TcpSocket的服务器端端口号
     */
    int fileSendServerPort;

    /**
     * 文件接收TcpServer端口号
     */
    int fileReceivePort;

    /**
     * 是否正在连接
     */
    bool m_bConnected;

    /**
     * 是否自动连接，若isautoconnect == true，每次连接断开后自动发起连接，else断开就断开
     * 只在构造函数时赋值，default为true
     */
    bool isautoconnect;

    /**
     * 主机IP地址
     */
    QHostAddress masterIPAddress;

    /**
     * 接收和发送消息的QTcpSocket
     */
    QTcpSocket *m_tcpSocket;
    // 发送消息的互斥信号量
    QMutex mutex;
    // 接受消息的互斥信号量
    QMutex mutex2;

    /**
     * 心跳发包计时器
     */
    QTimer* timer;

    /**
     * 多文件发送类(串行发送)
     */
    FileListSender * fileListSender;
    /**
     * 多文件接收类,多线程并行接收不同主机发来的文件
     */
    FileReceiverServer * fileReceiverServer;

signals:
    void transmitmsg(QString);

    // 告知上层应用
    void signalSocketError(bool);
    // @author 熊雪
    void sendtosendsourceimage(int,int,QString);

private slots:
    /**
     * 接收主控发送的消息
     */
    void readMsgFromMaster();

    /**
     * tcpscokect错误处理机制,包括断开连接时选择重连
     */
    void displayError(QAbstractSocket::SocketError);

    /**
     * 建立连接槽函数
     */
    void onConnected();
    /**
     * 断开连接槽函数
     */
    void onDisconnected();
    /**
     * 心跳发包计时器
     */
    void onTimeOut();

    /**
     * 接收发送文件需要
     * @param type 0表示接收，1表示发送
     * @author fanxiang
     * @date 2014-5-10
     */
    // 文件传输相关，接收fileSender，fileSender的消息
    void updateClientProgress(QString ip, QString filename, qint64 total, qint64 numBytes, int type);
    void fileError(QString ip, QString file, QString err, int type);
    void fileopenError(QString ip, QString file, int type);
    void fileend(QString ip, QString file, int filesize, int timespend, int type);

public:
    /**
     * 发送文件需要
     * @author fanxiang
     * @date 2013-11-25
     */
    void sendFileToServer(QString ipaddress, QString fileName);

    /**
     * 设置文件传输-接受文件存放目录
     * @author 范翔
     * @return true设置成功,false可能因为当前正在接收文件而设置不成功
     */
    bool setCurrentSavingPath(QString newpath);
};

#endif // CLIENT_H
