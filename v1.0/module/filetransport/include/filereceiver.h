#ifndef FILERECEIVER_H
#define FILERECEIVER_H

#include <QtNetwork>

/**
 * 备份服务器端（含断点大文件接收端）类声明
 * @author 冯明晨
 * @date：2013-12-2
 */
class FileReceiver : public QObject//接收文件的服务器端类
{
    Q_OBJECT
public:
    explicit FileReceiver(QObject *parent = 0);

    ~FileReceiver();

    bool breaklabel;
    /**
     * 启动文件监听Tcp，开始监听，时刻准备接受文件
     */
    void start();

    /**
     * 设置监听端口配置
     * @param p 新端口号
     */
    void setPort(int p);

    /**
     * 设置存放路径
     * @param newpath 新路径
     */
    void setPath(QString newpath);

    /**
     * 得到该线程维护的socket，每个线程维护一个socket
     */
    virtual QTcpSocket * getTcpSocket();
    virtual void setTcpSocket(QTcpSocket* newsocket);

    virtual QString getReceiveIP();
    virtual void setReceiveIP(QString newip);

    /**
     * 虚函数，需要用到信号槽
     */
    virtual void initTcpSocket();

private:

    /**
     * 是否正在传输文件
     */
    bool fileisopen;
    /**
     * 正在接收文件发送端的ip
     */
    QString receivedip;
    /**
     * 存放路径
     */
    QString path;

    /**
     * 文件传输相关TCPSocket
     */
    QTcpSocket * tcpSocket;

    qint64 totalBytes;      //存放总大小信息
    qint64 bytesReceived;   //已收到数据的大小
    qint64 fileNameSize;    //文件名的大小信息
    QString fileName;       //存放文件名
    QFile *localFile;       //本地文件
    QByteArray inBlock;     //数据缓冲区

    /**
     * 存储位置全名
     */
    QString receivefilefullname;

    /**
     * 记录开始时间
     */
    QDateTime startTime;

private slots:
    /**
     * tcpsocket缓存满了接收数据槽函数
     */
    void FileBackup();

    /**
     * 错误检测处理、连接关闭，槽函数 displayError
     * 若是断开连接，发送消息closeConnection
     */
    void displayError(QAbstractSocket::SocketError socketError);

signals:

    /**
     * 关闭连接信号，给上层FileReceiveServer捕获
     */
    void closeConnection(QString);

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
#endif//FILERECEIVER_H
