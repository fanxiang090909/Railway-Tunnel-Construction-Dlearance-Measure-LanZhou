#ifndef FILESENDER_H
#define FILESENDER_H

#include <QtNetwork>
#include <QDebug>

/**
 * 带断点的文件传输类声明
 * @author 冯明晨
 * @date 201401
 * @version 1.0.0
 */
class FileSender : public QObject
{
    Q_OBJECT
public:
    FileSender(QObject * obj = 0);
    FileSender(QString toip, int toport);

    ~FileSender();
    bool send(); //连接服务器
    int breaklabel;//控制断点
    qint64 breaknum;//记录断点的值，用于在续传时判断
    void ClientStart();

    void startTransferFile(QString filename);

    // 配置IP和端口号
    void setIP(QString ipadd);
    void setPort(int p);
    /**
     * @return 保存文件名（含路径）
     */
    QString getCurrentFilename();

private:
    /**
     * 传送配置相关IP地址，往哪里发
     */
    QString ip;
    /**
     * 传送配置相关端口号，往哪里发
     */
    int port;
    /**
     * 保存文件名（含路径）
     */
    QString fileName;

    // 文件传输相关
    /**
     * 负责管理文件传输的Sockect
     */
    QTcpSocket *tcpClient;
    QFile *localFile; //要发送的文件
    qint64 totalBytes; //数据总大小
    qint64 bytesWritten; //已经发送数据大小
    qint64 bytesToWrite; //剩余数据大小
    qint64 loadSize; //每次发送数据的大小
    bool breakflag;
    int break_offset;
    QByteArray outBlock; //数据缓冲区，即存放每次要发送的数据

    /**
     * 记录开始时间
     */
    QDateTime startTime;

    /**
     * 开始发送文件
     * 发送文件大小等信息
     */
    void startTransfer(); 

private slots:
    /**
     * 文件中断发送，记录断点位置
     * @author 冯明晨
     */
    void suspendTransferFile();
		
    /**
     * tcpsocket缓存满了发送数据槽函数
     */
    void FileBackup(qint64); 
    /**
     * 错误处理槽函数，包括处理断开连接
     */
    void displayError(QAbstractSocket::SocketError); 

signals:

    /**
     * 关闭连接信号，给上层调用类(Client或MultiThreadServer)捕获
     */
    void closeConnection(QString);

    /**
     * 文件发送有关信号-文件打开错误
     * @param _ip ip地址
     * @param file 文件名
     * @param type integer型type 0表示接收，1表示发送
     */
    void error_openfile(QString _ip, QString file, int type);
    void current_progress(QString _ip, QString file, qint64 total, qint64 written, int type);
    void end_transfer(QString _ip, QString file, int filesize, int timespend, int type);
    void error_transfer(QString _ip, QString file, QString err, int type);

};

#endif // FILESENDER_H
