#ifndef FILE_OPERATION_H
#define FILE_OPERATION_H

#include <QFile>
#include <QDir>
#include <QFileInfoList>
#include <QThread>
#include <QVector>

/**
 * 本地文件操作类（NAS磁盘挂载后也可使用）
 * @author 范翔
 * @date 2014-08-07
 * @version 1.0.0
 */
class LocalFileOperation
{

public:
    LocalFileOperation() { };
	
    virtual ~LocalFileOperation() { };

    /**
     * 将正斜杠路径转换为反斜杠路径 主要针对AxObject操作EXCEL输出，Excel的SaveAs和AddPicture函数
     * 例如 "D:/data.xls" -> "D:\\data.xls"
     * @param inputstr 输入string
     * @return 输出string
     */
    static QString toBackSlashStr(QString inputstr)
    {
        QStringList slist = inputstr.split("/");
        QString retstr;
        
        if (slist.length() >= 1)
            retstr = slist.at(0);
        else
            return "";

        for (int i = 1; i < slist.length(); i++)
        {
            retstr += "\\" + slist.at(i);
        }
        //qDebug() << "input string:" << inputstr << ", output string:" << retstr;
        return retstr;
    }

    /**
     * 拷贝目录及目录下文件
     * @param fromDir 源目录
     * @param toDir 目标目录
     * @param bCoverifFileExists true 同名时覆盖，flase，同名时返回false，终止拷贝
     */
    static bool copyDirectoryFiles(const QDir& fromDir, const QDir & toDir, bool bCoverifFileExists)
    {
        if (!toDir.exists())
        {
             if (!toDir.mkdir(toDir.absolutePath()))
                  return false;
        }

        QFileInfoList fileinfolist = fromDir.entryInfoList();
        foreach (QFileInfo fileinfo, fileinfolist)
        {
             if (fileinfo.fileName().compare(".") == 0 || fileinfo.fileName().compare("..") == 0)
                  continue;

             // 拷贝子目录
             if (fileinfo.isDir())
             {
                  // 递归调用拷贝
                  if (!copyDirectoryFiles(fileinfo.filePath(), toDir.filePath(fileinfo.fileName()), bCoverifFileExists))
                       return false;
             }
             else // 拷贝子文件
             {
                  if (bCoverifFileExists && toDir.exists(fileinfo.fileName()))
                  {
                       QDir toDir_ = toDir;
                       toDir_.remove(fileinfo.fileName());
                  }
                  if (!filecopy(fileinfo.filePath(), toDir.filePath(fileinfo.fileName())))
                  {
                       return false;
                  }
             }
        }
        return true;
    };

    /**
     * 删除文件夹及其中的内容
     */
    static bool removeDirwithContent(const QString &dirName)
    {
        static QVector<QString> dirNames;
        QDir dir;
        QFileInfoList filst;
        QFileInfoList::iterator curFi;
        //初始化
        dirNames.clear();
        if(dir.exists()){
            dirNames<<dirName;
        }
        else {
            return true;
        }
        //遍历各级文件夹，并将这些文件夹中的文件删除
        for(int i=0;i<dirNames.size();++i)
        {
            dir.setPath(dirNames[i]);
            filst=dir.entryInfoList(QDir::Dirs|QDir::Files
                                    |QDir::Readable|QDir::Writable
                                    |QDir::Hidden|QDir::NoDotAndDotDot
                                    ,QDir::Name);
            if (filst.size()>0){
                curFi=filst.begin();
                while (curFi!=filst.end())
                {
                    //遇到文件夹,则添加至文件夹列表dirs尾部
                    if(curFi->isDir())
                    {
                        dirNames.push_back(curFi->filePath());
                    }
                    else if (curFi->isFile())
                    {
                        //遇到文件,则删除之
                        if (!dir.remove(curFi->fileName()))
                        {
                            return false;
                        }
                    }
                    curFi++;
                }// end of while
            }
        }
        // 删除文件夹
        for (int i=dirNames.size()-1; i>=0; --i)
        {
            dir.setPath(dirNames[i]);
            if (!dir.rmdir("."))
            {
                return false;
            }
        }
        return true;
    };

    /**
     * 单文件copy
     * @param fromDir 源路径（整路径，含文件名）
     * @param toDir 目标路径（整路径，含文件名）
     */
    static bool filecopy(QString fromDir, QString toDir)
    {
        return QFile::copy(fromDir, toDir);  
    };
    
