#ifndef FILE_DECOMPRESS_H
#define FILE_DECOMPRESS_H

#include <QObject>
#include <QMutex>

/**
 * RAR文件解压类接口声明
 * @author 熊雪
 * @author 范翔
 * @date 20140818
 * @version 1.0.0
 */
class File_decompress : public QObject
{
    Q_OBJECT

private:
    /**
     * RAR文件解压类构造函数
     */
    explicit File_decompress(QObject *parent = 0);
    
    // singleton设计模式，静态私有实例变量
    static File_decompress * file_decompressInstance;

public:
    /**
     * singleton设计模式，公有静态方法返回单一实例
     * @return 单一实例
     */
    static File_decompress * getFileDecompressDAOInstance();

    /**
     * 解压源图像文件
     */
    bool deCompress();
    /**
     **设置图像存放路径
     */
    void setPathFileName(QString newpath);

    /**
     * 得到Rar.exe路径名
     */
    QString getRarAddressname();
    /**
     * 得到图像存储路径
     */
    QString getImagePath();

private:
    /**
     * 图像路径变量
     */
    QString pathFileName;
    /**
     * 解压图像命令Rar.exe存放路径
     */
    QString Rar_address;

    bool hasinitpath;

    QMutex rarmutex;

signals:

    void signalParsedMsg(QString);
    /**
     * 解压完成信号
     */
    void finish(QString);
};

#endif // FILE_DECOMPRESS_H
