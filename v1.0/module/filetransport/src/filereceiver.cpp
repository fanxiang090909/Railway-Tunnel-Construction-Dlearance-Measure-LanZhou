#include "filereceiver.h"
#include <QDebug>

#include "filename_filter.h"
#include "file_decompress.h"

/**
 * 备份服务器端（含断点大文件接收端）类定义
 * @author 冯明晨
 * @author 范翔
 * @date：2013-12-2
 */
FileReceiver::FileReceiver(QObject *parent) : QObject(parent)
{
    qDebug() << tr("构造filereceiver");

    totalBytes = 0;
    bytesReceived = 0;
    fileNameSize = 0;
    breaklabel = false;
    fileisopen = false;

    // 路径
    path = ".";

    // 【注意】必须添加，否则setTcpSocket删除空指针异常
    tcpSocket = NULL;
}

FileReceiver::~FileReceiver()
{
    qDebug() << tr("析构filerecerver");
}

/**
 * 得到该线程维护的socket，每个线程维护一个socket
 */
QTcpSocket * FileReceiver::getTcpSocket()
{
    if (tcpSocket != NULL)
        return tcpSocket;
    else
        return NULL;
}
void FileReceiver::setTcpSocket(QTcpSocket* newsocket)
{
    if (tcpSocket != NULL)
        delete tcpSocket;
    tcpSocket = newsocket;
    initTcpSocket();
}

QString FileReceiver::getReceiveIP() { return receivedip; }
void FileReceiver::setReceiveIP(QString str) { receivedip = str; }

void FileReceiver::initTcpSocket()
{
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(FileBackup()), Qt::DirectConnection);
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(displayError(QAbstractSocket::SocketError)), Qt::DirectConnection);

    QHostAddress address = tcpSocket->peerAddress();
    receivedip = address.toString();
    setReceiveIP(receivedip);
    qDebug() << "address:" << receivedip;

    qDebug() << "init ok";
}

// 设置存放路径
void FileReceiver::setPath(QString newpath)
{
    path = newpath;
    qDebug() << "File Receiver set new received path :" << newpath;
}

