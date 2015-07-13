#include "filelistsender.h"

#include <QFile>

/**
 * 多文件发送类实现
 * 单线程发送，发完一个后继续发送另外一个：考虑发送速率，单线程优于多线程
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 2014-6-8
 */
FileListSender::FileListSender(QObject * parent) : QObject(parent)
{
    tosendlist = new QList<FileToSend>();

    fileSender = new FileSender();
    connect(fileSender, SIGNAL(error_openfile(QString, QString, int)), this, SLOT(fileopenError(QString, QString, int)));
    connect(fileSender, SIGNAL(current_progress(QString,QString, qint64, qint64, int)), this, SLOT(updateClientProgress(QString, QString, qint64, qint64, int)));
    connect(fileSender, SIGNAL(error_transfer(QString, QString, QString, int)), this, SLOT(fileError(QString, QString, QString, int)));
    connect(fileSender, SIGNAL(end_transfer(QString, QString, int, int, int)), this, SLOT(fileend(QString, QString, int, int, int)));
    connect(fileSender, SIGNAL(closeConnection(QString)), this, SLOT(removeFileSender(QString)));

    // 设置文件发送和接收类
    istranferfilesend.lock();
    istransferfile_send = false;
    istranferfilesend.unlock();
}

/**
 * 析构函数，删除线程数组中的实例线程
 */
FileListSender::~FileListSender()
{
    delete tosendlist;

    if (fileSender != NULL)
        delete fileSender;
}

/**
 * qDebug打印当前待发送的文件队列
 */
void FileListSender::printToSendList()
{
    qDebug() << "*****ToSendList****"; 
    for (int i = 0; i < tosendlist->size(); i++)
    {
        FileToSend tmp = tosendlist->at(i);
        qDebug() << "\t" << tmp.toip << tmp.toport << tmp.filename;
    }
}


const QList<FileToSend>* FileListSender::list()
{
    return tosendlist;
}

/**
 * 插入到队列尾部
 * @return true 立即发送，false 正有其他文件在发送，加入队列稍后发送
 */
bool FileListSender::pushBack(FileToSend slavetaskModel)
{
    tosendlist->push_back(slavetaskModel);

    istranferfilesend.lock();
    if (istransferfile_send == false)
    {
        istranferfilesend.unlock();
        startTransferNextFile();
        return true;
    }
    else
    {
        istranferfilesend.unlock();
        return false;
    }
}

/**
 * 从队列首部取出，队列中删除队首元素
 * @param ret 返回的FileToSend
 */
bool FileListSender::popfront(FileToSend & ret)
{
    if (tosendlist->size() > 0)
    {
        ret = tosendlist->at(0);
        tosendlist->removeFirst();
        return true;
    }
    return false;
}

void FileListSender::clear()
{
    tosendlist->clear();
}

/**
 * 开始发送
 */
void FileListSender::startTransferNextFile()
{
    FileToSend topTaskFile;
    // 从队列中取出第一个元素
    if (popfront(topTaskFile))
    {
        printToSendList();

        fileSender->setPort(topTaskFile.toport);
        fileSender->setIP(topTaskFile.toip);
        fileSender->ClientStart();
        bool ret = fileSender->send();
        if (ret == false)
        {
            qDebug() << "cannot connect to ip: " << topTaskFile.toport << ", send filename:" << topTaskFile.filename;
            istranferfilesend.lock();
            istransferfile_send = false;
            istranferfilesend.unlock();
            return;
        }
        istranferfilesend.lock();
        istransferfile_send = true;
        istranferfilesend.unlock();

        fileSender->startTransferFile(topTaskFile.filename);
    }
}

void FileListSender::removeFileSender(QString slaveip)
{
    istranferfilesend.lock();
    istransferfile_send = false;
    //istranferfilesend.unlock();//改过，不知道是否有问题TODO
    qDebug() << "remove ok " << slaveip;

    //istranferfilesend.lock();
    if (istransferfile_send == false)
    {
        istranferfilesend.unlock();
        // 发送队列中下一个文件
        startTransferNextFile();
    }
    else
        istranferfilesend.unlock();
}

void FileListSender::fileopenError(QString ip, QString file, int type)
{
    emit error_openfile(ip, file, type);
}

void FileListSender::fileend(QString ip, QString file, int filesize, int timespend, int type)
{
    emit end_transfer(ip, file, filesize, timespend, type);
}

void FileListSender::updateClientProgress(QString ip, QString file, qint64 total, qint64 numBytes, int type)
{
    //emit
}

void FileListSender::fileError(QString ip, QString file, QString err, int type)
{
    emit error_transfer(ip, file, err, type);
}