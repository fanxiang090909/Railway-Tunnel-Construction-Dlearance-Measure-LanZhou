#include "filesender.h"
#include <QDebug>

/**
 * 带断点的文件传输类定义
 * @author 冯明晨
 * @date 201401
 * @version 1.0.0
 */
FileSender::FileSender(QObject* obj) : QObject(obj)
{
    qDebug() << tr("构造filesender");

    loadSize = 10*1024;
    totalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;
    breakflag=false;
    breaklabel=0;

    // 默认IP自己，端口号8888
    ip = "127.0.0.1";
    port = 8888;
}

FileSender::FileSender(QString toip, int toport)
{
    qDebug() << tr("构造filesender");

    loadSize = 10*1024;
    totalBytes = 0;
    bytesWritten = 0;
    bytesToWrite = 0;
    breakflag=false;
    breaklabel=0;

    ip = toip;
    port = toport;
}


FileSender::~FileSender()
{
    qDebug() << tr("析构filesender");
}

// 配置IP和端口号
void FileSender::setIP(QString ipadd) { ip = ipadd; }
void FileSender::setPort(int p) { port = p; }
QString FileSender::getCurrentFilename() { return fileName; }

void FileSender::ClientStart()
{
    tcpClient = new QTcpSocket();
    //当连接服务器成功时，发出connected()信号，我们开始传送文件
    //connect(tcpClient,SIGNAL(connected()),this,SLOT(startTransfer()));
    //当连接服务器成功时，发出connected()信号，我们开始传送文件
    connect(tcpClient, SIGNAL(bytesWritten(qint64)), this, SLOT(FileBackup(qint64)), Qt::DirectConnection);
    //当有数据发送成功时，我们更新进度条
    connect(tcpClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)), Qt::DirectConnection);
}

bool FileSender::send() {
    bytesWritten = 0;//初始化已发送字节为0
    tcpClient->connectToHost(ip,port);//连接
    bool connected = tcpClient->waitForConnected(10000);
    if (!connected)
    {
        qDebug() << "file sender cannot connected to ip!!" << ip << port;
        return false;
    }
    qDebug() << "file sender connected to ip";

    // 记录开始发送时间
    startTime = QDateTime::currentDateTime();

    return true;
}

void FileSender::startTransfer(){
    qDebug() << "file sender $$$$$$$$$$$$$$$" << fileName;

    // 打开当前文件
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly))
    {
        qDebug() << "file sender open file error!";
        emit error_openfile(ip, fileName, 1);
        delete localFile;
        localFile = NULL;
        // 【注】去掉如下两句，由FileReceiver断开连接，这里只要发现断开连接异常再处理，@see displayError
        //tcpClient->close();
        // 【注意】 emit closeConnection会触发delete本对象socket的函数，因此delete localFile; 必须在之前操作
        //emit closeConnection(tcpClient->peerAddress().toString());
        return;
    }
    totalBytes = localFile->size();//文件总大小

    // 读出文件名与断点值
    /*QFile file("breakpoint_sender.txt");//读断点值
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"file sender log file failed!";
    }
    breaknum = file.readLine().trimmed().toInt();
    file.close();*/

    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_8);
    QString currentFileName = fileName.right(fileName.size()-fileName.lastIndexOf('/')-1);
    sendOut << qint64(0) << qint64(0) << currentFileName;

    //依次写入总大小信息空间，文件名大小信息空间，文件名
    totalBytes += outBlock.size();
    qDebug() <<"file sender FFF"<<outBlock.size();
    break_offset=outBlock.size();
    //这里的总大小是文件名大小等信息和实际文件大小的总和
    sendOut.device()->seek(0);
    sendOut<<totalBytes<<qint64((outBlock.size() - sizeof(qint64)*2));
    //返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    bytesToWrite = totalBytes - tcpClient->write(outBlock);
    //发送完头数据后剩余数据的大小
    outBlock.resize(0);
    /*if(breaknum>0)
    {
        bytesWritten=breaknum;
        qDebug() <<"file sender byte"<<bytesWritten;
        localFile->seek(bytesWritten-break_offset);
        bytesToWrite=totalBytes-bytesWritten;
        breakflag=true;
    }*/
}