//实现更新进度条函数
void FileReceiver::FileBackup()//接收数据
{
    qDebug() << "file receiver **********" << bytesReceived << totalBytes;

    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_8);
    if (bytesReceived <= sizeof(qint64)*2)
    {
        // 如果接收到的数据小于16个字节，那么是刚开始接收数据，我们保存到//来的头文件信息
        if ((tcpSocket->bytesAvailable() >= sizeof(qint64)*2) && (fileNameSize == 0))
        { 
            //接收数据总大小信息和文件名大小信息
            in >> totalBytes >> fileNameSize;
            bytesReceived += sizeof(qint64) * 2;
        }
        if ((tcpSocket->bytesAvailable() >= fileNameSize) && (fileNameSize != 0))
        {
            // 接收文件名，并建立文件
            in >> fileName;
            bytesReceived += fileNameSize;

            qDebug() << "file receiver "<< QObject::tr("接收文件 %1 …").arg(fileName);
            // @author范翔 文件名过滤代码，@param path参数是个引用
            qDebug() << "filereceive original path" << path;
            FILTER_FILENAME(fileName, path);

            receivefilefullname = QString("%1/%2").arg(path).arg(fileName);
            qDebug() << "filereceive after filtered path" << path << "file receive dir " << receivefilefullname;

            // 接收要写入的文件
            localFile = new QFile(receivefilefullname);

            // 记录开始接收时间
            startTime = QDateTime::currentDateTime();

            // 若文件存在,可能是续传文件
            //if (localFile->exists())
            //{
                // @author冯明晨 断点文件
                //如果文件存在会读断点文件，把断点值读入bytesReceived
                /*QFile file("breakpoint_receiver.txt");//读断点
                if(!file.open(QIODevice::ReadOnly))
                {
                    qDebug()<<"file receiver breakpoint file failed!";
                }
                bytesReceived=file.readLine().trimmed().toInt();
                localFile->open(QFile::Append);//如果文件存在，追加打开*/
            //}
            //else // 第一次接收(不是续传文件)
            //{
                // 如果目录不存在，创建该存放目录
                QDir receivedir;
                if (!receivedir.exists(path))
                {
                    receivedir.mkpath(path);
                }

                if(!localFile->open(QFile::WriteOnly))
                {
                    qDebug() << "file receiver open file error!";
                    emit error_openfile(receivedip, receivefilefullname, 0);
                    delete localFile;
                    localFile = NULL;
                    fileisopen = false;
                    tcpSocket->close();
                    // 【注意】 emit closeConnection会触发delete本对象socket的函数，因此delete localFile; 必须在之前操作
                    emit closeConnection(tcpSocket->peerAddress().toString());
                    return;
                }
                fileisopen = true;
            //}
        }
        else
            return;
    }
    // 如果接收的数据小于总数据，那么写入文件
    if (bytesReceived < totalBytes) 
    {   
        bytesReceived += tcpSocket->bytesAvailable();
        inBlock = tcpSocket->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);

        // 在这里写入断点文件
        /*QFile file1("breakpoint_receiver.txt");
        if (!file1.open(QIODevice::WriteOnly))
        {
            qDebug() << "file receiver log file failed!";
        }
        QTextStream out1(&file1);
        out1 << bytesReceived << endl;
        file1.close();*/
        emit current_progress(receivedip, path + "/" + fileName, totalBytes, bytesReceived, 0);
    }
    // 接收数据完成时
    if (bytesReceived == totalBytes)
    {   
        localFile->close();
        delete localFile;

        qDebug() << "file receiver rec succ";

        // 告知上层，文件接收结束，文件大小，传输时间
        emit end_transfer(receivedip,  path + "/" + fileName, totalBytes / 1000, startTime.secsTo(QDateTime::currentDateTime()), 0);
        
        // clear for next receive
        localFile = NULL;
        bytesReceived = 0;
        totalBytes = 0;
        fileNameSize = 0;

        fileisopen = false;

        QString fileimage1 = "rar";
        if(fileName.contains(fileimage1))
        {
            File_decompress::getFileDecompressDAOInstance()->setPathFileName(receivefilefullname);
            File_decompress::getFileDecompressDAOInstance()->deCompress();
        }
        /*QFile file1("breakpoint_receiver.txt");//传输完成把断点设置为0
        if(!file1.open(QIODevice::WriteOnly))
        {
            qDebug() << "file receiver log file failed!";
        }
        QTextStream out1(&file1);
        out1<< 0 << endl;
        file1.close();*/
        tcpSocket->close();
        emit closeConnection(tcpSocket->peerAddress().toString());
    }
}

/**
 * 错误检测处理、连接关闭，槽函数 displayError
 */
void FileReceiver::displayError(QAbstractSocket::SocketError)
{
    // 输出错误信息
    qDebug() << "file receiver error from:" << QThread::currentThreadId() << tcpSocket->errorString();

    // 发出信号给主线程
    if (tcpSocket->errorString().compare("The remote host closed the connection") == 0)
    {
        tcpSocket->close();
        if (fileisopen == true)
        {
            localFile->close();
            delete localFile;
            localFile = NULL;
            fileisopen = false;
        }
        //@author范翔【注】此处不能用以下三句，因为这三句话会调用析构函数，delete tcpSocket;tcpSocket = NULL;deleteLater();
        // 【注意】 emit closeConnection会触发delete本对象socket的函数，因此tcpSocket->close() 必须在之前操作
        emit closeConnection(tcpSocket->peerAddress().toString());
    }
    else
        emit error_transfer(receivedip,  path + "/" + fileName, QString("%1").arg(tcpSocket->errorString()), 0);

    if (fileisopen == true)
    {
        localFile->close();
        delete localFile;
        localFile = NULL;
        fileisopen = false;
    }
}
