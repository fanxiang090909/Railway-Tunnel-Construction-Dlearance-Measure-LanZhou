#ifndef FILE_LIST_SENDER_H
#define FILE_LIST_SENDER_H

#include <QTcpServer>
#include <QList>
#include <QFile>
#include <QDebug>

#include "filesender.h"

/**
 * 待发送文件列表
 */
struct FileToSend
{
    QString filename; // 包含路径
    QString toip;     // 发送到的IP地址
    int toport;       // 发送到的端口号
};

/**
 * 多文件发送类设计
 * 单线程发送，发完一个后继续发送另外一个：考虑发送速率，单线程优于多线程
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-6-8
 */
class FileListSender : public QObject
{
    Q_OBJECT

public:
    /**
     * 构造函数
     */
    FileListSender(QObject * parent = 0);

    virtual ~FileListSender();

    /**
     * qDebug打印当前待发送的文件队列
     */
    void printToSendList();

    /**
     * 返回list
     */
    const QList<FileToSend>* list();
    /**
     * 插入到队列尾部
     * @return true 立即发送，false 正有其他文件在发送，加入队列稍后发送
     */
    bool pushBack(FileToSend item);

private:

    /**
     * 当前所用的FileSender
     */
    FileSender * fileSender;

    /**
     * 待发送的文件记录
     */
    QList<FileToSend>* tosendlist;

    // 是否正在发送
    QMutex istranferfilesend;
    bool istransferfile_send;
    
    /**
     * 从队列首部取出，队列中删除队首元素
     * @param ret 返回的FileToSend
     */
    bool popfront(FileToSend & ret);

    /**
     * 清空list
     */
    void clear();
        
    /**
     * 开始发送
     */
    virtual void startTransferNextFile();

private slots:
    
    /**
     * 从内存fileSender中销毁当前FileSender
     * @param slaveip 即将销毁的从机ip地址
     */
    void removeFileSender(QString slaveip);
    
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

#endif // FILE_LIST_SENDER_H