void FileSender::FileBackup(qint64 numBytes) //更新进度条，实现文件的传送
{
    qDebug() << "file sender *************" << bytesWritten;
    if (breakflag)
    {
        bytesWritten-=break_offset;//减去文件名长度
        breakflag=false;
    }
    bytesWritten += (int)numBytes;
    if (breaklabel==1)
    {
        //把断点写入breakpoint文件
        /*QFile file("breakpoint_sender.txt");//写断点文件
        if(!file.open(QIODevice::WriteOnly)){
            qDebug()<<"file sender log file failed!";
        }
        QTextStream out(&file);
        out<<bytesWritten<<endl;
        file.close();*/
        localFile->close();
        delete localFile;
        localFile = NULL;
        tcpClient->close();
        breaklabel=0;
        qDebug() << "file sender " << bytesWritten;
    }

    //已经发送数据的大小,如果已经发送了数据
    if (bytesToWrite > 0) 
    {
        outBlock = localFile->read(qMin(bytesToWrite,loadSize));
        // 每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，
        // 就发送剩余数据的大小
        bytesToWrite -= (int)tcpClient->write(outBlock);
        // 发送完一次数据后还剩余数据的大小
        outBlock.resize(0);
        // 清空发送缓冲区
    }
    else // 如果没有发送任何数据，则关闭文件
    {
        localFile->close(); 
    }
    emit current_progress(ip, fileName, totalBytes, bytesWritten, 1);
    // 发送完毕
    if (bytesWritten == totalBytes)
    {
        localFile->close();
        delete localFile;

        qDebug() <<"file sender send succ";
        // 发送文件传输结束信号
        emit end_transfer(ip, fileName, totalBytes / 1000, startTime.secsTo(QDateTime::currentDateTime()), 1);

        // clear for next send
        localFile = NULL;
        bytesWritten = 0; 
        bytesToWrite = 0;
        totalBytes = 0;

        // 传输完成把断点设置为0
        /*QFile file1("breakpoint_sender.txt");
        if (!file1.open(QIODevice::WriteOnly))
        {
            qDebug()<<"file sender log file failed!";
        }
        QTextStream out1(&file1);
        out1<< 0 <<endl;
        file1.close();*/
        // 【注】去掉如下两句，由FileReceiver断开连接，这里只要发现断开连接异常再处理，@see displayError
        //tcpClient->close();
        //emit closeConnection(tcpClient->peerAddress().toString());
    }
}

void FileSender::displayError(QAbstractSocket::SocketError) //显示错误
{
    qDebug() << "file sender socket error " << tcpClient->errorString();

    if (tcpClient->errorString().compare("The remote host closed the connection") == 0)
    {
        tcpClient->close();
        // @author范翔【注】此处不能用以下三句，因为这三句话会调用析构函数，delete tcpSocket;tcpSocket = NULL;deleteLater();
        // 【注意】 emit closeConnection会触发delete本对象socket的函数，因此tcpSocket->close() 必须在之前操作
        emit closeConnection(tcpClient->peerAddress().toString());
    }
    else
        emit error_transfer(ip, fileName, QString("%1").arg(tcpClient->errorString()), 1);
}

// 开始（继续）传输文件
void FileSender::startTransferFile(QString filename)
{
    breaklabel=0;
    fileName = filename;
    qDebug() <<"file sender start transfer " << filename;
    QDir my_dir;
    if(!my_dir.exists(filename))
    {
        qDebug() << tr("file sender no such file") << filename;
        return;
    }
    startTransfer();
}

// 暂停
void FileSender::suspendTransferFile()
{

    breaklabel=1;
    qDebug() <<"file sender ##########";
}