    /**
     * 将一个文件夹下的某个文件复制保存到另一个文件夹下
     * @return 0  拷贝成功
     *         -1 相同目录 不用拷贝
     *         -2 源目录不存在
     *         -3 目标目录不存在且无法创建
     *         -4 未知错误（非上述3个）无法拷贝
     * @author 熊雪
     */
    static int copyFileToPath(QString sourceDir, QString toDir, bool coverFileIfExist)
    {
        toDir.replace("\\","/");
        if (sourceDir == toDir)
        {
            return -1;
        }
        if (!QFile::exists(sourceDir))
        {
            return -2;
        }
        QDir createfile;
        bool exist = createfile.exists(toDir);
        if (exist)
        {
            if(coverFileIfExist){
                createfile.remove(toDir);
            }
        }
        else
        {
            if (!createfile.mkpath(QFileInfo(toDir).path()))
                return -3;
        }

        if (!filecopy(sourceDir,toDir))
        {
            return -4;
        }
        return 0;
    };
};

/**
 * 备份线程类
 * @author 范翔
 * @version 1.0.0
 * @date 2014-08-05
 */
class LzBackupThread : public QThread 
{
    Q_OBJECT

signals:
    /**
     * 线程运行结束消息
     * @param isok 0 正常备份结束，反馈结果
     *            -1 暂停结束
     *            -2 "source file same with destination file";
     *            -3 "no source file"
     *            -4 "cannot create to destination dir"
     *            -5 "cannot copy: may not connect to nas!"
     *            -6 "has not init backup thread!"
     *            -7 "cannot open source file"
     *            -8 "cannot open destination file"
     *            -9 "other error, cannot retriveblock"
     * @param filename 文件名
     * @param suspendfilepos 文件暂停中断位置
     */
    void finish(int isok, QString filename, qint64 suspendfilepos);

    /**
     * 线程运行过程中消息
     * @param filename 文件名
     * @param currentfilepos 当前文件正在备份中的文件位置
     */
    void statusShow(qint64 currentfilepos, QString filename);

public:

    LzBackupThread(QObject * parent = 0) : QThread(parent) { hasinit = false; ifsuspend = false; }

    virtual ~LzBackupThread() { }

    /**
     * 初始化文件拷贝线程
     * @param initsourceDir
     * @param inittodir
     * @param initcoverFileIfExist 是否在拷贝时覆盖原有文件
     * @param isinterrupt 是否从中断位置继续备份
     * @param interruptfilepos if (isinterrupt == true) 文件上次中断位置，这次从下一个位置开始备份
                               else 无意义，可赋为0
     */
    void initCopy(QString initsourceDir, QString inittodir, bool initcoverFileIfExist, bool isinterrupt, qint64 interruptfilepos)
    {
        sourceDir = initsourceDir;
        QFileInfo info(sourceDir);
        filename = info.fileName();
        toDir = inittodir;
        coverFileIfExist = initcoverFileIfExist;
        isfrominterruptfile = isinterrupt;
        interruptedfilepos = interruptfilepos;

        hasinit = true;
    }

    /**
     * 当前所有线程暂停计算
     */
    bool suspendThread()
    {
        if (ifsuspend == false)
        {
            ifsuspend = true;
            return true;
        }
        else
            return false;
    }

protected:

    virtual void run()
    {
        // 初始时不中断设置
        ifsuspend = false;

        if (!hasinit)
        {
            emit finish(-6, filename, 0);
            return;
        }
        toDir.replace("\\", "/");
        if (sourceDir == toDir)
        {
            emit finish(-2, filename, 0);
            return;
        }
        if (!QFile::exists(sourceDir))
        {
            emit finish(-3, filename, 0);
            return;
        }
        QDir createfile;
        bool exist = createfile.exists(toDir);
        if (exist)
        {
            if (coverFileIfExist){
                createfile.remove(toDir);
            }
        }
        else
        {
            if (!createfile.mkpath(QFileInfo(toDir).path()))
            {
                emit finish(-4, filename, 0);
                return;
            }
        }

        if (!LocalFileOperation::filecopy(sourceDir, toDir))
        {
            emit finish(-5, filename, 0);
            return;
        }

        emit finish(0, filename, 0);
        return;
    }

    bool hasinit;

    // 拷贝用参数
    QString sourceDir;
    QString filename;
    QString toDir;
    bool coverFileIfExist;

    // 是否暂停
    bool ifsuspend;

    // 是否是从上次暂停计算位置继续计算
    bool isfrominterruptfile;
    // if (isfrominterruptfile == true) 上次终止位置
    qint64 interruptedfilepos;

};

#endif // FILE_OPERATION