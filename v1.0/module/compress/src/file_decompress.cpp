#include "file_decompress.h"
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QFileInfo>

/**
 * RAR文件解压类接口实现
 * @author 熊雪 范翔
 * @date 20140818
 * @version 1.0.0
 */
// singleton设计模式，静态私有实例变量
File_decompress * File_decompress::file_decompressInstance = NULL;

/**
 * singleton设计模式，公有静态方法返回单一实例
 * @return 单一实例
 */
File_decompress * File_decompress::getFileDecompressDAOInstance()
{
    if (file_decompressInstance == NULL)
    {
        file_decompressInstance = new File_decompress();
    }
    return file_decompressInstance;
}

File_decompress::File_decompress(QObject *parent) :
    QObject(parent)
{
    pathFileName = "./";
    hasinitpath = false;
    Rar_address = "C:/Program Files/WinRAR/Rar.exe";
}

// 设置图像存放路径
void File_decompress::setPathFileName(QString newpath)
{
    pathFileName = newpath;
    hasinitpath = true;
}

//解压源图像文件
bool File_decompress::deCompress()
{
    if (!hasinitpath)
    {
        qDebug() << "has not init deperessing path " << pathFileName;
        return false;
    }
    //将接收的文件进行解压文件
    //解压文件的cmd命令：QString sCmd = "Rar.exe x -ibck -y -o+ C:/test.rar C:/list";
    QFileInfo info(pathFileName);
    QString destpath = info.absolutePath();

    QFile file(pathFileName);
    if(file.exists())
    {
        // 加锁
        rarmutex.lock();

        //不是中文目录的原因，因为前面解压的变量在循环之外，所以只解压了一次
        QProcess pro1(0);
        QString Rar_address = getRarAddressname();
        QStringList t22;
        t22.append("x");
        t22.append("-ibck");
        t22.append("-y");
        t22.append("-o+");
        
        t22.append(pathFileName);
        t22.append(destpath);
        int bret1 = pro1.execute(Rar_address,t22);

        // 解锁
        rarmutex.unlock();

        qDebug() << pathFileName << QObject::tr("的文件解压到目录%1").arg(destpath) << bret1;
        emit signalParsedMsg(QObject::tr("%1的文件解压到目录%2成功").arg(pathFileName).arg(destpath));

        // 发送解压完成信号
        emit finish(destpath);

        return true;
    }
    else
    {
        qDebug() << pathFileName << QObject::tr("的文件解压到目录%1失败， 无图像文件").arg(destpath);
        emit signalParsedMsg(QObject::tr("%1的文件解压到目录%2失败， 无图像文件").arg(pathFileName).arg(destpath));
        return false;
    }
}

QString File_decompress::getRarAddressname() { return Rar_address;}
QString File_decompress::getImagePath() { return pathFileName;}
